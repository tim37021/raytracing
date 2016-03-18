#ifndef __RAYTRACING_H
#define __RAYTRACING_H

#include "objects.h"
#include <stdint.h>

/* return true if you wish to keep running */
typedef int (*event_progress_change)(float percent);
typedef int (*event_after_iteration)(int iter, uint8_t *pixels);

void raytracing(uint8_t *pixels, color background_color,
                object_node rectangulars, light_node lights,
                const viewpoint *view,
                int width, int height, event_progress_change event_progress);

void pathtracing(uint8_t *pixels, color background_color,
                object_node rectangulars, light_node lights,
                const viewpoint *view,
                int width, int height, event_progress_change event_progress, event_after_iteration event_iter);
#endif
