#include "blink/blink.h"

#include "common.h"
#include "driver/gpio.h"
#include "led_strip.h"

#include "button/button.h"

#define BLINK_GPIO 45

//---- FUNCTIONS -------------------------------------------------------------------------------------------------------------------------------------------------

void blink_task() {

    static led_strip_handle_t led_strip;

    ESP_LOGI(TAG, "Jan Jiří Bauer je borec!");

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
        vTaskDelay(pdMS_TO_TICKS(500));
        
        led_strip_clear(led_strip);
        led_strip_refresh(led_strip);   
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    vTaskDelete(NULL);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
