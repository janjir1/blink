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
#include "common.h"
#include "blink.h"

void app_main(void) {

    xTaskCreate(blink_task, "led_blink", 2048, NULL, 1, NULL);

    while(1) vTaskDelay(500 / portTICK_PERIOD_MS);
}
