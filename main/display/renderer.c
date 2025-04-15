#include <stdio.h>
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "display/renderer.h"
#include "display/assets.h"
#include "display/tubes_image.h"
#include "common.h"

lv_draw_img_dsc_t draw_dsc;
static lv_obj_t *canvas = NULL;

EXT_RAM_BSS_ATTR static lv_color_t canvas_buf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(SCREEN_WIDTH, SCREEN_HEIGHT)];
EXT_RAM_BSS_ATTR static lv_color_t canvas_cache[LV_CANVAS_BUF_SIZE_TRUE_COLOR(SCREEN_WIDTH, SCREEN_HEIGHT)];

void create_canvas(void)
{
    // Create a canvas object on the active screen
    canvas = lv_canvas_create(lv_scr_act());

    lv_canvas_set_buffer(canvas, canvas_buf, SCREEN_WIDTH, SCREEN_HEIGHT, LV_IMG_CF_TRUE_COLOR);

    lv_draw_img_dsc_init(&draw_dsc);

    lv_canvas_draw_img(canvas, 0, 0, &background, &draw_dsc);
    memcpy(canvas_cache, canvas_buf, sizeof(canvas_buf));
}

void render_scene(void)
{
    bsp_display_lock(50);
    // Clear the canvas by filling the entire area with black.
    memcpy(canvas_buf, canvas_cache, sizeof(canvas_buf));
    // Loop over the pipe positions and draw each pipe image
    for (int i = 0; i < NUM_TUBES; i++) {
        lv_canvas_draw_img(canvas, tubes[i].x_position, tubes[i].y_position, tubes[i].image, &draw_dsc);
    }

    lv_canvas_draw_img(canvas, 160, 130, &plane, &draw_dsc);

    lv_task_handler();
    bsp_display_unlock();
}