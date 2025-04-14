#include "display/tubes_image.h"
#include "lvgl.h"
#include "bsp/esp-bsp.h"
//#include "display/my_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "display/tubes.h"


static int next_id = 0;
static uint8_t speed = 5;


// A function to update the global speed.
void set_speed(uint8_t new_speed) {
    speed = new_speed;
}

// This task moves the image across the screen.
void move_image_task(void *pvParameters) {
    tube_t *obj = (tube_t *)pvParameters;

    // Create an LVGL image object on the active screen.
    lv_obj_t *img_obj = lv_img_create(lv_scr_act());
    
    // Choose the correct image based on the object's "top" flag.
    const lv_img_dsc_t *selected_image = NULL;
    if (obj->top) {
        selected_image = &tube_top;
        obj->y_position += 100;

    } else {
        selected_image = &tube_bottom;
    }
    
    // Set the image source.
    lv_img_set_src(img_obj, selected_image);
    
    // Retrieve the image dimensions.
    uint8_t img_w = lv_obj_get_width(img_obj);
    
    // Set the initial position
    bsp_display_lock(0);
    lv_obj_set_pos(img_obj, obj->x_position, obj->y_position);
    bsp_display_unlock();
    
    // Move the image until its right edge reaches the screen boundary.
    while (obj->x_position < (SCREEN_WIDTH - img_w)) {
        bsp_display_lock(0);
        lv_obj_set_pos(img_obj, obj->x_position, obj->y_position);
        bsp_display_unlock();

        // Advance the image position by the current speed.
        obj->x_position += speed;
        
        // Delay to control the animation speed.
        vTaskDelay(pdMS_TO_TICKS(FRAME_TIME_MS));
    }
    
    // Clean up: delete the image object.
    bsp_display_lock(0);
    lv_obj_del(img_obj);
    bsp_display_unlock();
    
    // Free the allocated tube object.
    free(obj);
    
    // Delete this task.
    vTaskDelete(NULL);
}

// Function to spawn a new tube dynamically.
void spawn_tube(bool top, uint8_t y_position) {
    // Allocate memory for a new tube object.
    tube_t *obj = malloc(sizeof(tube_t));
    if (!obj) {
        printf("Memory allocation error!\n");
        return;
    }

    // Initialize the tube's fields.
    obj->top = top;
    obj->y_position = y_position;
    obj->x_position = 0;
    obj->id = next_id++;

    // Create a unique task name for debugging.
    char taskName[16];
    snprintf(taskName, sizeof(taskName), "Task%d", obj->id);

    // Create a FreeRTOS task to move the image.
    BaseType_t result = xTaskCreate(
        move_image_task,  // Task function.
        taskName,         // Task name.
        2048,             // Stack size in words (for ESP32, 2048 words = 8192 bytes)
        obj,              // Task parameter: pointer to the tube object.
        5,                // Task priority.
        NULL              // Task handle (unused).
    );
    
    if (result != pdPASS) {
        printf("Task creation failed for tube id %d\n", obj->id);
        free(obj);
    }
}
