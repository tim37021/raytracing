#include <stdio.h>
#include <stdlib.h>

#include "math-toolkit.h"
#include "primitives.h"
#include "raytracing.h"

#define MAX_REFLECTION_BOUNCES	3
#define MAX_DISTANCE 1000000000000.0
#define MIN_DISTANCE 0.00001

#define SQUARE(x) (x * x)
#define MAX(a, b) (a > b ? a : b)

static void localColor(color local_color,
                       const color light_color, double diffuse,
                       double specular, const object_fill *fill)
{
    color ambi = { 0.1, 0.1, 0.1 };
    color diff, spec, lightCo,surface;

    /* Local Color = ambient * surface +
     *               light * ( kd * surface * diffuse + ks * specular)
     */

    COPY_COLOR(diff, fill->fill_color);
    multiply_vector(diff, fill->Kd, diff);
    multiply_vector(diff, diffuse, diff);
    COPY_COLOR(lightCo, light_color);
    multiply_vectors(diff, lightCo, diff);

    COPY_COLOR(spec, light_color);
    multiply_vector(spec, fill->Ks, spec);
    multiply_vector(spec, specular, spec);

    COPY_COLOR(surface, fill->fill_color);
    multiply_vectors(ambi,surface, ambi);
    add_vector(diff, ambi, diff);
    add_vector(diff, spec, diff);
    add_vector(local_color, diff, local_color);
}

/* @param d direction of the ray into intersection
 * @param l direction of intersection to light
 * @param n surface normal
 */
static void compute_specular_and_diffuse(double *diffuse, double *specular,
        const point3 d, const point3 l,
        const point3 n, double phong_pow)
{
    point3 d_copy, l_copy, middle, r;

    /* Calculate vector to eye V */
    COPY_POINT3(d_copy, d);
    multiply_vector(d_copy, -1, d_copy);
    normalize(d_copy);

    /* Calculate vector to light L */
    COPY_POINT3(l_copy, l);
    multiply_vector(l_copy, -1, l_copy);
    normalize(l_copy);

    /* Calculate reflection direction R */
    double tmp = dot_product(n, l_copy);
    multiply_vector(n, tmp, middle);
    multiply_vector(middle, 2, middle);
    subtract_vector(middle, l_copy, r);
    normalize(r);

    /* diffuse = max(0, dot_product(n, -l)) */
    *diffuse = MAX(0, dot_product(n, l_copy));

    /* specular = (dot_product(r, -d))^p */
    *specular = pow(MAX(0, dot_product(r, d_copy)), phong_pow);
}

/* @param r direction of reflected ray
 * @param d direction of primary ray into intersection
 * @param n surface normal at intersection
 */
static void reflection(point3 r, const point3 d, const point3 n)
{
    /* r = d - 2(d . n)n */
    multiply_vector(n, -2.0 * dot_product(d, n), r);
    add_vector(r, d, r);
}

/* reference: https://www.opengl.org/sdk/docs/man/html/refract.xhtml */
static void refraction(point3 t, const point3 I, const point3 N, double n1, double n2)
{
    double eta = n1/n2;
    double dot_NI = dot_product(N,I);
    double k = 1.0 - eta*eta*(1.0 - dot_NI*dot_NI);
    if(k < 0.0)
        t[0]=t[1]=t[2]=0.0;
    else {
        point3 tmp;
        multiply_vector(I, eta, t);
        multiply_vector(N, eta*dot_NI+sqrt(k), tmp);
        subtract_vector(t, tmp, t);
    }
}


/* @param i direction of incoming ray, unit vector
 * @param r direction of refraction ray, unit vector
 * @param normal unit vector
 * @param n1 refraction index
 * @param n2 refraction index
 */

/* reference: http://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf */
static double fresnel(const point3 r, const point3 l, const point3 normal, double n1, double n2)
{
    // TIR
    if(length(l)<0.99)
        return 1.0;
    double cos_theta_i = -dot_product(r, normal);
    double cos_theta_t = -dot_product(l, normal);
    double r_vertical_root = (n1*cos_theta_i-n2*cos_theta_t)/(n1*cos_theta_i+n2*cos_theta_t);
    double r_parallel_root = (n2*cos_theta_i-n1*cos_theta_t)/(n2*cos_theta_i+n1*cos_theta_t);
    return (r_vertical_root*r_vertical_root+r_parallel_root*r_parallel_root)/2.0;
}

/* @param t distance */
static double ray_hit_object(const point3 e, const point3 d,
                             double t0, double t1,
                             point3 normal,
                             const object_node objects,
                             object_node *hit_object,
                             const object_node last_object)
{

    point3 biased_e;
    multiply_vector(d, t0, biased_e);
    add_vector(biased_e, e, biased_e);

    double nearest = t1;
    point3 tmpnormal;

    *hit_object=NULL;

    for (object_node obj = objects; obj; obj = obj->next) {
        if (obj == last_object)
            continue;

        if (obj->element.vt->rayIntersection(&(obj->element), biased_e, d, tmpnormal,
                                       &t1) && t1<nearest) {
            /* hit is closest so far */
            *hit_object = obj;
            nearest = t1;
            COPY_POINT3(normal, tmpnormal);
        }
    }

    return nearest;
}

/* @param d direction of ray
 * @param w basic vectors
 */
static void rayConstruction(point3 d, const point3 u, const point3 v,
                            const point3 w, unsigned int i, unsigned int j,
                            const viewpoint *view, unsigned int width,
                            unsigned int height)
{
    double xmin = -0.0175;
    double ymin = -0.0175;
    double xmax = 0.0175;
    double ymax = 0.0175;
    double focal = 0.05;

    double u_s, v_s, w_s;
    point3 u_tmp, v_tmp, w_tmp, s;

    w_s = focal;
    u_s = xmin + ((xmax - xmin) * (float)i / (width - 1));
    v_s = ymax + ((ymin - ymax) * (float)j / (height - 1));

    /* s = e + u_s * u + v_s * v + w_s * w */
    multiply_vector(u, u_s, u_tmp);
    multiply_vector(v, v_s, v_tmp);
    multiply_vector(w, w_s, w_tmp);
    add_vector(view->vrp, u_tmp, s);
    add_vector(s, v_tmp, s);
    add_vector(s, w_tmp, s);

    /* p(t) = e + td = e + t(s - e) */
    subtract_vector(s, view->vrp, d);
    normalize(d);
}

static void calculateBasisVectors(point3 u, point3 v, point3 w,
                                  const viewpoint *view)
{
    /* w  */
    COPY_POINT3(w, view->vpn);
    normalize(w);

    /* u = (t x w) / (|t x w|) */
    cross_product(w, view->vup, u);
    normalize(u);

    /* v = w x u */
    cross_product(u, w, v);

    normalize(v);
}

/* @brief protect color value overflow */
static void protect_color_overflow(color c)
{
    for (int i = 0; i < 3; i++)
        if (c[i] > 1.0) c[i] = 1.0;
}

static unsigned int ray_color(const point3 e, double t,
                              const point3 d,
                              const object_node objects,
                              const light_node lights,
                              color object_color, int bounces_left,
                              const object_node last_object)
{
    object_node hit_object = NULL, light_hit_object = NULL;
    double t1 = MAX_DISTANCE, diffuse, specular;
    point3 p, surface_normal, l, _l, ignore_me, r, rr;
    object_fill fill;

    color reflection_part;
    color refraction_part;
    /* might be a reflection ray, so check how many times we've bounced */
    if (bounces_left < 0) {
        SET_COLOR(object_color, 0.0, 0.0, 0.0);
        return 0;
    }

    /* check for intersection with a sphere or a rectangular */
    t1 = ray_hit_object(e, d, t, MAX_DISTANCE, surface_normal, objects, &hit_object, last_object);
   
    if (!hit_object)
        return 0;

    /* p = e + t * d */
    multiply_vector(d, t1, p);
    add_vector(e, p, p);

    /* pick the fill of the object that was hit */
    fill = hit_object->element.fill;

    /* assume it is a shadow */
    SET_COLOR(object_color, 0.0, 0.0, 0.0);

    for (light_node light = lights; light; light = light->next) {
        /* calculate the intersection vector pointing at the light */
        subtract_vector(p, light->element.position, l);
        multiply_vector(l, -1, _l);
        normalize(_l);
        
        /* check for intersection with an object. use ignore_me
         * because we don't care about this normal
        */
        ray_hit_object(p, _l, MIN_DISTANCE, length(l), ignore_me,
                       objects, &light_hit_object, hit_object);

        /* the light was not block by itself(lit object) */
        if (light_hit_object)
            continue;

        compute_specular_and_diffuse(&diffuse, &specular, d, l,
                                     surface_normal, fill.phong_power);

        localColor(object_color, light->element.light_color,
                  diffuse, specular, &fill);
    }

    reflection(r, d, surface_normal);
    double idx = 1.0;
    if (last_object)
        idx = last_object->element.fill.index_of_refraction;
    refraction(rr, d, surface_normal, idx, fill.index_of_refraction);
    double R = fill.T>0.1? fresnel(d, rr, surface_normal, idx, fill.index_of_refraction): 1.0;

    /* totalColor = localColor + mix((1-fill.Kd)*fill.R*reflection, T*refraction, R) */
    if (fill.R > 0) {
        /* if we hit something, add the color
        * that's a result of that */
        if (ray_color(p, MIN_DISTANCE , r, objects,
                          lights, reflection_part,
                          bounces_left - 1,
                          hit_object)) {

            multiply_vector(reflection_part, R*(1.0-fill.Kd)*fill.R,
                            reflection_part);
            add_vector(object_color, reflection_part,
                               object_color);
        }
    }


    /* calculate refraction ray */
    if (fill.T > 0.0 && fill.index_of_refraction > 0.0) {

        if (ray_color(p, MIN_DISTANCE, rr, objects,
                          lights, refraction_part,
                          bounces_left - 1, hit_object)) {
            multiply_vector(refraction_part, (1.0-R)*fill.T,
                            refraction_part);
            add_vector(object_color, refraction_part,
                           object_color);
        }
    }
    
    protect_color_overflow(object_color);
    return 1;
}

/* @param background_color this is not ambient light */
void raytracing(uint8_t *pixels, color background_color,
                object_node objects, light_node lights, 
                const viewpoint *view,
                int width, int height)
{
    point3 u, v, w, d;
    color object_color = { 0.0, 0.0, 0.0 };

    /* calculate u, v, w */
    calculateBasisVectors(u, v, w, view);

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            rayConstruction(d, u, v, w, i, j, view, width, height);
            normalize(d);
            if (ray_color(view->vrp, 0.0, d, objects,

                          lights, object_color,
                          MAX_REFLECTION_BOUNCES,
                          NULL)) {
                pixels[((i + (j*width)) * 3) + 0] = object_color[0] * 255;
                pixels[((i + (j*width)) * 3) + 1] = object_color[1] * 255;
                pixels[((i + (j*width)) * 3) + 2] = object_color[2] * 255;
            } else {
                pixels[((i + (j*width)) * 3) + 0] = background_color[0] * 255;
                pixels[((i + (j*width)) * 3) + 1] = background_color[1] * 255;
                pixels[((i + (j*width)) * 3) + 2] = background_color[2] * 255;
            }
        }
    }
}
