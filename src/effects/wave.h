//
// Created by benjamin on 18/02/2021.
//

#ifndef HID_MOUSE_WAVE_H
#define HID_MOUSE_WAVE_H

#define LEFT 0
#define RIGHT 1
#define MIDDLE 2
#define END 3

typedef struct {
    uint8_t direction;
    uint8_t first_color_idx;
} wave_effect;


#endif //HID_MOUSE_WAVE_H
