#ifndef __RAYTRACING_H
#define __RAYTRACING_H

#include "objects.h"
#include <stdint.h>

void raytracing(uint8_t *pixels, color background_color,
                object_node rectangulars, light_node lights, 
                const viewpoint *view,
                int width, int height);
#endif
