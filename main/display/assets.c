#include "display/assets.h"
#include "display/tubes_image.h"


tube_t tubes[NUM_TUBES] = {
    {-236, 150, -236, 150, &tube_bottom2},
    {-236, -120, -236, -100, &tube_top},
    {-108, 200, -108, 200,&tube_bottom2},
    {-108, -80,  -108, -50,&tube_top},
    {20, 100, 20, 100, &tube_bottom2},
    {20, -170, 20, -150, &tube_top}
};

const lv_img_dsc_t* background_image = &background2;

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
tube_t plane_inst = {160, 50, 160, 50,&plane};

const lv_img_dsc_t* plane_images[PLANE_IMAGE_COUNT] = {
    &plane,
    &plane_up,
    &plane_down,
    &plane_bum,
    &plane_fall
};


text_t text_inst = {10, 2, "Score:"};  
