#include <stdio.h>
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "display/renderer.h"
#include "display/assets.h"
#include "display/tubes_image.h"
#include "common.h"

lv_draw_img_dsc_t draw_dsc;
static lv_obj_t *canvas = NULL;

void create_canvas(void)
{
    // Create a canvas object on the active screen
    canvas = lv_canvas_create(lv_scr_act());

    EXT_RAM_BSS_ATTR static lv_color_t canvas_buf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(SCREEN_WIDTH, SCREEN_HEIGHT)];

    lv_canvas_set_buffer(canvas, canvas_buf, SCREEN_WIDTH, SCREEN_HEIGHT, LV_IMG_CF_TRUE_COLOR);

    lv_draw_img_dsc_init(&draw_dsc);
}

void render_scene(void)
{
    // Clear the canvas by filling the entire area with black.
    lv_canvas_fill_bg(canvas, lv_color_hex(0x33a2ff), LV_OPA_COVER);
    // Loop over the pipe positions and draw each pipe image
    for (int i = 0; i < NUM_TUBES; i++) {
        lv_canvas_draw_img(canvas, tubes[i].x_position, tubes[i].y_position, &ORIGINAL_IMAGE, &draw_dsc);
    }
}