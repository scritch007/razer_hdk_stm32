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
#include "effects.h"


void apply_brightness(struct led_rgb *led, int brightness) {
    led->r = (led->r * brightness) / 255;
    led->b = (led->b * brightness) / 255;
    led->g = (led->g * brightness) / 255;
}

void breath(effect_union *eu, struct led_rgb *pixels, int len) {
    breath_effect *te = &eu->breath;
    if (te->timestamp == 0) {
        te->timestamp = k_uptime_get();
        te->duration = 1500;
        te->stage = UP;
    }

    int64_t delta = k_uptime_get() - te->timestamp;
    int brightness = 0;


    if (delta > te->duration) {
        te->stage++;
        te->timestamp = k_uptime_get() - (delta - te->duration);
        switch (te->stage) {
            case LOW:
            case HIGH:
                te->duration = 2000;
                break;
            default:
                te->stage = UP;
                te->color++;
                if (te->color == ARRAY_SIZE(colors)) {
                    te->color = 0;
                }
            case UP:
            case DOWN:
                te->duration = 1500;
                break;
        }
        delta = 0;
    }

    switch (te->stage) {
        case LOW:
            brightness = 0;
            break;
        case HIGH:
            brightness = 255;
            break;
        case UP:
            brightness = (255 * delta) / te->duration;
            break;
        case DOWN:
            brightness = 255 - (255 * delta) / te->duration;
            break;
    }

    struct led_rgb dimmed_color = colors[te->color];
    apply_brightness(&dimmed_color, brightness);

    memset(pixels, 0, len);

    for (int i = 0; i < len; i++) {
        memcpy(&pixels[i], &dimmed_color, sizeof(struct led_rgb));
    }

}
