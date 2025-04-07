#include "button.h"

#include "common.h"
#include "driver/gpio.h"

static button_handle_t audio_button[BSP_BUTTON_NUM] = {};
QueueHandle_t audio_button_q = NULL;

static uint32_t key_state = 0;                          // bit field containing debounced states of push buttons
static uint32_t key_press_time_ms[BSP_BUTTON_NUM] = {0};     // time of key press [ms]
static uint32_t key_press_acknowledged = 0;             // bit corresponding to a key is set after keypad_is_pressed() is called with do_once flag set, reset after button release


static void btn_handler(void *arg, void *arg2);
static void btn_release_handler(void *arg, void *arg2);

void button_init() {

    /* Init audio buttons */
    for (int i = 0; i < BSP_BUTTON_NUM; i++) {
        audio_button[i] = iot_button_create(&bsp_button_config[i]);
        assert(audio_button[i] != NULL);
        ESP_ERROR_CHECK(iot_button_register_cb(audio_button[i], BUTTON_PRESS_DOWN, btn_handler, NULL));
        ESP_ERROR_CHECK(iot_button_register_cb(audio_button[i], BUTTON_PRESS_UP, btn_release_handler, NULL));
    }
}

// returns key state. If do_once flag is used, function returns true only first time, key press is registered
bool button_is_pressed(enum button_t key, bool do_once) {

    return false;
}

// returns key hold time [ms]
uint32_t button_get_hold_time(enum button_t key) {

    return 0;
}

// returns true if key is pressed and was held for specified ammount of time
bool button_is_pressed_for_ms(enum button_t key, uint32_t time_ms, bool do_once) {

    return false;
}

static void btn_handler(void *arg, void *arg2)
{
    for (uint8_t i = 0; i < BSP_BUTTON_NUM; i++) {
        if ((button_handle_t)arg == audio_button[i]) {
            xQueueSend(audio_button_q, &i, 0);
            break;
        }
    }
}

static void btn_release_handler(void *arg, void *arg2)
{
    esp_lo
    /*for (uint8_t i = 0; i < BSP_BUTTON_NUM; i++) {
        if ((button_handle_t)arg == audio_button[i]) {
            xQueueSend(audio_button_q, &i, 0);
            break;
        }
    }*/
}