#include "game_loop/game_loop.h"

#include "common.h"
#include "button/button.h"
#include "display/assets.h"
#include "display/renderer.h"
#include "esp_timer.h"
#include  <inttypes.h>

#define FRAME_TIME 50

void update_tubes(int8_t speed){
    for (int i = 0; i < NUM_TUBES; i++) {
        if (tubes[i].x_position > SCREEN_WIDTH + 20){
            tubes[i].x_position = -40;
        }
        tubes[i].x_position += speed;
    }
}

void change_plane(uint8_t plane_type){
    if (plane_type == PLANE) plane_inst.image = plane_images[PLANE];
    else if (plane_type == PLANE_DOWN) plane_inst.image = plane_images[PLANE_DOWN];
    else if (plane_type == PLANE_UP) plane_inst.image = plane_images[PLANE_UP];
}

void game_loop_task() {
    uint8_t counter = 0;
    uint64_t start = 0;
    uint64_t end = 0;
    uint64_t delay = 0;
    while (1) {
        start = esp_timer_get_time();
        change_plane(counter/100);
        counter++;
        update_tubes(5);
        render_scene(); 
        end = esp_timer_get_time();  


        if(((end - start) / 1000.0) > 60){
            delay = 10;
        }
        else{
             delay = FRAME_TIME - ((end - start) / 1000.0);
        }

        printf("delay is %" PRIu64 " ms\n", delay); 



        vTaskDelay(pdMS_TO_TICKS(delay));

    }
}

