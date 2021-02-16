//
// Created by benjamin on 04/02/2021.
//

#ifndef LED_WS2812_BREATH_H
#define LED_WS2812_BREATH_H

#include "effects.h"

#define UP 0
#define HIGH 1
#define DOWN 2
#define LOW 3

typedef struct {
    size_t color;
    int timestamp;
    int stage;
    int rc;
} breath_effect;

void breath(breath_effect *te, struct led_rgb *pixels, int len);

#endif //LED_WS2812_BREATH_H
