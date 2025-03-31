/* 
    projekt
    MPC-DIS

    naprogramovali borci
    Bc. Martin Kopka
    Bc. Jan Jiří Bauer
    Bc. Viktor Cejnek
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

static const char *TAG = "hovno";

#define BLINK_GPIO 45
#define BTN_GPIO 6

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

    gpio_reset_pin(BTN_GPIO);
    gpio_set_direction(BTN_GPIO, GPIO_MODE_INPUT);

    while (1) {
        
        led_strip_set_pixel(led_strip, 0, 0, 255, 0);
        led_strip_refresh(led_strip);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        
        led_strip_clear(led_strip); led_strip_refresh(led_strip);   
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

void app_main(void) {

    xTaskCreate(blink_task, "led_blink", 512, NULL, 1, NULL);

    while (1) {

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}
