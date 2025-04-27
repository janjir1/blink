/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_spiffs.h"

#include "bsp/esp-bsp.h"
#include "es8311.h"
#include "led_strip.h"
#include "lvgl.h"
//#include "display/my_disp.h"

#include "display/renderer.h"
#include "game_loop/game_loop.h"

#include "lvgl.h"
#include "bsp/esp-bsp.h"

#include "common.h"
#include "blink/blink.h"
#include "button/button.h"

#include "audio/audio.h"    //VC

//custom macros
#define delay_ms(my_delay) vTaskDelay(pdMS_TO_TICKS(my_delay))  //VC

//end of custom macros

//custom defines
#define CONFIG_I2S_ENABLE_DEBUG_LOG 1 //VC

/* Globals */

extern QueueHandle_t audio_button_q;    //delete?


static esp_err_t spiffs_init(void)      //file system init
{
    esp_err_t ret = ESP_OK;
    ESP_LOGI(TAG, "Initializing SPIFFS");

    const esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };

    /*!< Use settings defined above to initialize and mount SPIFFS filesystem. */
    /*!< Note: esp_vfs_spiffs_register is an all-in-one convenience function. */
    ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }

        return ret;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    return ret;
}

void app_main(void)
{

    srand((uint32_t)esp_timer_get_time());

    button_init();
    xTaskCreate(blink_task, "led_blink", 2048, NULL, 1, NULL);

    // Init board peripherals
    // audio:
    bsp_i2c_init(); // Used by ES8311 driver
    audio_init();   // VC
    spiffs_init();

    /* Needed from random RGB LED color generation */
    //time_t t;
    //srand((unsigned) time(&t));

    /* Create FreeRTOS tasks and queues */
    // audio_button_q = xQueueCreate(10, sizeof(uint8_t));
    // assert (audio_button_q != NULL);



    bsp_display_start(); // Start LVGL and LCD driver
    //disp_init();         // Create LVGL screen and widgets
    //disp_set_volume(DEFAULT_VOLUME);
    renderer_create_canvas();
    
    xTaskCreate(game_loop_task, "game_loop_task", 4096, NULL, 2, NULL);
}

