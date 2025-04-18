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

#define line 320


lv_draw_img_dsc_t draw_dsc;
lv_draw_label_dsc_t label_dsc;
static lv_obj_t *canvas = NULL;



EXT_RAM_BSS_ATTR static lv_color_t canvas_buf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(SCREEN_WIDTH, SCREEN_HEIGHT)];
//EXT_RAM_BSS_ATTR lv_color_t canvas_buf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(SCREEN_WIDTH, SCREEN_HEIGHT)];
EXT_RAM_BSS_ATTR static lv_color_t canvas_cache[LV_CANVAS_BUF_SIZE_TRUE_COLOR(SCREEN_WIDTH, SCREEN_HEIGHT)];
//static lv_color_t* canvas_buf;
//static lv_color_t* canvas_cache;

volatile bool make_background_flag = false;



void renderer_create_canvas(void)
{
    // Create a canvas object on the active screen
    //canvas_buf = heap_caps_aligned_alloc(16, LV_CANVAS_BUF_SIZE_TRUE_COLOR(SCREEN_WIDTH, SCREEN_HEIGHT), MALLOC_CAP_SPIRAM);
    //canvas_cache = heap_caps_aligned_alloc(16, LV_CANVAS_BUF_SIZE_TRUE_COLOR(SCREEN_WIDTH, SCREEN_HEIGHT), MALLOC_CAP_SPIRAM);
    
    canvas = lv_canvas_create(lv_scr_act());

    lv_canvas_set_buffer(canvas, canvas_buf, SCREEN_WIDTH, SCREEN_HEIGHT, LV_IMG_CF_TRUE_COLOR);

    lv_draw_img_dsc_init(&draw_dsc);

    lv_canvas_draw_img(canvas, 0, 0, &background, &draw_dsc);
    memcpy(canvas_cache, canvas_buf, sizeof(canvas_buf));

    lv_draw_label_dsc_init(&label_dsc);               /* Default styling */ //:contentReference[oaicite:0]{index=0}
    label_dsc.color = lv_color_white();                /* White text */     
    label_dsc.font  = &lv_font_montserrat_14;          /* 14 px Montserrat */ 
}

static void draw_text_and_number(lv_coord_t x, lv_coord_t y, const char *text)
{
    /* 1) Prepare a small buffer for the number string */
    //char num_buf[16];
    //lv_snprintf(num_buf, sizeof(num_buf), "%ld", (long)number);

    /* 3) Draw the static text */
    lv_canvas_draw_text(canvas,
    x, y,
    200, 
    &label_dsc,
    text);      


}


void IRAM_ATTR renderer_render_scene(void)
{
    
    
    bsp_display_lock(50);   
    //memcpy(canvas_buf, canvas_cache, sizeof(canvas_cache)); //40ms

    //esp can do PSRAM by DMA but ESP-IDF can not

    //lv_canvas_draw_img(canvas, 0, 0, &background, &draw_dsc); //30ms
    ///// plane
    uint16_t y_start = plane_inst.y_position;
    uint16_t y_end = y_start + PLANE_HEIGH + 10;

    uint16_t x_start = plane_inst.x_position;
    uint16_t copy_width = PLANE_WIDTH*2;
    uint16_t line_bytes = SCREEN_WIDTH;

    lv_color_t* dst = canvas_buf + y_start * line_bytes + x_start;
    lv_color_t* src = canvas_cache + y_start * line_bytes + x_start;

    for(uint16_t j = y_start; j <= y_end; j++){
        memcpy(dst, src, copy_width);
        dst += line_bytes;
        src += line_bytes;
    }
    ///// text
    y_start = text_inst.y;
    y_end = y_start + 15;

    x_start = text_inst.x + 45; //adjust this if text is overwriting itself
    copy_width = 64;
    line_bytes = SCREEN_WIDTH;

    dst = canvas_buf + y_start * line_bytes + x_start;
    src = canvas_cache + y_start * line_bytes + x_start;

    for(uint16_t j = y_start; j <= y_end; j++){
        memcpy(dst, src, copy_width);
        dst += line_bytes;
        src += line_bytes;
    }

    lv_canvas_draw_img(canvas, plane_inst.x_position, plane_inst.y_position, plane_inst.image, &draw_dsc);
    //// tubes
    //15ms
    for (int i = 0; i < NUM_TUBES; i++) {
        if(tubes[i].x_position + TUBE_WIDTH > 0 && tubes[i].x_position - 16 <SCREEN_WIDTH){
            if(tubes[i].x_position > 0){
                const uint16_t y_start = (tubes[i].y_position < 0) ? 0 : (uint16_t)tubes[i].y_position;
                const uint16_t y_end = (tubes[i].y_position + TUBE_HEIGH > SCREEN_WIDTH) ? SCREEN_WIDTH : (uint16_t)tubes[i].y_position+TUBE_HEIGH;
        
                const uint8_t copy_width = 16;
                const uint16_t x_start = tubes[i].x_position - copy_width/2;        
                const uint16_t line_bytes = SCREEN_WIDTH;
        
                lv_color_t* dst = canvas_buf + y_start * line_bytes + x_start;
                lv_color_t* src = canvas_cache + y_start * line_bytes + x_start;
        
                

                for(uint16_t j = y_start; j <= y_end; j++){
                    memcpy(dst, src, copy_width);
                    dst += line_bytes;
                    src += line_bytes;
                }
            }

            lv_canvas_draw_img(canvas, tubes[i].x_position, tubes[i].y_position, tubes[i].image, &draw_dsc);
        }
    }


    draw_text_and_number(text_inst.x, text_inst.y, text_inst.text);


    lv_task_handler();  //20ms
    bsp_display_unlock();

}
