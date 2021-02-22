//
// Created by benjamin on 18/02/2021.
//

#include <logging/log.h>

LOG_MODULE_REGISTER(wave);

#include <drivers/led_strip.h>
#include <sys/util.h>

#include "effects.h"


uint8_t *Wheel(uint8_t wheel_pos, uint8_t direction) {
    static uint8_t c[3];

    /* if (wheel_pos < 10) {
         c[0] = 0;
         c[1] = 0;
         c[2] = 0;
     } else */if (wheel_pos < 85) {
        c[0] = wheel_pos * 3;
        c[1] = 255 - wheel_pos * 3;
        c[2] = 0;
    } else if (wheel_pos < 170) {
        wheel_pos -= 85;
        c[0] = 255 - wheel_pos * 3;
        c[1] = 0;
        c[2] = wheel_pos * 3;
    } else {
        wheel_pos -= 170;
        c[0] = 0;
        c[1] = wheel_pos * 3;
        c[2] = 255 - wheel_pos * 3;

    }
    return c;
}


void wave(effect_union *eu, struct led_rgb *pixels, int len) {
    wave_effect *te = &eu->wave;
    uint8_t *c;
    uint8_t factor = 5;

    for (int i = 0; i < len; i++) {
        uint8_t id;
        switch (te->direction) {
            case 1:
                id = ((i * 256 / len) - te->first_color_idx * factor) & 255;
                break;
            default:
            case 2:
                id = ((i * 256 / len) + te->first_color_idx * factor) & 255;
                break;
        }
        c = Wheel(id, te->direction);
        pixels[i].r = c[0];
        pixels[i].g = c[1];
        pixels[i].b = c[2];
    }
    //LOG_HEXDUMP_INF(pixels, len * sizeof(struct led_rgb), "wave");

    te->first_color_idx++;

    if (te->first_color_idx >= 256 * 5) {
        te->first_color_idx = 0;
    }
}
