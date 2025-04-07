#ifndef TUBE_H
#define TUBE_H

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Screen dimensions.
#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   240

// Frame rate settings.
#define FRAMERATE       20
#define FRAME_TIME_MS   (1000 / FRAMERATE)

// Public structure for a tube object.
// You may choose to keep this structure private if only used internally.
typedef struct {
    bool top;             // Flag to determine which image to use.
    uint8_t y_position;   // Vertical position on the screen.
    uint8_t id;           // Unique identifier.
    uint8_t x_position;   // Horizontal position on the screen.
} tube_t;

/**
 * @brief Set the global speed for tube movement.
 *
 * @param new_speed The new speed value.
 */
void set_speed(uint8_t new_speed);

/**
 * @brief Dynamically spawns a tube that moves an image across the screen.
 *
 * @param top       If true, uses the top image; otherwise uses the bottom image.
 * @param y_position The vertical position at which the image is displayed.
 */
void spawn_tube(bool top, uint8_t y_position);

#endif // TUBE_H
