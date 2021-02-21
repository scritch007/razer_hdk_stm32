//
// Created by benjamin on 16/02/2021.
//

#ifndef HID_MOUSE_EFFECTS_H
#define HID_MOUSE_EFFECTS_H

#include "breath.h"
#include "wave.h"
#include "spectrum.h"
#include "static.h"

#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }

static const struct led_rgb colors[] = {
        RGB(0xff, 0x00, 0x00), /* red */
        RGB(0x00, 0xff, 0x00), /* green */
        RGB(0x00, 0x00, 0xff), /* blue */
        RGB(0xff, 0xff, 0xff),  /* white */
};


typedef union  {
    breath_effect breath;
    wave_effect wave;
    spectrum_effect spectrum;
    static_effect static_;
} effect_union;

#define BREATH 1
#define CUSTOM 2
#define WAVE   3
#define SPECTRUM 4
#define STATIC 5
#define NONE 6

typedef void (*apply_effect)(effect_union *eu, struct led_rgb *pixels, int len);


void breath(effect_union *eu, struct led_rgb *pixels, int len);
void wave(effect_union *eu, struct led_rgb *pixels, int len);
void spectrum(effect_union *eu, struct led_rgb *pixels, int len);
void static_(effect_union *eu, struct led_rgb *pixels, int len);

#endif //HID_MOUSE_EFFECTS_H
