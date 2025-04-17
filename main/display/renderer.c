#include <stdio.h>
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "display/renderer.h"
#include "display/assets.h"
#include "display/tubes_image.h"
#include "common.h"
#include "esp_timer.h"
//#include "lv_misc/lv_math.h"
/*
#ifndef LV_MIN
  #define LV_MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef LV_MAX
  #define LV_MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif
*/

lv_draw_img_dsc_t draw_dsc;
static lv_obj_t *canvas = NULL;

//EXT_RAM_BSS_ATTR
EXT_RAM_BSS_ATTR static lv_color_t canvas_buf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(SCREEN_WIDTH, SCREEN_HEIGHT)];
//EXT_RAM_BSS_ATTR lv_color_t canvas_buf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(SCREEN_WIDTH, SCREEN_HEIGHT)];
EXT_RAM_BSS_ATTR static lv_color_t canvas_cache[LV_CANVAS_BUF_SIZE_TRUE_COLOR(SCREEN_WIDTH, SCREEN_HEIGHT)];

volatile bool make_background_flag = false;



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
    memcpy(canvas_buf+4096, canvas_cache, 4096); //30ms
    
    //lv_canvas_draw_img(canvas, 0, 0, &background, &draw_dsc); //20ms

    //10ms
    for (int i = 0; i < NUM_TUBES; i++) {
        lv_canvas_draw_img(canvas, tubes[i].x_position, tubes[i].y_position, tubes[i].image, &draw_dsc);
    }

    lv_canvas_draw_img(canvas, plane_inst.x_position, plane_inst.y_position, plane_inst.image, &draw_dsc);

    //lv_obj_invalidate(canvas);
    //lv_refr_now(NULL);


    lv_task_handler();  //20ms
    bsp_display_unlock();

}
/*
void render_scene(void) {

     Initialize dirty_area to the first tube’s union 
    lv_area_t dirty_area = {
        .x1 = LV_MIN(tubes[0].x_position, tubes[0].x_position_next),
        .y1 = LV_MIN(tubes[0].y_position, tubes[0].y_position_next),
        .x2 = LV_MAX(tubes[0].x_position + tubes[0].image->header.w,
                        tubes[0].x_position_next + tubes[0].image->header.w) - 1,
        .y2 = LV_MAX(tubes[0].y_position + tubes[0].image->header.h,
                        tubes[0].y_position_next + tubes[0].image->header.h) - 1
    };

     Extend for each tube 
    for(int i = 1; i < NUM_TUBES; i++) {
        int x_min = LV_MIN(tubes[i].x_position, tubes[i].x_position_next);
        int y_min = LV_MIN(tubes[i].y_position, tubes[i].y_position_next);
        int x_max = LV_MAX(tubes[i].x_position + tubes[i].image->header.w,
                                tubes[i].x_position_next + tubes[i].image->header.w);
        int y_max = LV_MAX(tubes[i].y_position + tubes[i].image->header.h,
                                tubes[i].y_position_next + tubes[i].image->header.h);
        dirty_area.x1 = LV_MIN(dirty_area.x1, x_min);
        dirty_area.y1 = LV_MIN(dirty_area.y1, y_min);
        dirty_area.x2 = LV_MAX(dirty_area.x2, x_max - 1);
        dirty_area.y2 = LV_MAX(dirty_area.y2, y_max - 1);
    }

     Also include the plane if it moves: 
    {
        int px_min = LV_MIN(plane_inst.x_position, plane_inst.x_position_next);
        int py_min = LV_MIN(plane_inst.y_position, plane_inst.y_position_next);
        int px_max = LV_MAX(plane_inst.x_position + plane_inst.image->header.w,
                                plane_inst.x_position_next + plane_inst.image->header.w);
        int py_max = LV_MAX(plane_inst.y_position + plane_inst.image->header.h,
                                plane_inst.y_position_next + plane_inst.image->header.h);
        dirty_area.x1 = LV_MIN(dirty_area.x1, px_min);
        dirty_area.y1 = LV_MIN(dirty_area.y1, py_min);
        dirty_area.x2 = LV_MAX(dirty_area.x2, px_max - 1);
        dirty_area.y2 = LV_MAX(dirty_area.y2, py_max - 1);
    }

    bsp_display_lock(50);
    
    lv_color_t *dst_buf = (lv_color_t *)lv_canvas_get_buf(canvas);
    const lv_color_t *src = canvas_cache;
    const uint32_t w      = lv_obj_get_width(canvas);
    uint32_t row_bytes    = (dirty_area.x2 - dirty_area.x1 + 1) * sizeof(lv_color_t);
    for(int y = dirty_area.y1; y <= dirty_area.y2; y++) {
        lv_color_t *dst = dst_buf + y * w + dirty_area.x1;
        const lv_color_t *line_src = src + y * w + dirty_area.x1;
        memcpy(dst, line_src, row_bytes);
    }

     3) Draw moved tubes & plane 
    for(int i = 0; i < NUM_TUBES; i++) {
        lv_canvas_draw_img(canvas,
                           tubes[i].x_position_next,
                           tubes[i].y_position_next,
                           tubes[i].image,
                           &draw_dsc);
    }
    lv_canvas_draw_img(canvas,
                       plane_inst.x_position_next,
                       plane_inst.y_position_next,
                       plane_inst.image,
                       &draw_dsc);

    4) Invalidate only dirty_area and flush 
    lv_obj_invalidate_area(canvas, &dirty_area);
    lv_task_handler();

     5) Store new positions for next frame 
    for(int i = 0; i < NUM_TUBES; i++) {
        tubes[i].x_position = tubes[i].x_position_next;
        tubes[i].y_position = tubes[i].y_position_next;
    }
    plane_inst.x_position = plane_inst.x_position_next;
    plane_inst.y_position = plane_inst.y_position_next;

    bsp_display_unlock();
}
*/