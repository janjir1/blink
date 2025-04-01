#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#define TAG "hovno"

static inline void sleep_ms(uint32_t duration_ms) {

    vTaskDelay(duration_ms / portTICK_PERIOD_MS);
    taskYIELD(); // Yield to avoid watchdog resets
}

#endif /* _COMMON_H_ */