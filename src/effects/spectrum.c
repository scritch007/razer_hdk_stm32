//
// Created by benjamin on 19/02/2021.
//

#include <errno.h>
#include <string.h>

#include <logging/log.h>

LOG_MODULE_REGISTER(spectrum);

#include <drivers/led_strip.h>
#include <sys/util.h>

#include "spectrum.h"

#define SPECTRUM_RAMP_UP 1000
#define SPECTRUM_HIGH 1000

int8_t update_color(uint8_t next, uint8_t current, int elapsed) {
    if (next > current) {
        return current + (((next - current) * (elapsed - SPECTRUM_RAMP_UP)) / SPECTRUM_RAMP_UP);
    } else if (next < current) {
        return current - (((current - next) * (elapsed - SPECTRUM_RAMP_UP)) / SPECTRUM_RAMP_UP);
    }
    return next;
}

void spectrum(spectrum_effect *te, struct led_rgb *pixels, int len) {
    int64_t now = k_uptime_get();
    if (te->timestamp == 0) {
        te->timestamp = k_uptime_get();
    }
    int color = ((now - te->timestamp) / (SPECTRUM_HIGH + SPECTRUM_RAMP_UP)) % ARRAY_SIZE(colors);
    int next = (color + 1) % ARRAY_SIZE(colors);
    int elapsed = (now - te->timestamp) % (SPECTRUM_HIGH + SPECTRUM_RAMP_UP);

    struct led_rgb new_color = colors[color];
    struct led_rgb next_c = colors[next];
    struct led_rgb current_c = colors[color];
    if (elapsed > SPECTRUM_HIGH) {
        new_color.r = update_color(next_c.r, current_c.r, elapsed);
        new_color.g = update_color(next_c.g, current_c.g, elapsed);
        new_color.b = update_color(next_c.b, current_c.b, elapsed);

        LOG_HEXDUMP_INF(&current_c, sizeof(struct led_rgb), "Spectrum current");
        LOG_HEXDUMP_INF(&new_color, sizeof(struct led_rgb), "Spectrum");
        LOG_HEXDUMP_INF(&next_c, sizeof(struct led_rgb), "Spectrum next");

    }
    for (int i = 0; i < len; i++) {
        memcpy(&pixels[i], &new_color, sizeof(struct led_rgb));
    }
}

