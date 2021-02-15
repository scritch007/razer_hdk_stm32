//
// Created by benjamin on 10/02/2021.
//
#include <zephyr.h>
#include <device.h>

#include <usb/usb_device.h>

#include "razer.h"
#include "set_report.h"
#include "strip.h"

#define LOG_LEVEL LOG_LEVEL_DBG
LOG_MODULE_REGISTER(set_report);


int parse_08_requests(int id, const struct device *dev_data,
                      struct usb_setup_packet *setup, int32_t *len,
                      uint8_t **data) {
    if (*len < RAZER_REPORT_LENGTH) return -ENOTSUP;

    struct razer_report *report = &gContext.current_report;
    memcpy(&gContext.current_report, *data, *len);
    gContext.state = STATE_RUNNING;

    switch (report->command_id.id) {
        case 0x02:
            // Set Effect
            break;
        case 0xB5:
            // Number of plugged in lines
            report->arguments[1] = STRIP_NUM_PIXELS; // Force only one HDK plugged in
            break;
        case 0x84:
            // Get brightness
            report->arguments[2] = gContext.brightness[report->arguments[1]];
            return 0;
        case 0x04:
            // Set brightness or get brightness;
            gContext.brightness[report->arguments[1]] = report->arguments[2];
            return 0;
        case 0x82:
            switch (report->command_class) {
                case 0x00:
                    //gContext.serial_requested = true;
                    memcpy(report, serial, RAZER_REPORT_LENGTH);
                    break;
                case 0x03:
                default:
                    return -ENOTSUP;
            }

            break;
        case 0x87:
            // Force answer. Don't know why we have this 0x03
            report->arguments[2] = 0x03; // 0x05 was on the HDK
        case 0x85:
        case 0x86:
            report->arguments[0] = 0x01;
        case 0x03: {
            // This sets the configuration for the line:
            struct row_req *req = (struct row_req *) &report->arguments;
            for (int i = 0; i < (req->last - req->first); i++) {
                gContext.row[req->row][req->first + i].r = req->leds[i].r;
                gContext.row[req->row][req->first + i].g = req->leds[i].g;
                gContext.row[req->row][req->first + i].b = req->leds[i].b;
            }
            //memcpy(&gContext.row[req->row][req->first], &req->leds, (req->last - req->first) * sizeof(struct led_rgb));
            LOG_INF("Updating %02X %d, %d", req->row, req->first, req->last - req->first);
            break;
        }
    }

    switch (report->command_id.id) {
        case 0x03:
            LOG_INF("Breath");
            break;
        case 0x07:
            LOG_INF("Starlight");
            break;
        case 0x04:
            LOG_INF("Spectrum");
            break;
        case 0x06:
            LOG_INF("Static");
            break;
        case 0x86:
            LOG_INF("SET DPI??");
            break;
        default:
            LOG_ERR("Unknown effect %02X", report->command_id.id);
            return 0;
    }

    return 0;
}