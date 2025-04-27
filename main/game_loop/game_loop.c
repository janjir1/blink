#include "game_loop/game_loop.h"

#include "common.h"
#include "button/button.h"
#include "audio/audio.h"
#include "display/assets.h"
#include "display/renderer.h"
#include "esp_timer.h"
#include  <inttypes.h>

#define FRAME_TIME 33
static const char *game_loop_TAG = "game_loop";

void update_tubes(int8_t speed) {

    for (int i = 0; i < NUM_TUBES; i += 2) {

        if (tubes[i].x_position > SCREEN_WIDTH + 20) {

            int32_t random_y_shift = (rand() & 0x3f) - 32;

            if ((tubes[i].y_position > 200) && (random_y_shift > 0)) random_y_shift = -random_y_shift;
            if ((tubes[i].y_position < 140) && (random_y_shift < 0)) random_y_shift = -random_y_shift;
            
            tubes[i].x_position = -40;
            tubes[i].y_position += random_y_shift;

            tubes[i+1].x_position = -40;
            tubes[i+1].y_position += random_y_shift;
        }

        tubes[i].x_position += speed;
        tubes[i+1].x_position += speed;
    }
}

void change_plane(uint8_t plane_type){

    if (plane_type == PLANE) plane_inst.image = plane_images[PLANE];
    else if (plane_type == PLANE_DOWN) plane_inst.image = plane_images[PLANE_DOWN];
    else if (plane_type == PLANE_UP) plane_inst.image = plane_images[PLANE_UP];
    else if (plane_type == PLANE_BUM) plane_inst.image = plane_images[PLANE_BUM];
    else if (plane_type == PLANE_FALL) plane_inst.image = plane_images[PLANE_FALL];
}

void set_score(char *buffer){

    text_inst.text = buffer; 
}

void game_loop_task() {

    uint32_t score_counter = 0;

    bool engine_stall = false;
    bool explosion_sound_played = false;
    bool optimize_renderer = true;
    bool hard_mode = false;
    bool the_end = false;

    uint64_t delay = 0;
    uint64_t render_time = 0;

    uint8_t volume = DEFAULT_VOLUME;

    // start background sound
    xTaskCreate(audio_loop_task, "audio_loop_task", 4096, (void *)FILE_NYAN, 10, NULL);

    while (1) {

        uint64_t start = esp_timer_get_time();

        //---- CHECK COLLISION -------------------------------------------------------------------------------------------------------------------------------------------

        bool tower_hit = false;
        bool bird_hit = false;
        

        bool above_tower = false;
        static bool prev_above_tower = false;

        for (int i = 0; i < NUM_TUBES; i++) {

            int32_t tower_dist_x = plane_inst.x_position - tubes[i].x_position;

            if ((tower_dist_x > 0) && (tower_dist_x < TUBE_WIDTH)) {

                // check tower collision for tubes 0,2,4
                if (!(i & 1)) {

                    if (plane_inst.y_position + 25 > tubes[i].y_position) {

                        tower_hit = true;
                        engine_stall = false;
    
                        // collapse tower
                        if (plane_inst.y_position < SCREEN_HEIGHT || tubes[i].y_position < SCREEN_HEIGHT)
                        {
                            optimize_renderer = false;
                            if (plane_inst.y_position < SCREEN_HEIGHT) plane_inst.y_position+=2;
                            if (tubes[i].y_position < SCREEN_HEIGHT) tubes[i].y_position+=2;
                        }
                        else {                            
                            the_end = true;
                            ESP_LOGI(game_loop_TAG, "Game over! Score: %u", score_counter);
                            break;
                        }

                        if (!explosion_sound_played) {

                            if (volume > 0)
                            {
                                xTaskCreate(audio_set_volume, "audio_set_volume", 2048, (void *)(volume + 15), 4, NULL);
                                vTaskDelay(pdMS_TO_TICKS(10));
                                xTaskCreate(audio_task, "audio_task", 4096, (void *)FILE_EXPLOSION, 5, NULL);
                            }
                            explosion_sound_played = true;
                        }

                        break;
                    }

                // check bird collision for tubes (1,3,5)
                } else {

                    if (plane_inst.y_position < tubes[i].y_position + TUBE_HEIGH - 15) {

                        bird_hit = true;
                        engine_stall = true;
                        break;
                    }
                }

                above_tower = true;
            }
        }
        
        // end the game
        if (the_end) break;

        // update score
        if (prev_above_tower && !above_tower) score_counter++;
        prev_above_tower = above_tower;

        if (!tower_hit) explosion_sound_played = false;

        //---- PLANE MOVEMENT --------------------------------------------------------------------------------------------------------------------------------------------

        if (tower_hit) {

            // JANJIŘÍ ZMĚNÍ LETADLO NA VYBUCHLÉ TADY
            change_plane(PLANE_BUM);

        } else if (engine_stall) {

            // JANJIŘÍ TOTO LETADLO VYMĚNÍ ZA LETADLO NAKLONĚNÉ NAHORU S KOUŘEM Z MOTORU
            change_plane(PLANE_FALL);
            plane_inst.y_position += 1;

        } else {

            if (!button_is_pressed(BUTTON_VOLDOWN, false) && !button_is_pressed(BUTTON_VOLUP, false)) change_plane(PLANE);

            else if (button_is_pressed(BUTTON_VOLDOWN, false)) {
                
                change_plane(PLANE_UP);
                plane_inst.y_position -= (score_counter / 10 + 1) * 2;

            } else if (button_is_pressed(BUTTON_VOLUP, false)) {
                
                change_plane(PLANE_DOWN);
                plane_inst.y_position += (score_counter / 10 + 1) * 2;
            }
        }

        
        if (button_is_pressed(BUTTON_REC, true) && volume < 100)
        {
            volume += 5;
            xTaskCreate(audio_set_volume, "audio_set_volume", 2048, (void *)volume, 1, NULL);
        }
        else if (button_is_pressed(BUTTON_MODE, true) && volume > 0)
        {
            volume -= 5;
            xTaskCreate(audio_set_volume, "audio_set_volume", 2048, (void *)volume, 1, NULL);
        }


        //---- CHANGE GRAPHICS ------------------------------------------------------------------------------------------------------------------------------------------
        if (button_is_pressed(BUTTON_PLAY, true)){
            if (hard_mode){
                for(uint8_t i=0; i<NUM_TUBES; i+=2) tubes[i].image = &tube_bottom2;
                background_image = &background2;     
            }
            else{
                for(uint8_t i=0; i<NUM_TUBES; i+=2) tubes[i].image = &tube_bottom;
                background_image = &background; 
            }
            hard_mode = !hard_mode;
            renderer_update_background();
                  
        }
        

        //---- TUBE MOVEMENT --------------------------------------------------------------------------------------------------------------------------------------------

        if (!tower_hit) update_tubes((score_counter / 10 + 1) * (button_is_pressed(BUTTON_SET, false) * !engine_stall + 1));

        //---- RENDER SCENE ---------------------------------------------------------------------------------------------------------------------------------------------

        char buffer[128];

        uint8_t test = 69;
        
        snprintf(buffer, sizeof(buffer), "Score: %-010" PRId32 " RT: %-3" PRId64 " ms      Volume: %-3" PRId8 " %%", score_counter, render_time, volume);
        //snprintf(buffer, sizeof(buffer), "Volume:%" PRId8 " %%", volume);
        //ESP_LOGI(game_loop_TAG, "%d", volume);

        set_score(buffer);

        renderer_render_scene(optimize_renderer); 

        uint64_t end = esp_timer_get_time();  

        render_time = ((end - start) / 1000);

        //if (render_time > FRAME_TIME - 5) delay = render_time + FRAME_TIME;
        if (render_time > FRAME_TIME - 5) delay = 10;
        else delay = FRAME_TIME - render_time;

        //printf("delay is %" PRIu64 " ms\n", delay); 

        vTaskDelay(pdMS_TO_TICKS(delay));

        //---------------------------------------------------------------------------------------------------------------------------------------------------------------
    }
    vTaskDelete(NULL);
}

