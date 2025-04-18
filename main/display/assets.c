#include "display/assets.h"
#include "display/tubes_image.h"


tube_t tubes[NUM_TUBES] = {
    {50, 150, 50, 150, &tube_bottom},
    {50, -100, 50, -100, &tube_top},
    {180, 200, 180, 200,&tube_bottom},
    {180, -50,  180, -50,&tube_top},
    {320, 100, 320, 100, &tube_bottom},
    {320, -150, 320, -150, &tube_top}
};

/*
tube_t tubes[NUM_TUBES] = {
    {320, 100, 50, 150, &tube_bottom},
    {320, -150, 50, -100, &tube_top},
    {320, 100, 50, 150, &tube_bottom},
    {320, -150, 50, -100, &tube_top},
    {320, 100, 50, 150, &tube_bottom},
    {320, -150, 50, -100, &tube_top},
};
*/
tube_t plane_inst = {160, 160, 160, 160,&plane};

const lv_img_dsc_t* plane_images[PLANE_IMAGE_COUNT] = {
    &plane,
    &plane_up,
    &plane_down
};

text_t text_inst = {10, 10, "Score:"};  
