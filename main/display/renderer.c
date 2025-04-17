#include <stdio.h>
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "display/renderer.h"
#include "display/assets.h"
#include "display/tubes_image.h"
#include "common.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "esp_cache.h"

#define line 2*320


lv_draw_img_dsc_t draw_dsc;
static lv_obj_t *canvas = NULL;


EXT_RAM_BSS_ATTR static lv_color_t canvas_buf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(SCREEN_WIDTH, SCREEN_HEIGHT)];
//EXT_RAM_BSS_ATTR lv_color_t canvas_buf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(SCREEN_WIDTH, SCREEN_HEIGHT)];
EXT_RAM_BSS_ATTR static lv_color_t canvas_cache[LV_CANVAS_BUF_SIZE_TRUE_COLOR(SCREEN_WIDTH, SCREEN_HEIGHT)];
//static lv_color_t* canvas_buf;
//static lv_color_t* canvas_cache;

volatile bool make_background_flag = false;



void create_canvas(void)
{
    // Create a canvas object on the active screen
    //canvas_buf = heap_caps_aligned_alloc(16, LV_CANVAS_BUF_SIZE_TRUE_COLOR(SCREEN_WIDTH, SCREEN_HEIGHT), MALLOC_CAP_SPIRAM);
    //canvas_cache = heap_caps_aligned_alloc(16, LV_CANVAS_BUF_SIZE_TRUE_COLOR(SCREEN_WIDTH, SCREEN_HEIGHT), MALLOC_CAP_SPIRAM);
    
    canvas = lv_canvas_create(lv_scr_act());

    lv_canvas_set_buffer(canvas, canvas_buf, SCREEN_WIDTH, SCREEN_HEIGHT, LV_IMG_CF_TRUE_COLOR);

    lv_draw_img_dsc_init(&draw_dsc);

    lv_canvas_draw_img(canvas, 0, 0, &background, &draw_dsc);
    memcpy(canvas_cache, canvas_buf, sizeof(canvas_buf));
}


void render_scene(void)
{
    
    
    bsp_display_lock(50);   
    //memcpy(canvas_buf, canvas_cache, sizeof(canvas_cache)); //30ms
    memcpy(canvas_buf + 16*4096, canvas_cache, 2);
    memcpy(canvas_buf + 16*4096+2*640, canvas_cache, 2);
    memcpy(canvas_buf + 16*4096+4*640, canvas_cache, 2); //smallest i can do is half a line (probably psram aligment issue)
    memcpy(canvas_buf + 16*4096+6*640, canvas_cache, 2);
    memcpy(canvas_buf + 16*4096+8*640, canvas_cache, 2);
    memcpy(canvas_buf + 16*4096+10*640, canvas_cache, 2);
    //esp_cache_msync((void*)canvas_buf, 1024, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    //esp can do PSRAM by DMA but ESP-IDF can not

    //lv_canvas_draw_img(canvas, 0, 0, &background, &draw_dsc); //20ms
    
    //10ms
    for (int i = 0; i < NUM_TUBES; i++) {
        lv_canvas_draw_img(canvas, tubes[i].x_position, tubes[i].y_position, tubes[i].image, &draw_dsc);
        for(int j = 0; j <tubes[i].y_position; j++){
            memcpy(canvas_buf + (tubes[i].y_position-j)*line + tubes[i].x_position*2, canvas_cache + (tubes[i].y_position-j)*line  + tubes[i].x_position*2, 20);
        }
        
    }

    lv_canvas_draw_img(canvas, plane_inst.x_position, plane_inst.y_position, plane_inst.image, &draw_dsc);

    // makes it slower
    //lv_obj_invalidate(canvas);
    //lv_refr_now(NULL);


    lv_task_handler();  //20ms
    bsp_display_unlock();

}
