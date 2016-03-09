#include "primitives.h"
#include "math-toolkit.h"
#include <string.h>

static int raySphereIntersection(const object *obj,
                                 const point3 ray_e,
                                 const point3 ray_d,
                                 intersection *ip,
                                 double *t1);
static int rayRectangleIntersection(const object *obj,
                                      const point3 ray_e,
                                      const point3 ray_d,
                                      intersection *ip,
                                      double *t1);
static int rayTriangleIntersection(const object *obj,
                                      const point3 ray_e,
                                      const point3 ray_d,
                                      intersection *ip,
                                      double *t1);
static void cloneSphere(const object *src, object *dest);
static void cloneRectangle(const object *src, object *dest);
static void cloneTriangle(const object *src, object *dest);

object_virtual_table vt_sphere={.object_id=0, .rayIntersection=raySphereIntersection, .clone=cloneSphere, .private_data_size = sizeof(sphere)-sizeof(object)};
object_virtual_table vt_rectangle={.object_id=1, .rayIntersection=rayRectangleIntersection, .clone=cloneRectangle, .private_data_size = sizeof(rectangle)-sizeof(object)};
object_virtual_table vt_triangle={.object_id=2, .rayIntersection=rayTriangleIntersection, .clone=cloneTriangle, .private_data_size = sizeof(triangle)-sizeof(object)};

/* @param t t distance
 * @return 1 means hit, otherwise 0
 */
static int raySphereIntersection(const object *obj,
                                 const point3 ray_e,
                                 const point3 ray_d,
                                 intersection *ip,
                                 double *t1)
{
    const sphere *sph = (const sphere *)obj;
    point3 l;
    subtract_vector(sph->center, ray_e, l);
	double s = dot_product(l, ray_d);
	double l2 = dot_product(l, l);
	double r2 = sph->radius*sph->radius;

	if(s<0 && l2 > r2)
		return 0;
	float m2 = l2 - s*s;
	if(m2>r2)
		return 0;
	float q = sqrt(r2 - m2);
	if(l2>r2)
		*t1 = s - q;
	else
		*t1 = s + q;
    // p=e+t1*d
    multiply_vector(ray_d, *t1, ip->point);
    add_vector(ray_e, ip->point, ip->point);

    subtract_vector(ip->point, sph->center, ip->normal);
    normalize(ip->normal);
    if(dot_product(ip->normal, ray_d)>0.0)
        multiply_vector(ip->normal, -1, ip->normal);
      
	return 1;
}

/* @return 1 means hit, otherwise 0; */
static int rayRectangleIntersection(const object *obj,
                                      const point3 ray_e,
                                      const point3 ray_d,
                                      intersection *ip,
                                      double *t1)
{
    const rectangle *rec = (const rectangle *)obj;

    point3 e01, e03, p;
    subtract_vector(rec->vertices[1], rec->vertices[0], e01);
    subtract_vector(rec->vertices[3], rec->vertices[0], e03);

    cross_product(ray_d, e03, p);

    double det = dot_product(e01, p);

    // Reject rays orthagonal to the normal vector. I.e. rays parallell to the plane.
    if(det < 1e-4)
        return 0;
    
    double inv_det = 1.0/det;

    point3 s;
    subtract_vector(ray_e, rec->vertices[0], s);

    double alpha = inv_det * dot_product(s, p);

    if(alpha > 1.0)
        return 0;
    if(alpha < 0.0)
        return 0;

    point3 q;
    cross_product(s, e01, q);

    double beta = inv_det * dot_product(ray_d, q);
    if(beta > 1.0)
        return 0;
    if(beta < 0.0)
        return 0;

    *t1 = inv_det * dot_product(e03, q);

    if(alpha + beta > 1.0f) {
        /* for the second triangle */
        point3 e23, e21;
        subtract_vector(rec->vertices[3], rec->vertices[2], e23);
        subtract_vector(rec->vertices[1], rec->vertices[2], e21);
        
        cross_product(ray_d, e21, p);

        det = dot_product(e23, p);

        if(det < 1e-4)
            return 0;

        inv_det = 1.0/det;
        subtract_vector(ray_e, rec->vertices[2], s);
        
        alpha = inv_det * dot_product(s, p);
        if(alpha < 0.0)
            return 0;
        
        cross_product(s, e23, q);
        beta = inv_det * dot_product(ray_d, q);

        if(beta < 0.0)
            return 0;

        if(beta + alpha > 1.0)
            return 0;
 
        *t1 = inv_det * dot_product(e21, q);
    }
    
    if(*t1 < 1e-4)
        return 0;
    
    cross_product(e01, e03, ip->normal);
    normalize(ip->normal);
    if(dot_product(ip->normal, ray_d)>0.0)
        multiply_vector(ip->normal, -1, ip->normal);
    multiply_vector(ray_d, *t1, ip->point);
    add_vector(ray_e, ip->point, ip->point);

    return 1;
}

static int rayTriangleIntersection(const object *obj,
                                      const point3 ray_e,
                                      const point3 ray_d,
                                      intersection *ip,
                                      double *t1)
{
    const triangle *tri = (const triangle *)obj;

    point3 e01, e02, p;
    subtract_vector(tri->vertices[1], tri->vertices[0], e01);
    subtract_vector(tri->vertices[2], tri->vertices[0], e02);

    cross_product(ray_d, e02, p);

    double det = dot_product(e01, p);

    // Reject rays orthagonal to the normal vector. I.e. rays parallell to the plane.
    if(det < 1e-4)
        return 0;
    
    double inv_det = 1.0/det;

    point3 s;
    subtract_vector(ray_e, tri->vertices[0], s);

    double alpha = inv_det * dot_product(s, p);

    if(alpha > 1.0)
        return 0;
    if(alpha < 0.0)
        return 0;

    point3 q;
    cross_product(s, e01, q);

    double beta = inv_det * dot_product(ray_d, q);
    if(beta > 1.0)
        return 0;
    if(beta < 0.0)
        return 0;
    if(alpha+beta>1.0)
        return 0;

    *t1 = inv_det * dot_product(e02, q);

    if(*t1 < 1e-4)
        return 0;
    
    cross_product(e01, e02, ip->normal);
    normalize(ip->normal);
    if(dot_product(ip->normal, ray_d)>0.0)
        multiply_vector(ip->normal, -1, ip->normal);
    multiply_vector(ray_d, *t1, ip->point);
    add_vector(ray_e, ip->point, ip->point);

    return 1;
}

static void cloneSphere(const object *src, object *dest)
{
    memcpy(dest, src, sizeof(sphere));
}

static void cloneRectangle(const object *src, object *dest)
{
    memcpy(dest, src, sizeof(rectangle));
}

static void cloneTriangle(const object *src, object *dest)
{
    memcpy(dest, src, sizeof(triangle));
}