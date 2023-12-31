//
// Created by benjamin on 04/02/2021.
//

#ifndef LED_WS2812_BREATH_H
#define LED_WS2812_BREATH_H

#define UP 0
#define HIGH 1
#define DOWN 2
#define LOW 3

typedef struct {
    int64_t timestamp;
    int duration;
    struct led_rgb colors[2];
    size_t color;
    uint8_t stage;
    uint8_t color_count;
} breath_effect;

#endif //LED_WS2812_BREATH_H
