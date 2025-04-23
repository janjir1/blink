#include "game_loop/game_loop.h"

#include "common.h"
#include "button/button.h"
#include "audio/audio.h"
#include "display/assets.h"
#include "display/renderer.h"
#include "esp_timer.h"
#include  <inttypes.h>

#define FRAME_TIME 25

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
}

void set_score(char *buffer){

    text_inst.text = buffer; 
}

void game_loop_task() {

    uint32_t score_counter = 0;

    bool engine_stall = false;
    bool explosion_sound_played = false;
    bool sound_on = true;

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
                        if (plane_inst.y_position < 300) plane_inst.y_position++;
                        if (tubes[i].y_position < 300) tubes[i].y_position++;

                        if (!explosion_sound_played) {

                            if (sound_on) xTaskCreate(audio_play_explosion, "explosion_task", 4096, NULL, 2, NULL);
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

        // update score
        if (prev_above_tower && !above_tower) score_counter++;
        prev_above_tower = above_tower;

        if (!tower_hit) explosion_sound_played = false;

        //---- PLANE MOVEMENT --------------------------------------------------------------------------------------------------------------------------------------------

        if (tower_hit) {

            // JANJIŘÍ ZMĚNÍ LETADLO NA VYBUCHLÉ TADY
            change_plane(PLANE);

        } else if (engine_stall) {

            // JANJIŘÍ TOTO LETADLO VYMĚNÍ ZA LETADLO NAKLONĚNÉ NAHORU S KOUŘEM Z MOTORU
            change_plane(PLANE_UP);
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

        if (button_is_pressed(BUTTON_REC, true)) sound_on = !sound_on;

        //---- TUBE MOVEMENT --------------------------------------------------------------------------------------------------------------------------------------------

        if (!tower_hit) update_tubes((score_counter / 10 + 1) * (button_is_pressed(BUTTON_SET, false) * !engine_stall + 1));

        //---- RENDER SCENE ---------------------------------------------------------------------------------------------------------------------------------------------

        char buffer[32];

        // JANEJIŘÍ, tady by bylo lepší ukazovat ikonku na opačné straně displeje když je zvuk vypnutý
        if (sound_on) snprintf(buffer, sizeof(buffer), "%u", score_counter);
        else snprintf(buffer, sizeof(buffer), "(X) %u", score_counter);
        set_score(buffer);

        renderer_render_scene(); 

        uint64_t end = esp_timer_get_time();  
        uint64_t delay;

        if (((end - start) / 1000) > FRAME_TIME - 5) delay = 5;
        else delay = FRAME_TIME - ((end - start) / 1000);

        //printf("delay is %" PRIu64 " ms\n", delay); 

        vTaskDelay(pdMS_TO_TICKS(delay));

        //---------------------------------------------------------------------------------------------------------------------------------------------------------------
    }
}

