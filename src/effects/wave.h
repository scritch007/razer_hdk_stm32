//
// Created by benjamin on 18/02/2021.
//

#ifndef HID_MOUSE_WAVE_H
#define HID_MOUSE_WAVE_H

#include "effects.h"

#define LEFT 0
#define RIGHT 1
#define MIDDLE 2
#define END 3

typedef struct {
    uint8_t direction;
    uint8_t first_color_idx;
} wave_effect;

void wave(wave_effect *te, struct led_rgb *pixels, int len);

#endif //HID_MOUSE_WAVE_H
