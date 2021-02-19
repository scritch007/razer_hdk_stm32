//
// Created by benjamin on 19/02/2021.
//

#ifndef HID_MOUSE_SPECTRUM_H
#define HID_MOUSE_SPECTRUM_H
#include "effects.h"

typedef struct {
    int timestamp;
    uint8_t current_color;
} spectrum_effect;

void spectrum(spectrum_effect *te, struct led_rgb *pixels, int len);
#endif //HID_MOUSE_SPECTRUM_H
