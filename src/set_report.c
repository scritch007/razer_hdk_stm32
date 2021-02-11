//
// Created by benjamin on 10/02/2021.
//
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>

#include <usb/usb_device.h>
#include <usb/class/usb_hid.h>

#include "razer.h"
#include "set_report.h"

int parse_08_requests(int id, const struct device *dev_data,
                      struct usb_setup_packet *setup, int32_t *len,
                      uint8_t **data) {
    if (*len < 90) return -ENOTSUP;
    switch ((*data)[7]) {
        case 0x82:
            gContext.serial_requested = true;
            break;
        case 0x87:
            // Force answer. Don't know why we have this 0x03
            (*data)[10] = 0x03;
        case 0x85:
        case 0x86:
            (*data)[8] = 0x01;
        case 0x84:
        case 0x04:
            memcpy(&gContext.current_report, *data, *len);
            gContext.state = STATE_RUNNING;
            break;
    }

    return 0;
}