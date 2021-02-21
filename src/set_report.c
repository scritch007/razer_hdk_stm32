//
// Created by benjamin on 10/02/2021.
//
#include <zephyr.h>
#include <device.h>

#include <usb/usb_device.h>

#include "razer.h"
#include "set_report.h"
#include "strip.h"
#include "effects/spectrum.h"

#define LOG_LEVEL LOG_LEVEL_DBG
LOG_MODULE_REGISTER(set_report);


int parse_08_requests(int id, const struct device *dev_data,
                      struct usb_setup_packet *setup, int32_t *len,
                      uint8_t **data) {
    if (*len < RAZER_REPORT_LENGTH) return -ENOTSUP;

    struct razer_report *report = &gContext.current_report;
    memcpy(&gContext.current_report, *data, *len);
    gContext.state = STATE_RUNNING;

    LOG_HEXDUMP_INF(*data, *len, "set report");

    switch (report->command_class) {
        case 0x0f: {
            // Extended command
            switch (report->command_id.id) {
                case 0x84:
                    // Get brightness
                    report->arguments[2] = gContext.config.brightness[report->arguments[1]];

                    return 0;
                case 0x04:

                    // Set brightness or get brightness;
                    gContext.config.brightness[report->arguments[1]] = report->arguments[2];
#if CONFIG_USB_RAZER_TYPE == 0
                    if (report->arguments[1] == 0x00) {
                        gContext.config.brightness[0x05] = report->arguments[2];
                    }
#endif
                    gContext.save = true;
                    return 0;
                case 0x03:
                    gContext.config.current_effect = CUSTOM;
                    // This sets the configuration for the line:
                    struct row_req *req = (struct row_req *) &report->arguments;
                    for (int i = 0; i <= (req->last - req->first); i++) {
                        gContext.row[req->row * HDK_LED_STRIP_LENGTH + req->first + i].r = req->leds[i].r;
                        gContext.row[req->row * HDK_LED_STRIP_LENGTH + req->first + i].g = req->leds[i].g;
                        gContext.row[req->row * HDK_LED_STRIP_LENGTH + req->first + i].b = req->leds[i].b;
                    }
                    LOG_DBG("Updating %02X %d, %d", req->row, req->first, req->last - req->first);
                    return 0;
                case 0x02: {
                    // Set effect now we need to dissect the effect.
                    switch (report->arguments[2]) {
                        case 0x02:
                            LOG_INF("Breath");
                            set_effect(BREATH, true);
                            break;
                        case 0x07:
                            LOG_INF("Starlight");
                            break;
                        case 0x04:
                            LOG_INF("Wave");
                            set_effect(WAVE, true);
                            gContext.config.effect.wave.direction = report->arguments[3];
                            break;
                        case 0x03:
                            LOG_INF("Spectrum");
                            set_effect(WAVE, true);
                            break;
                        case 0x01:
                            LOG_INF("Static");
                            set_effect(STATIC, true);
                            gContext.config.effect.static_.color.r = report->arguments[6];
                            gContext.config.effect.static_.color.g = report->arguments[7];
                            gContext.config.effect.static_.color.b = report->arguments[8];
                            break;
                        case 0x86:
                            LOG_INF("SET DPI??");
                            break;
                        case 0x00: {
                            LOG_INF("SET None");
                            set_effect(STATIC, true);
                            // None is static with no led set.
                            break;
                        }
                        default:
                            LOG_ERR("Unknown effect %02X", report->arguments[2]);
                    }
                    gContext.save = report->arguments[0]==1;
                    break;
                }
                default:
                    LOG_ERR("Unhandled %02X", report->command_id.id);

            }
        }
            break;
        case 0x00: {
            switch (report->command_id.id) {
                case 0x82:
                    //gContext.serial_requested = true;
                    memcpy(report, serial, RAZER_REPORT_LENGTH);
                    break;
                case 0xB5:
                    // Number of plugged in lines

                    report->arguments[1] = HDK_LED_STRIP_LENGTH; // Force only one HDK plugged in
                    if (STRIP_NUM_PIXELS > HDK_LED_STRIP_LENGTH) {
                        report->arguments[2] = HDK_LED_STRIP_LENGTH;
                    }
                    if (STRIP_NUM_PIXELS > HDK_LED_STRIP_LENGTH * 2) {
                        report->arguments[3] = HDK_LED_STRIP_LENGTH;
                    }
                    if (STRIP_NUM_PIXELS > HDK_LED_STRIP_LENGTH * 3) {
                        report->arguments[4] = HDK_LED_STRIP_LENGTH;
                    }
                    break;
                case 0x04:
                    // Set mode for the device;
                    break;
                default:
                    LOG_HEXDUMP_ERR(*data, *len, "Unknown report command id");
            }
            break;
        }
        case 0x03: {
            switch (report->command_id.id) {
                case 0x83: // Get brightness standard
                    report->arguments[2] = gContext.config.brightness[report->arguments[1]];
                    break;
                case 0x82: // Get led effect standard
                case 0x87:
                    // Force answer. Don't know why we have this 0x03
#if CONFIG_USB_RAZER_TYPE == 1
                    report->arguments[2] = 0x03; // 0x05 was on the HDK
#elif CONFIG_USB_RAZER_TYPE == 0
                    report->arguments[2] = 0x05; // 0x05 was on the HDK
#endif//CONFIG_USB_RAZER_TYPE == 1
                    report->arguments[2] = 0x03; // 0x05 was on the HDK
                case 0x85:
                case 0x86:
                    report->arguments[0] = 0x01;
            }
        }
            break;
        default:
            LOG_INF("Unhandled command class %02X", report->command_class);
            break;
    }

    return 0;
}