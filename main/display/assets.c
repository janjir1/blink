#include "display/assets.h"
#include "display/tubes_image.h"

tube_t tubes[NUM_TUBES] = {
    {50, 150, &tube_bottom},
    {50, -100, &tube_top},
    {180, 200, &tube_bottom},
    {180, -50,  &tube_top},
    {320, 100, &tube_bottom},
    {320, -150,  &tube_top}
};