#ifndef __RAYTRACING_H
#define __RAYTRACING_H

#include "objects.h"
#include <stdint.h>

typedef void (*event_progress_change)(float percent);

void raytracing(uint8_t *pixels, color background_color,
                object_node rectangulars, light_node lights, 
                const viewpoint *view,
                int width, int height, event_progress_change event_progress);
#endif
