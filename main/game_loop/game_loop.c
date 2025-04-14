#include "game_loop/game_loop.h"

#include "common.h"
#include "button/button.h"
#include "display/assets.h"
#include "display/renderer.h"


void update_tubes(uint8_t speed){
    for (int i = 0; i < NUM_TUBES; i++) {
        if (tubes[i].x_position > SCREEN_WIDTH + 20){
            tubes[i].x_position = -20;
        }
        tubes[i].x_position += speed;
    }
}

void game_loop_task() {

    while (1) {
        update_tubes(10);
        render_scene();       // Draw the current state (pipes, etc.)
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(10));

    }
}

