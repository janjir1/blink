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

#define BLACK_HEIGHT 20


lv_draw_img_dsc_t draw_dsc;
lv_draw_label_dsc_t label_dsc;
lv_draw_rect_dsc_t rect_dsc;

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

    lv_canvas_draw_img(canvas, 0, 0, background_image, &draw_dsc);
    
    memcpy(canvas_cache, canvas_buf, sizeof(canvas_buf));

    lv_draw_label_dsc_init(&label_dsc);               
    label_dsc.color = lv_color_white();                  
    label_dsc.font  = &lv_font_montserrat_14;        

    
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_black();
    rect_dsc.bg_opa = LV_OPA_COVER;  // Fully opaque
}

void renderer_update_background(void){

    lv_canvas_draw_img(canvas, 0, 0, background_image, &draw_dsc);
    memcpy(canvas_cache, canvas_buf, sizeof(canvas_buf));

}


static void draw_text_and_number(lv_coord_t x, lv_coord_t y, const char *text)
{
    // Create rectangle descriptor

    
    // Draw black rectangle at the top
    lv_canvas_draw_rect(canvas, 0, 0, SCREEN_WIDTH, BLACK_HEIGHT, &rect_dsc);
    
    // Draw text (on top of the rectangle)
    lv_canvas_draw_text(canvas,
        x, y,
        SCREEN_WIDTH*2, 
        &label_dsc,
        text);
}


void copy_canvas_section(uint16_t y_start, uint16_t y_end_offset, uint16_t x_start, uint16_t copy_width) {
    
    lv_color_t* dst = canvas_buf + y_start * SCREEN_WIDTH + x_start;
    lv_color_t* src = canvas_cache + y_start * SCREEN_WIDTH + x_start;
    uint16_t y_end = y_start + y_end_offset;

    for(uint16_t j = y_start; j <= y_end; j++) {
        memcpy(dst, src, copy_width);
        dst += SCREEN_WIDTH;
        src += SCREEN_WIDTH;
    }
}


void IRAM_ATTR renderer_render_scene(bool optimize)
{
    
    
    bsp_display_lock(50);   
    //memcpy(canvas_buf, canvas_cache, sizeof(canvas_cache)); //40ms

    //esp can do PSRAM by DMA but ESP-IDF can not

    //lv_canvas_draw_img(canvas, 0, 0, &background, &draw_dsc); //30ms
    ///// plane
    if (optimize)
    {
        //plane
        copy_canvas_section(plane_inst.y_position, PLANE_HEIGH + 10, plane_inst.x_position, PLANE_WIDTH_ANGLE*2);
        

        //// tubes
        for (int i = 0; i < NUM_TUBES; i++) { //for every tube
            if(tubes[i].x_position + TUBE_WIDTH > 0 && tubes[i].x_position - 16 <SCREEN_WIDTH){ //thats on screen
                const uint16_t y_start = (tubes[i].y_position < BLACK_HEIGHT) ? BLACK_HEIGHT : (uint16_t)tubes[i].y_position;
                const uint16_t y_end = (tubes[i].y_position + TUBE_HEIGH > SCREEN_HEIGHT) ? SCREEN_HEIGHT : (uint16_t)tubes[i].y_position+TUBE_HEIGH - y_start;

                if (!(i & 1)) { //full transparency for birds
            
                    copy_canvas_section(y_start, y_end, tubes[i].x_position - 8, 16);
                }
                else{

                    copy_canvas_section(y_start, y_end, tubes[i].x_position-8, TUBE_WIDTH*2+8);
                }             
            }
        }

        lv_canvas_draw_img(canvas, plane_inst.x_position, plane_inst.y_position, plane_inst.image, &draw_dsc);

        for (int i = 0; i < NUM_TUBES; i++) {
            lv_canvas_draw_img(canvas, tubes[i].x_position, tubes[i].y_position, tubes[i].image, &draw_dsc);
        }

        draw_text_and_number(text_inst.x, text_inst.y, text_inst.text);
    }

    else{
        lv_canvas_draw_img(canvas, 0, 0, background_image, &draw_dsc);

        //tubes
        for (int i = 0; i < NUM_TUBES; i++) {
            lv_canvas_draw_img(canvas, tubes[i].x_position, tubes[i].y_position, tubes[i].image, &draw_dsc);
        }

        // plane
        lv_canvas_draw_img(canvas, plane_inst.x_position, plane_inst.y_position, plane_inst.image, &draw_dsc);

        // text
        draw_text_and_number(text_inst.x, text_inst.y, text_inst.text);

    }

    lv_task_handler();  //20ms
    bsp_display_unlock();

}
