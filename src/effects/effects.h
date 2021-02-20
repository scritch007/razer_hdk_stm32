//
// Created by benjamin on 16/02/2021.
//

#ifndef HID_MOUSE_EFFECTS_H
#define HID_MOUSE_EFFECTS_H


#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }

static const struct led_rgb colors[] = {
        RGB(0xff, 0x00, 0x00), /* red */
        RGB(0x00, 0xff, 0x00), /* green */
        RGB(0x00, 0x00, 0xff), /* blue */
        RGB(0xff, 0xff, 0xff),  /* white */
};

#endif //HID_MOUSE_EFFECTS_H
