//
// Created by benjamin on 12/02/2021.
//
#include <zephyr.h>
#include "razer.h"

#if CONFIG_USB_RAZER_TYPE == 0
static const uint8_t hid0_report_desc[] = {
        0x05, 0x01, 0x09, 0x06, 0xa1, 0x01, 0x05, 0x07, 0x19, 0xe0, 0x29, 0xe7, 0x15, 0x00, 0x25, 0x01,
        0x75, 0x01, 0x95, 0x08, 0x81, 0x02, 0x81, 0x01, 0x19, 0x00, 0x2a, 0xff, 0x00, 0x15, 0x00, 0x26,
        0xff, 0x00, 0x75, 0x08, 0x95, 0x06, 0x81, 0x00, 0x05, 0x08, 0x19, 0x01, 0x29, 0x03, 0x15, 0x00,
        0x25, 0x01, 0x75, 0x01, 0x95, 0x03, 0x91, 0x02, 0x95, 0x05, 0x91, 0x01, 0xc0,
};

static const uint8_t hid1_report_desc[] = {
        0x05, 0x01, 0x09, 0x06, 0xa1, 0x01, 0x85, 0x01, 0x05, 0x07, 0x19, 0xe0, 0x29, 0xe7, 0x15, 0x00,
        0x25, 0x01, 0x75, 0x01, 0x95, 0x08, 0x81, 0x02, 0x19, 0x00, 0x2a, 0xff, 0x00, 0x15, 0x00, 0x26,
        0xff, 0x00, 0x75, 0x08, 0x95, 0x0e, 0x81, 0x00, 0xc0, 0x05, 0x0c, 0x09, 0x01, 0xa1, 0x01, 0x85,
        0x02, 0x19, 0x00, 0x2a, 0x3c, 0x02, 0x15, 0x00, 0x26, 0x3c, 0x02, 0x95, 0x01, 0x75, 0x10, 0x81,
        0x00, 0x75, 0x08, 0x95, 0x0d, 0x81, 0x01, 0xc0, 0x05, 0x01, 0x09, 0x80, 0xa1, 0x01, 0x85, 0x03,
        0x19, 0x81, 0x29, 0x83, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x03, 0x81, 0x02, 0x95, 0x05,
        0x81, 0x01, 0x75, 0x08, 0x95, 0x0e, 0x81, 0x01, 0xc0, 0x05, 0x01, 0x09, 0x00, 0xa1, 0x01, 0x85,
        0x04, 0x09, 0x03, 0x15, 0x00, 0x26, 0xff, 0x00, 0x35, 0x00, 0x46, 0xff, 0x00, 0x75, 0x08, 0x95,
        0x0f, 0x81, 0x00, 0xc0, 0x05, 0x01, 0x09, 0x00, 0xa1, 0x01, 0x85, 0x05, 0x09, 0x03, 0x15, 0x00,
        0x26, 0xff, 0x00, 0x35, 0x00, 0x46, 0xff, 0x00, 0x75, 0x08, 0x95, 0x0f, 0x81, 0x00, 0xc0,
};
static const uint8_t hid2_report_desc[] = {
        0x05, 0x01, 0x09, 0x02, 0xa1, 0x01, 0x09, 0x01, 0xa1, 0x00, 0x05, 0x09, 0x19, 0x01, 0x29, 0x05,
        0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x05, 0x81, 0x02, 0x75, 0x01, 0x95, 0x03, 0x81, 0x03,
        0x06, 0x00, 0xff, 0x09, 0x40, 0x75, 0x08, 0x95, 0x02, 0x15, 0x81, 0x25, 0x7f, 0x81, 0x02, 0x05,
        0x01, 0x09, 0x38, 0x15, 0x81, 0x25, 0x7f, 0x75, 0x08, 0x95, 0x01, 0x81, 0x06, 0x09, 0x30, 0x09,
        0x31, 0x16, 0x00, 0x80, 0x26, 0xff, 0x7f, 0x75, 0x10, 0x95, 0x02, 0x81, 0x06, 0xc0, 0x06, 0x00,
        0xff, 0x09, 0x02, 0x15, 0x00, 0x25, 0x01, 0x75, 0x08, 0x95, 0x5a, 0xb1, 0x01, 0xc0,
};

const char serial[90] = {
        0x02, 0x08, 0x00, 0x00, 0x00, 0x16, 0x00, 0x82, 0x49, 0x4f, 0x31, 0x38, 0x34, 0x32, 0x55, 0x30,
        0x32, 0x34, 0x30, 0x30, 0x34, 0x33, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xca, 0x00,
};

bool get_report_hid(int reportID, char **report, int *len) {
    switch (reportID) {
        case 0:
            *report = hid0_report_desc;
            *len = sizeof(hid0_report_desc);
            break;
#if CONFIG_USB_HID_DEVICE_COUNT > 1
            case 1:
            *report = hid1_report_desc;
            *len = sizeof(hid1_report_desc);
            break;
#endif
#if CONFIG_USB_HID_DEVICE_COUNT > 2
            case 2:
            *report = hid2_report_desc;
            *len = sizeof(hid2_report_desc);
            break;
#endif
        default:
            return false;
    }
    return true;
}

#endif