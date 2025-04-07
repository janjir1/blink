/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "display/my_disp.h"
#include "lvgl.h"
#include "bsp/esp-bsp.h"

#include "display/my_image.h"

static lv_obj_t *recording_checkbox = NULL;
static lv_obj_t *playing_checkbox = NULL;
static lv_obj_t *volume_arc = NULL;

void disp_init(void)
{
    bsp_display_lock(0);

    /*Create a window*/
    lv_obj_t *win = lv_win_create(lv_scr_act(), 40);
    assert(win);
    lv_win_add_title(win, "ESP32-S2-Kaluga-Kit:\nBoard Support Package example");

    /* Volume arc */
    volume_arc = lv_arc_create(lv_scr_act());
    assert(volume_arc);
    lv_arc_set_end_angle(volume_arc, 200);
    lv_obj_set_size(volume_arc, 130, 130);
    lv_obj_center(volume_arc);
    lv_obj_t *volume_label = lv_label_create(volume_arc);
    assert(volume_label);
    lv_label_set_text_static(volume_label, "Volume");
    lv_obj_set_align(volume_label, LV_ALIGN_BOTTOM_MID);

    /* Checkboxes */
    recording_checkbox = lv_checkbox_create(win);
    assert(recording_checkbox);
    lv_checkbox_set_text_static(recording_checkbox, "Recording");
    playing_checkbox = lv_checkbox_create(win);
    assert(playing_checkbox);
    lv_checkbox_set_text_static(playing_checkbox, "Playing");

    bsp_display_unlock();
}

void disp_set_volume(int volume)
{
    assert(volume_arc);
    bsp_display_lock(0);
    lv_arc_set_value(volume_arc, volume);
    bsp_display_unlock();
}

void disp_set_playing(bool set)
{
    assert(playing_checkbox);
    bsp_display_lock(0);
    if (set) {
        lv_obj_add_state(playing_checkbox, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(playing_checkbox, LV_STATE_CHECKED);
    }
    bsp_display_unlock();
}

void disp_set_recording(bool set)
{
    assert(recording_checkbox);
    bsp_display_lock(0);
    if (set) {
        lv_obj_add_state(recording_checkbox, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(recording_checkbox, LV_STATE_CHECKED);
    }
    bsp_display_unlock();
}

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   240
#define RECT_WIDTH      50
#define RECT_HEIGHT     30

/*
void move_image_task(void *pvParameters)
{
    // Create an image object on the active screen.
    lv_obj_t *img_obj = lv_img_create(lv_scr_act());
    
    // Set the image source to your image descriptor.
    lv_img_set_src(img_obj, &my_image);
    
    // Retrieve the image dimensions.
    uint16_t img_w = lv_obj_get_width(img_obj);
    uint16_t img_h = lv_obj_get_height(img_obj);
    
    // Position the image at the left edge and center it vertically.
    int x = 0;
    int y = (SCREEN_HEIGHT - img_h) / 2;
    lv_obj_set_pos(img_obj, x, y);
    
    // Move the image until its right edge reaches the screen boundary.
    while (x < (SCREEN_WIDTH - img_w)) {
        // Lock display for thread-safe LVGL updates.
        bsp_display_lock(0);
        lv_obj_set_pos(img_obj, x, y);
        bsp_display_unlock();
        
        // Advance the image position.
        x += 5;
        
        // Delay to control animation speed (50ms delay).
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    
    // Optionally, delete the image object once the animation completes.
    bsp_display_lock(0);
    lv_obj_del(img_obj);
    bsp_display_unlock();
    
    // Delete this task.
    vTaskDelete(NULL);
}
    */

void move_yellow_rect_task(void *pvParameters)
{
    // Create a yellow rectangle on the active screen.
    lv_obj_t *yellow_rect = lv_obj_create(lv_scr_act());
    lv_obj_set_size(yellow_rect, RECT_WIDTH, RECT_HEIGHT);

    // Set the background color to yellow using LV_COLOR_MAKE and specify LV_PART_MAIN and LV_STATE_DEFAULT.
    lv_obj_set_style_bg_color(yellow_rect, lv_color_hex(0xFFFF00), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(yellow_rect, LV_OPA_COVER, LV_PART_MAIN);
    // Start at the left edge and center vertically.
    int x = 0;
    int y = (SCREEN_HEIGHT - RECT_HEIGHT) / 2;
    lv_obj_set_pos(yellow_rect, x, y);

    // Move the rectangle until its right edge reaches the screen boundary.
    while (x < (SCREEN_WIDTH - RECT_WIDTH)) {
        // Lock display to safely update the LVGL object.
        bsp_display_lock(0);
        lv_obj_set_pos(yellow_rect, x, y);
        bsp_display_unlock();

        // Advance by 5 pixels to the right.
        x += 5;

        // Delay to control animation speed (50ms delay).
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // Optionally, delete the rectangle once the animation completes.
    bsp_display_lock(0);
    lv_obj_del(yellow_rect);
    bsp_display_unlock();

    // Delete this task once finished.
    vTaskDelete(NULL);
}

