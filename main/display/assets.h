#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "display/tubes_image.h"

#define NUM_TUBES 6
#define ORIGINAL_IMAGE tube_bottom

typedef struct {
    int16_t x_position;
    int16_t y_position;
    const lv_img_dsc_t *image;
} tube_t;


extern tube_t tubes[NUM_TUBES];