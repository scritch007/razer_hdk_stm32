//
// Created by benjamin on 04/02/2021.
//

#include <errno.h>
#include <string.h>

#include <logging/log.h>

LOG_MODULE_REGISTER(breath);

#include <zephyr.h>
#include <drivers/led_strip.h>
#include <sys/util.h>

#include "breath.h"


void apply_brightness(struct led_rgb *led, int brightness) {
    led->r = (led->r * brightness) / 255;
    led->b = (led->b * brightness) / 255;
    led->g = (led->g * brightness) / 255;
}

void breath(breath_effect *te, struct led_rgb *pixels, int len) {
    struct led_rgb dimmed_color = colors[te->color];
    if (te->timestamp == 0){
        te->timestamp = k_uptime_get();
    }

    int delta = k_uptime_get() - te->timestamp;
    int brightness = 0;
    int duration = 1500;

    switch (te->stage) {
        case LOW:
            brightness = 0;
            duration = 2000;
            break;
        case HIGH:
            brightness = 255;
            duration = 2000;
            break;
        case UP:
            brightness = 255 * delta / duration;
            break;
        case DOWN:
            brightness = 255 - 255 * delta / duration;
            break;
    }
    apply_brightness(&dimmed_color, brightness);

    memset(pixels, 0, len);

    for (int i = 0; i < len; i++) {
        memcpy(&pixels[i], &dimmed_color, sizeof(struct led_rgb));
    }

    if (delta > duration) {
        te->stage++;
        te->timestamp = k_uptime_get();
        if (te->stage > 3) {
            te->stage = 0;
            te->color++;
            if (te->color == ARRAY_SIZE(colors)) {
                te->color = 0;
            }
        }
    }
}
