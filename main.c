#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "primitives.h"
#include "raytracing.h"
#include "obj_parser.h"
#include "math-toolkit.h"

#define OUT_FILENAME "out.ppm"

#define ROWS 512
#define COLS 512
#define MAX_VERTICES 4096

static void write_to_ppm(FILE *outfile, uint8_t *pixels,
                         int width, int height)
{
    fprintf(outfile, "P6\n%d %d\n%d\n", width, height, 255);
    fwrite(pixels, 1, height * width * 3, outfile);
}

static int load_obj_scene(const char *filename, point3 translate, point3 rotate, point3 scale, object_node *objects)
{
    static point3 tmp1[MAX_VERTICES],  tmp2[MAX_VERTICES];
    obj_scene_data data;

    printf("# Loading obj scene...");
    if( !parse_obj_scene(&data, filename) )
    {
        printf("Failed\n");
        return 0;
    }
    obj_material *mtl;
    for(int i=0; i<data.face_count; i++)
    {
        obj_face *o = data.face_list[i];
        for(int j=0; j<3; j++)
        {
            COPY_POINT3(tmp1[i*3+j], data.vertex_list[ o->vertex_index[j]]->e);
            COPY_POINT3(tmp2[i*3+j], data.vertex_normal_list[ o->normal_index[j]]->e);
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

int main()
{
    uint8_t *pixels;
    light_node lights = NULL;
    object_node objects = NULL;
    color background = { 0.0, 0.0, 0.0 };

#include "use-models.h"

    /* allocate by the given resolution */
    pixels = malloc(sizeof(unsigned char) * ROWS * COLS * 3);
    if (!pixels) exit(-1);

    printf("# Rendering scene\n");
    /* do the ray tracing with the given geometry */
    raytracing(pixels, background,
               objects, lights, &view, ROWS, COLS);
    {
        FILE *outfile = fopen(OUT_FILENAME, "wb");
        write_to_ppm(outfile, pixels, ROWS, COLS);
        fclose(outfile);
    }

    delete_object_list(&objects);
    delete_light_list(&lights);
    printf("Done!\n");
    return 0;
}
