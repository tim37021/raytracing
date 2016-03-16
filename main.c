#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>

#include "primitives.h"
#include "raytracing.h"
#include "obj_parser.h"
#include "math-toolkit.h"

#define OUT_FILENAME "out.ppm"

#define ROWS 512
#define COLS 512
#define MAX_VERTICES 4096

int keep_running = 1;

static void write_to_ppm(FILE *outfile, uint8_t *pixels,
                         int width, int height)
{
    fprintf(outfile, "P6\n%d %d\n%d\n", width, height, 255);
    fwrite(pixels, 1, height * width * 3, outfile);
}

static inline
void build_rotation_matrix(const double *rotation, double *out)
{
    mat3 tmp, tmp2;
    rotation_matrix_x(rotation[0], out);
    rotation_matrix_y(rotation[1], tmp);
    multiply_mat3(tmp, out, tmp2);
    rotation_matrix_z(rotation[2], tmp);
    multiply_mat3(tmp, tmp2, out);
}

static int load_obj_scene(const char *filename, point3 translate, point3 rotate, point3 scale, object_node *objects)
{
    static point3 tmp1[MAX_VERTICES],  tmp2[MAX_VERTICES];
    obj_scene_data data;

    printf("# Loading obj scene...");
    if( !parse_obj_scene(&data, filename) ) {
        printf("Failed\n");
        return 0;
    }

    mat3 rotate_mat;
    build_rotation_matrix(rotate, rotate_mat);
    mat3 rotate_scale_mat, scale_matrix;
    scalar_matrix(scale, scale_matrix);
    multiply_mat3(rotate_mat, scale_matrix, rotate_scale_mat);

    obj_material *mtl;
    for(int i=0; i<data.face_count; i++) {
        obj_face *o = data.face_list[i];
        for(int j=0; j<3; j++) {
            multiply_mat3_vector(rotate_scale_mat, data.vertex_list[ o->vertex_index[j]]->e, tmp1[i*3+j]);
            tmp1[i*3+j][0] += translate[0];
            tmp1[i*3+j][1] += translate[1];
            tmp1[i*3+j][2] += translate[2];
            multiply_mat3_vector(rotate_mat, data.vertex_normal_list[ o->normal_index[j]]->e, tmp2[i*3+j]);
        }
        mtl = data.material_list[o->material_index];
    }
    object_fill fill;
    COPY_COLOR(fill.fill_color, mtl->diff);
    fill.Kd = length(mtl->diff);
    fill.Ks = length(mtl->spec);
    fill.T = 0.0;
    fill.R = 0.0;
    fill.index_of_refraction=0.0;
    fill.phong_power = 180;
    triangle_mesh newObject = newTriangleMesh(data.face_count, tmp1, tmp2, fill);
    append_object((object *)&newObject, objects);

    printf("OK\n");

    delete_obj_data(&data);
    return 1;
}

static void signal_handler(int parameter)
{
    keep_running = 0;
}

static int progrss_report(float percent)
{
    printf("\r[ %.0f%% ]", percent);
    fflush(stdout);
    return keep_running;
}

static int iter_report(int iter)
{
    printf("\nIteration %d done\n", iter);
    fflush(stdout);
    return keep_running;
}

int main()
{
    uint8_t *pixels;
    light_node lights = NULL;
    object_node objects = NULL;
    color background = { 0.0, 0.0, 0.0 };

    /* setup signal handler */
    signal(SIGINT, signal_handler);

#include "use-models.h"

    /* allocate by the given resolution */
    pixels = malloc(sizeof(unsigned char) * ROWS * COLS * 3);
    if (!pixels) exit(-1);

    printf("# Rendering scene\n");
    /* do the ray tracing with the given geometry */
    pathtracing(pixels, background,
               objects, lights, &view, ROWS, COLS, progrss_report, iter_report);
    {
        FILE *outfile = fopen(OUT_FILENAME, "wb");
        write_to_ppm(outfile, pixels, ROWS, COLS);
        fclose(outfile);
    }

    delete_object_list(&objects);
    delete_light_list(&lights);
    printf("\nDone!\n");
    return 0;
}
