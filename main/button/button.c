#include "button/button.h"

#include "common.h"
#include "driver/gpio.h"
#include "esp_timer.h"

//---- INTERNAL DATA ---------------------------------------------------------------------------------------------------------------------------------------------

static button_handle_t button[BSP_BUTTON_NUM] = {};

static uint32_t key_state = 0;                              // bit field containing debounced states of push buttons
static uint64_t key_press_time_us[BSP_BUTTON_NUM] = {0};    // time of key press [us]
static uint32_t key_press_acknowledged = 0;                 // bit corresponding to a key is set after keypad_is_pressed() is called with do_once flag set, reset after button release

//---- INTERNAL FUNCTIONS ----------------------------------------------------------------------------------------------------------------------------------------

static inline bool get_key_state(uint32_t key) {return (key_state & (1 << key));}
static inline bool get_key_acknowledged(uint32_t key) {return (key_press_acknowledged & (1 << key));}

static void btn_handler(void *arg, void *arg2);
static void btn_release_handler(void *arg, void *arg2);

//---- FUNCTIONS -------------------------------------------------------------------------------------------------------------------------------------------------

void button_init() {

    /* Init audio buttons */
    for (int i = 0; i < BSP_BUTTON_NUM; i++) {
        button[i] = iot_button_create(&bsp_button_config[i]);
        iot_button_register_cb(button[i], BUTTON_PRESS_DOWN, btn_handler, NULL);
        iot_button_register_cb(button[i], BUTTON_PRESS_UP, btn_release_handler, NULL);
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

// returns key state. If do_once flag is used, function returns true only first time, key press is registered
bool button_is_pressed(enum button_t key, bool do_once) {

    if (get_key_state(key)) {

        if (!(do_once && get_key_acknowledged(key))) {

            if (do_once) key_press_acknowledged |= (1 << key);
            return true;

        } else return false;

    } else return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

// returns key hold time [us]
uint64_t button_get_hold_time(enum button_t key) {

    if (!button_is_pressed(key, false)) return 0;
    else return (esp_timer_get_time() - key_press_time_us[key]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

// returns true if key is pressed and was held for specified ammount of time
bool button_is_pressed_for_us(enum button_t key, uint32_t time_us, bool do_once) {

    if ((button_get_hold_time(key) >= time_us) && button_is_pressed(key, do_once)) return true;
    else return false;
}

//---- INTERNAL FUNCTIONS ----------------------------------------------------------------------------------------------------------------------------------------

static void btn_handler(void *arg, void *arg2) {

    for (uint8_t i = 0; i < BSP_BUTTON_NUM; i++) {

        if ((button_handle_t)arg == button[i]) {
            
            key_state |= (1 << i);
            key_press_acknowledged &= ~(1 << i);
            key_press_time_us[i] = esp_timer_get_time();
            break;
        }
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

static void btn_release_handler(void *arg, void *arg2) {

    for (uint8_t i = 0; i < BSP_BUTTON_NUM; i++) {

        if ((button_handle_t)arg == button[i]) {
            
            key_state &= ~(1 << i);
            key_press_acknowledged &= ~(1 << i);
            break;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
