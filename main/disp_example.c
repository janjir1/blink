/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "disp_example.h"
#include "lvgl.h"
#include "bsp/esp-bsp.h"

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

// Define screen and rectangle dimensions.
// Update these values to match your actual display resolution.
#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   240
#define RECT_WIDTH      50
#define RECT_HEIGHT     30

/*
// Task to move the yellow rectangle from left to right
void move_yellow_rect_task(void *pvParameters)
{
    // Create a yellow rectangle on the active screen.
    lv_obj_t *yellow_rect = lv_obj_create(lv_scr_act());
    lv_obj_set_size(yellow_rect, RECT_WIDTH, RECT_HEIGHT);

    // Set the background color to yellow.
    // (For LVGL v7, you might use lv_obj_set_style_local_bg_color().
    // For LVGL v8, you would use lv_obj_set_style_bg_color().)
    lv_obj_set_style_bg_color(yellow_rect, LV_COLOR_YELLOW, 0);
    lv_obj_set_style_bg_opa(yellow_rect, LV_OPA_COVER, 0);

    // Start at the left edge and center vertically.
    int x = 0;
    int y = (SCREEN_HEIGHT - RECT_HEIGHT) / 2;
    lv_obj_set_pos(yellow_rect, x, y);

    // Move the rectangle until its right edge reaches the screen boundary.
    while (x < (SCREEN_WIDTH - RECT_WIDTH)) {
        // Lock display to safely update the LVGL object.
        bsp_display_lock(0);
        lv_obj_set_pos(yellow_rect, x, y);
        bsp_display_unlock(0);

        // Advance by 5 pixels to the right.
        x += 5;

        // Delay to control animation speed (50ms delay).
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // Optionally, delete the rectangle once the animation completes.
    bsp_display_lock(0);
    lv_obj_del(yellow_rect);
    bsp_display_unlock(0);

    // Delete this task once finished.
    vTaskDelete(NULL);
}
*/