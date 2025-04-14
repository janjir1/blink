#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "display/tubes_image.h"

#define NUM_TUBES 6
#define ORIGINAL_IMAGE tube_top

typedef struct {
    //lv_img_dsc_t transformed_sprite;
    int16_t x_position;
    int16_t y_position;
} tube_t;


extern tube_t tubes[NUM_TUBES];