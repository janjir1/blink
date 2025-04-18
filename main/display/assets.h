#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "display/tubes_image.h"

#define NUM_TUBES 6
#define ORIGINAL_IMAGE tube_bottom

#define TUBE_WIDTH 40 //px
#define TUBE_HEIGH 200
#define PLANE_WIDTH 50 //only for straight plane
#define PLANE_HEIGH 50

#define PLANE_WIDTH_ANGLE 70 //only for straight plane
#define PLANE_HEIGH_ANGLE 70


typedef struct {
    int16_t x_position;
    int16_t y_position;
    int16_t x_position_next;
    int16_t y_position_next;
    const lv_img_dsc_t *image;
} tube_t;

typedef enum {
    PLANE,
    PLANE_UP,
    PLANE_DOWN,
    PLANE_IMAGE_COUNT 
} planeImageType;

extern const lv_img_dsc_t* plane_images[];

extern tube_t tubes[NUM_TUBES];

extern tube_t plane_inst;

typedef struct {
    lv_coord_t   x; 
    lv_coord_t   y;  
    const char  *text;    /* NUL‑terminated string pointer */
} text_t;  

extern text_t text_inst;