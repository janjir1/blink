#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "bsp/esp-bsp.h"

enum button_t {

    BUTTON_REC = BSP_BUTTON_REC,
    BUTTON_MODE = BSP_BUTTON_MODE,
    BUTTON_PLAY = BSP_BUTTON_PLAY,
    BUTTON_SET = BSP_BUTTON_SET,
    BUTTON_VOLDOWN = BSP_BUTTON_VOLDOWN,
    BUTTON_VOLUP = BSP_BUTTON_VOLUP,
    BUTTON_NUM = BSP_BUTTON_NUM

};

void button_init();

// returns key state. If do_once flag is used, function returns true only first time, key press is registered
bool button_is_pressed(enum button_t key, bool do_once);

// returns key hold time [ms]
uint32_t button_get_hold_time(enum button_t key);

// returns true if key is pressed and was held for specified ammount of time
bool button_is_pressed_for_ms(enum button_t key, uint32_t time_ms, bool do_once);

#endif /* _BUTTON_H_ */