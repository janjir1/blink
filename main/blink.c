#include "blink.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "common.h"

#define BLINK_GPIO 45

void blink_task() {

    static led_strip_handle_t led_strip;

    ESP_LOGI(TAG, "Jan Jiří Bauer je hňup!");

    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1, // at least one LED on board
    };

    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    led_strip_clear(led_strip);

    while (1) {
        
        led_strip_set_pixel(led_strip, 0, 0, 255, 0);
        led_strip_refresh(led_strip);
        sleep_ms(500);
        
        led_strip_clear(led_strip); led_strip_refresh(led_strip);   
        sleep_ms(500);
    }

    vTaskDelete(NULL);
}