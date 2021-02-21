//
// Created by benjamin on 21/02/2021.
//

#include <string.h>
#include <drivers/led_strip.h>

#include "effects.h"

void static_(effect_union *eu, struct led_rgb *pixels, int len) {
    static_effect *se = &eu->static_;

    for (int i = 0; i < len; i++) {
        memcpy(&pixels[i], &se->color, sizeof(struct led_rgb));
    }
}
