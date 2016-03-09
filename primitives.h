#ifndef __RAY_PRIMITIVES_H
#define __RAY_PRIMITIVES_H

typedef double point3[3];
typedef double point4[3];
typedef double color[3];

typedef struct {
    color light_color; /**< scale (0,1) */
    point3 position;
    double intensity;
} light;

typedef struct {
    color fill_color; /**< RGB is in terms of 0.0 to 1.0 */
    double Kd; /**< the diffuse component */
    double Ks; /**< the specular */
    double T;  /**< transmittance (fraction of light passed per unit) */
    double R;  /**< reflectance (effectiveness in reflecting)*/
    double index_of_refraction;
    double phong_power; /**< the Phong cosine power for highlights */
} object_fill;

/* forward declare is necessary */
struct _OBJECT;
struct _INTERSECTION;
typedef struct _INTERSECTION intersection;
typedef struct _OBJECT object;
typedef struct {
    void (*clone)(const object *this_ptr, object *target);
    int (*rayIntersection)(const struct _OBJECT *this_ptr, const point3 ray_e, const point3 ray_d, intersection *ip, double *t);
    /* desctuctor */
    void (*release)();
    /* though this is not a function pointer I put it here */
    int object_id;
    int private_data_size;
} object_virtual_table;

typedef struct _OBJECT{
    object_virtual_table *vt;
    object_fill fill;
    char private_data[];
} object;

extern object_virtual_table vt_sphere;
extern object_virtual_table vt_rectangle;
extern object_virtual_table vt_triangle;

typedef struct {
    object_virtual_table *vt;
    object_fill fill;
    point3 center;
    double radius;
} sphere;

typedef struct {
    object_virtual_table *vt;
    object_fill fill;
    point3 vertices[4];
} rectangle;

typedef struct {
    object_virtual_table *vt;
    object_fill fill;
    point3 vertices[3];
} triangle;

typedef struct {
    object_virtual_table *vt;
    object_fill fill;
    /* number of tris */
    int n;
    point3 *vertices;
    point3 *normals;
} triangle_mesh;

triangle_mesh newTriangleMesh(int n, point3 *vertex_list, point3 *normal_list, object_fill fill);

typedef struct {
    point3 vrp;
    point3 vpn;
    point3 vup;
} viewpoint;

typedef struct _INTERSECTION{
    point3 point;
    point3 normal;
} intersection;

#define COPY_POINT3(a,b)	{ (a)[0] = (b)[0]; \
	(a)[1] = (b)[1]; \
	(a)[2] = (b)[2]; }

#define COPY_COLOR(a,b)		{ (a)[0] = (b)[0]; \
	(a)[1] = (b)[1]; \
	(a)[2] = (b)[2]; }

#define SET_COLOR(r,R,G,B)	{ (r)[0] = (R); \
	(r)[1] = (G); \
	(r)[2] = (B); }

#endif
