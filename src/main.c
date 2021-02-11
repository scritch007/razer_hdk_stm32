/*
 * Copyright (c) 2018 qianfan Zhao
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>

#include <usb/usb_device.h>
#include <usb/class/usb_hid.h>

#include "razer.h"
#include "set_report.h"

#define LOG_LEVEL LOG_LEVEL_DBG
LOG_MODULE_REGISTER(main);

static const uint8_t hid0_report_desc[] = {0x05, 0x01, 0x09, 0x02, 0xa1, 0x01, 0x09, 0x01, 0xa1, 0x00, 0x05, 0x09, 0x19,
                                           0x01, 0x29, 0x05,
                                           0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x05, 0x81, 0x02, 0x75, 0x01, 0x95,
                                           0x03, 0x81, 0x03,
                                           0x06, 0x00, 0xff, 0x09, 0x40, 0x75, 0x08, 0x95, 0x02, 0x15, 0x81, 0x25, 0x7f,
                                           0x81, 0x02, 0x05,
                                           0x01, 0x09, 0x38, 0x15, 0x81, 0x25, 0x7f, 0x75, 0x08, 0x95, 0x01, 0x81, 0x06,
                                           0x09, 0x30, 0x09,
                                           0x31, 0x16, 0x00, 0x80, 0x26, 0xff, 0x7f, 0x75, 0x10, 0x95, 0x02, 0x81, 0x06,
                                           0xc0, 0x06, 0x00,
                                           0xff, 0x09, 0x02, 0x15, 0x00, 0x25, 0x01, 0x75, 0x08, 0x95, 0x5a, 0xb1, 0x01,
                                           0xc0,};

static const uint8_t hid1_report_desc[] = {
        /* 0x05, 0x01,		USAGE_PAGE (Generic Desktop)		*/
        HID_GI_USAGE_PAGE, USAGE_GEN_DESKTOP,
        /* 0x09, 0x00,		USAGE (Undefined)			*/
        HID_LI_USAGE, USAGE_GEN_DESKTOP_KEYBOARD,
        /* 0xa1, 0x01,		COLLECTION (Application)		*/
        HID_MI_COLLECTION, COLLECTION_APPLICATION,
        /* 0x85, 0x01,			REPORT_ID (1)			*/
        HID_GI_REPORT_ID, 0x01,
        /* 0x05, 0x07, GLOBAL USAGE Keyboard/KeyPad */
        HID_GI_USAGE_PAGE, USAGE_GEN_DESKTOP_KEYPAD,
        /* 0x19, 0xe0, Local Item Minimum usage*/
        HID_LI_USAGE_MIN(1), 0xE0,
        /* 0x29, 0xe7, Local Item Maximum usage*/
        HID_LI_USAGE_MAX(1), 0xE7,
        /* 0x15, 0x00,			LOGICAL_MINIMUM one-byte (0)	*/
        HID_GI_LOGICAL_MIN(1), 0x00,
        /* 0x25, 0x01		LOGICAL_MAXIMUM one-byte (01)	*/
        HID_GI_LOGICAL_MAX(1), 0x01,
        /* 0x75, 0x01,			REPORT_SIZE (1) in bits		*/
        HID_GI_REPORT_SIZE, 0x01,
        /* 0x95, 0x08,			REPORT_COUNT (8)		*/
        HID_GI_REPORT_COUNT, 0x08,
        /* 0x81, 0x02 Main Item Input */
        HID_MI_INPUT, 0x02,
        /* 0x19, 0x00, Local Item Minimum usage */
        HID_LI_USAGE_MIN(1), 0x00,
        /* 0x29, 0xff, Local item Maximum usage */
        HID_LI_USAGE_MAX(2), 0xff, 0x00,
        /* 0x15, 0x00,			LOGICAL_MINIMUM one-byte (0)	*/
        HID_GI_LOGICAL_MIN(1), 0x00,
        /* 0x25, 0xff		LOGICAL_MAXIMUM one-byte (ff)	*/
        HID_GI_LOGICAL_MAX(2), 0xff, 0x00,
        /* 0x75, 0x08,			REPORT_SIZE (1) in bits		*/
        HID_GI_REPORT_SIZE, 0x08,
        /* 0x95, 0x0e,			REPORT_COUNT (14)		*/
        HID_GI_REPORT_COUNT, 0x0e,
        /* 0x81, 0x00 Main item input 0 */
        HID_MI_INPUT, 0x00,
        /* 0xc0 main item end collection */
        HID_MI_COLLECTION_END,
        /* 0x05, 0x0C Global item usage */
        HID_GI_USAGE_PAGE, 0x0C,
        /* 0x09, 0x01 Local item usage */
        HID_LI_USAGE, 0x01,

        /******************/
        /* 0xa1, 0x01 Main Item collection (application) */
        HID_MI_COLLECTION, COLLECTION_APPLICATION,
        /* 0x85, 0x02 Global Item Report ID */
        HID_GI_REPORT_ID, 0x02,
        /* 0x19, 0x00 Local Item usage minimum (0) */
        HID_LI_USAGE_MIN(1), 0x00,
        /* 0x2a, 0x3c, 0x02 Local Item usage maximum (0x23C) */
        HID_LI_USAGE_MAX(2), 0x3C, 0x02,
        /* 0x15, 0x00 Global Item usage minimum (0) */
        HID_GI_LOGICAL_MIN(1), 0x00,
        /* 0x26, 0x3c, 0x02 Glabl item Logical max (572) */
        HID_GI_LOGICAL_MAX(2), 0x3c, 0x02,
        /* 0x95, 0x01 Report Count 1 */
        HID_GI_REPORT_COUNT, 0x01,
        /* 0x75, 0x10 Report Size 16 */
        HID_GI_REPORT_SIZE, 0x10,
        /* 0x81, 0x00 Main Input */
        HID_MI_INPUT, 0x00,
        /* 0x75, 0x08 Report Size 8 */
        HID_GI_REPORT_SIZE, 0x08,
        /* 0x95, 0x01 Report Count 13 */
        HID_GI_REPORT_COUNT, 0xd,
        /* 0x81, 0x01 Main input */
        HID_MI_INPUT, 0x01,
        /* 0xc0 Main Input end collection */
        HID_MI_COLLECTION_END,
        /***********/

        /* 0x05, 0x01 Global item usage 1 */
        HID_GI_USAGE_PAGE, 0x01,
        /* 0x09, 0x80 Local Item Usage */
        HID_LI_USAGE, 0x80,
        /* 0xa1, 0x01 Main item collection (application) */
        HID_MI_COLLECTION, COLLECTION_APPLICATION,
        /* 0x85, 0x03 Global Item Report ID */
        HID_GI_REPORT_ID, 0x03,
        /* 0x19, 0x81, Local usage minimum */
        HID_LI_USAGE_MIN(1), 0x81,
        /* 0x29, 0x83, Local usage maximum */
        HID_LI_USAGE_MAX(1), 0x83,
        /* 0x15, 0x00 Global Logical Usage min */
        HID_GI_LOGICAL_MIN(1), 0x00,
        /* 0x25, 0x01 Global logical usage max */
        HID_GI_LOGICAL_MAX(1), 0x01,
        /* 0x75, 0x01 Report size */
        HID_GI_REPORT_SIZE, 0x01,
        /* 0x95, 0x03 Report Count */
        HID_GI_REPORT_COUNT, 0x03,
        /* 0x81, 0x02 Main Input */
        HID_MI_INPUT, 0x02,
        /* 0x95, 0x05 Report Count */
        HID_GI_REPORT_COUNT, 0x05,
        /* 0x81, 0x01 Main Input */
        HID_MI_INPUT, 0x01,
        /* 0x75, 0x08 Report Size */
        HID_GI_REPORT_SIZE, 0x08,
        /* 0x95, 0x0E Report count */
        HID_GI_REPORT_COUNT, 0x0E,
        /* 0x81, 0x01 Main input */
        HID_MI_INPUT, 0x01,
        /* 0xc0 Main collection end */
        HID_MI_COLLECTION_END,
        /*******/

        /*******/
        /* 0x05, 0x01 Global item usage */
        HID_GI_USAGE_PAGE, 0x01,
        /* 0x09, 0x00 Local item usage */
        HID_LI_USAGE, 0x00,
        /* 0xa1, 0x01 Main Collection (application) */
        HID_MI_COLLECTION, COLLECTION_APPLICATION,
        /* 0x85, 0x04 Report ID */
        HID_GI_REPORT_ID, 0x04,
        /* 0x09, 0x03 Local usage */
        HID_LI_USAGE, 0x03,
        /* 0x15, 0x00 Global Logical min */
        HID_GI_LOGICAL_MIN(1), 0x00,
        /* 0x26 0xFF, 0x00 Global logical max */
        HID_GI_LOGICAL_MAX(2), 0xff, 0x00,
        /* 0x35, 0x00 Global physical min */
        0x35, 0x00,
        /* 0x46, 0xff, 0x00 Global physical max */
        0x46, 0xff, 0x00,
        /* 0x75, 0x08 Report size */
        HID_GI_REPORT_SIZE, 0x08,
        /* 0x95, 0x03 Report count */
        HID_GI_REPORT_COUNT, 0x03,
        /* 0x81, 0x00 Main input */
        HID_MI_INPUT, 0x00,
        /* 0xc0 MI end */
        HID_MI_COLLECTION_END,
        /*********/

        /* 0x05, 0x01 Global item usage */
        HID_GI_USAGE_PAGE, 0x01,
        /* 0x09, 0x00 Local item usage 0x00 */
        HID_LI_USAGE, 0x00,

        /*******/
        /* 0xa1, 0x01 Main collection (application) */
        HID_MI_COLLECTION, COLLECTION_APPLICATION,
        /* 0x85, 0x05  Report ID */
        HID_GI_REPORT_ID, 0x05,
        /* 0x09, 0x03 local item usage 0x03 */
        HID_LI_USAGE, 0x03,
        /* 0x15, 0x00 Global logical min */
        HID_GI_LOGICAL_MIN(1), 0x00,
        /* 0x26, 0xff, 0x00 Gloabl logical max */
        HID_GI_LOGICAL_MAX(2), 0xff, 0x00,
        /* 0x35, 0x00 Global physical min */
        0x35, 0x00,
        /* 0x46, 0xff, 0x00 Global physical max */
        0x46, 0xff, 0x00,
        /* 0x75, 0x08, Report size */
        HID_GI_REPORT_SIZE, 0x08,
        /* 0x95,0x0F, Report count */
        HID_GI_REPORT_COUNT, 0x0F,
        /* 0x81, 0x00  Main Input */
        HID_MI_INPUT, 0x00,
        /* 0xC0 MI collection end */
        HID_MI_COLLECTION_END,
};

static const uint8_t hid2_report_desc[] = {0x05, 0x01, 0x09, 0x06, 0xa1, 0x01, 0x05, 0x07, 0x19, 0xe0, 0x29, 0xe7, 0x15,
                                           0x00, 0x25, 0x01,
                                           0x75, 0x01, 0x95, 0x08, 0x81, 0x02, 0x81, 0x01, 0x19, 0x00, 0x2a, 0xff, 0x00,
                                           0x15, 0x00, 0x26,
                                           0xff, 0x00, 0x75, 0x08, 0x95, 0x06, 0x81, 0x00, 0x05, 0x08, 0x19, 0x01, 0x29,
                                           0x03, 0x15, 0x00,
                                           0x25, 0x01, 0x75, 0x01, 0x95, 0x03, 0x91, 0x02, 0x95, 0x05, 0x91, 0x01, 0xc0,
};
static const uint8_t hid3_report_desc[] = {0x05, 0x59, 0x09, 0x01, 0xa1, 0x01, 0x85, 0x01, 0x09, 0x02, 0xa1, 0x02, 0x09,
                                           0x03, 0x15, 0x00,
                                           0x27, 0xff, 0xff, 0x00, 0x00, 0x75, 0x10, 0x95, 0x01, 0xb1, 0x03, 0x09, 0x04,
                                           0x09, 0x05, 0x09,
                                           0x06, 0x09, 0x07, 0x09, 0x08, 0x15, 0x00, 0x27, 0xff, 0xff, 0xff, 0x7f, 0x75,
                                           0x20, 0x95, 0x05,
                                           0xb1, 0x03, 0xc0, 0x85, 0x02, 0x09, 0x20, 0xa1, 0x02, 0x09, 0x21, 0x15, 0x00,
                                           0x27, 0xff, 0xff,
                                           0x00, 0x00, 0x75, 0x10, 0x95, 0x01, 0xb1, 0x02, 0xc0, 0x85, 0x03, 0x09, 0x22,
                                           0xa1, 0x02, 0x09,
                                           0x21, 0x15, 0x00, 0x27, 0xff, 0xff, 0x00, 0x00, 0x75, 0x10, 0x95, 0x01, 0xb1,
                                           0x02, 0x09, 0x23,
                                           0x09, 0x24, 0x09, 0x25, 0x09, 0x27, 0x09, 0x26, 0x15, 0x00, 0x27, 0xff, 0xff,
                                           0xff, 0x7f, 0x75,
                                           0x20, 0x95, 0x05, 0xb1, 0x02, 0x09, 0x28, 0x09, 0x29, 0x09, 0x2a, 0x09, 0x2c,
                                           0x09, 0x2d, 0x15,
                                           0x00, 0x26, 0xff, 0x00, 0x75, 0x08, 0x95, 0x05, 0xb1, 0x02, 0xc0, 0x85, 0x04,
                                           0x09, 0x50, 0xa1,
                                           0x02, 0x09, 0x03, 0x09, 0x55, 0x15, 0x00, 0x25, 0x08, 0x75, 0x08, 0x95, 0x02,
                                           0xb1, 0x02, 0x09,
                                           0x21, 0x15, 0x00, 0x27, 0xff, 0xff, 0x00, 0x00, 0x75, 0x10, 0x95, 0x08, 0xb1,
                                           0x02, 0x09, 0x51,
                                           0x09, 0x52, 0x09, 0x53, 0x09, 0x51, 0x09, 0x52, 0x09, 0x53, 0x09, 0x51, 0x09,
                                           0x52, 0x09, 0x53,
                                           0x09, 0x51, 0x09, 0x52, 0x09, 0x53, 0x09, 0x51, 0x09, 0x52, 0x09, 0x53, 0x09,
                                           0x51, 0x09, 0x52,
                                           0x09, 0x53, 0x09, 0x51, 0x09, 0x52, 0x09, 0x53, 0x09, 0x51, 0x09, 0x52, 0x09,
                                           0x53, 0x15, 0x00,
                                           0x26, 0xff, 0x00, 0x75, 0x08, 0x95, 0x18, 0xb1, 0x02, 0xc0, 0x85, 0x05, 0x09,
                                           0x60, 0xa1, 0x02,
                                           0x09, 0x55, 0x15, 0x00, 0x25, 0x08, 0x75, 0x08, 0x95, 0x01, 0xb1, 0x02, 0x09,
                                           0x61, 0x09, 0x62,
                                           0x15, 0x00, 0x27, 0xff, 0xff, 0x00, 0x00, 0x75, 0x10, 0x95, 0x02, 0xb1, 0x02,
                                           0x09, 0x51, 0x09,
                                           0x52, 0x09, 0x53, 0x15, 0x00, 0x26, 0xff, 0x00, 0x75, 0x08, 0x95, 0x03, 0xb1,
                                           0x02, 0xc0, 0x85,
                                           0x06, 0x09, 0x70, 0xa1, 0x02, 0x09, 0x71, 0x15, 0x00, 0x25, 0x01, 0x75, 0x08,
                                           0x95, 0x01, 0xb1,
                                           0x02, 0xc0, 0x85, 0x07, 0x06, 0x00, 0xff, 0x15, 0x00, 0x26, 0xff, 0x00, 0x09,
                                           0x02, 0x75, 0x08,
                                           0x95, 0x3f, 0xb1, 0x02, 0xc0
};

static volatile uint8_t status[4];
static K_SEM_DEFINE(sem, 0, 1);    /* starts off "not available" */
static struct gpio_callback callback[4];


#define MOUSE_BTN_REPORT_POS    0
#define MOUSE_X_REPORT_POS    1
#define MOUSE_Y_REPORT_POS    2


static void status_cb(enum usb_dc_status_code status, const uint8_t *param) {
    LOG_INF("status_cb");
}


int debug_cb(const struct device *dev_data,
             struct usb_setup_packet *setup, int32_t *len,
             uint8_t **data) {
    LOG_INF("Debug callback");
    int size = *len;
    if (size > 20) {
        size = 20;
    }
    LOG_HEXDUMP_INF(*data, size, "Debug");

    return -ENOTSUP;
}

int set_idle_cb(const struct device *dev_data,
                struct usb_setup_packet *setup, int32_t *len,
                uint8_t **data) {
    LOG_INF("Set Idle callback");

    /* TODO: Do something */

    return 0;
}

/**
 * Calculate the checksum for the usb message
 *
 * Checksum byte is stored in the 2nd last byte in the messages payload.
 * The checksum is generated by XORing all the bytes in the report starting
 * at byte number 2 (0 based) and ending at byte 88.
 */
unsigned char razer_calculate_crc(struct razer_report *report) {
    /*second to last byte of report is a simple checksum*/
    /*just xor all bytes up with overflow and you are done*/
    unsigned char crc = 0;
    unsigned char *_report = (unsigned char *) report;

    unsigned int i;
    for (i = 2; i < 88; i++) {
        crc ^= _report[i];
    }

    return crc;
}

/**
 * Get initialised razer report
 */
struct razer_report get_razer_report(unsigned char command_class, unsigned char command_id, unsigned char data_size) {
    struct razer_report new_report = {0};
    memset(&new_report, 0, sizeof(struct razer_report));

    new_report.status = 0x00;
    new_report.transaction_id.parts.device = 0x01;
    new_report.transaction_id.parts.id = 0x02;
    new_report.remaining_packets = 0x00;
    new_report.protocol_type = 0xd4;
    new_report.command_class = command_class;
    new_report.command_id.id = command_id;
    new_report.data_size = data_size;

    return new_report;
}

struct razer_context gContext = {0};
char init_report[23] = {0x01, 0x02, 0x00, 0xd4, 0x11, 0x01, 0x00, 0x50, 0xef, 0x01, 0x00, 0xea, 0xa6, 0x00, 0x00,
                        0x02,
                        0x00, 0x00, 0x00, 0x35, 0x82, 0x00, 0x00
};

char start_report[28] = {0x03, 0x00, 0x00, 0x10, 0x8b, 0x00, 0x00, 0x74, 0x59, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe8,
                         0x03, 0x00, 0x00, 0x04,
                         0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x01, 0x00,};
char start2_report[28] = {0x03, 0x01, 0x00, 0x04, 0x8d, 0x00, 0x00, 0xf8, 0x6f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0xe8, 0x03, 0x00, 0x00, 0x04,
                          0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x01, 0x00,};

char serial[90] = {0x02, 0x08, 0x00, 0x00, 0x00, 0x16, 0x00, 0x82, 0x50, 0x4d, 0x32, 0x30, 0x30, 0x31, 0x48, 0x31, 0x34,
                   0x34, 0x33, 0x38,
                   0x36, 0x38, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0xc2, 0x00,};

int get_report_cb(int ID, const struct device *dev_data,
                  struct usb_setup_packet *setup, int32_t *len,
                  uint8_t **data) {
    LOG_INF("Get report callback %d", ID);

    int size = 90;

    if (gContext.serial_requested) {
        *data = serial;
        *len = 90;
        gContext.serial_requested = false;
        return 0;
    }

    switch (gContext.state) {
        case STATE_INIT: {
            *data = init_report;
            *len = 23;
        }
            break;
        case STATE_START: {
            *data = start_report;
            *len = 28;
            break;
        }
        case STATE_START2: {
            *data = start2_report;
            *len = 28;
            break;
        }
        case STATE_RUNNING: {
            gContext.current_report.status = 0x02;
            gContext.current_report.crc = razer_calculate_crc(&gContext.current_report);
            memcpy(*data, (void *) &gContext.current_report, 90);
            *len = 90;
        }
            break;
        default:
            LOG_ERR("Unknown state");
            break;
    }

    return 0;
}

int get_report_cb0(const struct device *dev_data,
                   struct usb_setup_packet *setup, int32_t *len,
                   uint8_t **data) {
    return get_report_cb(0, dev_data, setup, len, data);
}

int get_report_cb1(const struct device *dev_data,
                   struct usb_setup_packet *setup, int32_t *len,
                   uint8_t **data) {
    return get_report_cb(1, dev_data, setup, len, data);
}

int get_report_cb2(const struct device *dev_data,
                   struct usb_setup_packet *setup, int32_t *len,
                   uint8_t **data) {
    return get_report_cb(2, dev_data, setup, len, data);
}

int get_report_cb3(const struct device *dev_data,
                   struct usb_setup_packet *setup, int32_t *len,
                   uint8_t **data) {
    return get_report_cb(3, dev_data, setup, len, data);
}

int get_protocol(const struct device *dev_data,
                 struct usb_setup_packet *setup, int32_t *len,
                 uint8_t **data) {
    LOG_INF("Get protocol callback");

    /* TODO: Do something */
    int size = *len;
    if (size > 20) {
        size = 20;
    }
    LOG_HEXDUMP_INF(*data, size, "Get Protocol");

    return 0;
}

char report_temp[90];

int set_report(int id, const struct device *dev_data,
               struct usb_setup_packet *setup, int32_t *len,
               uint8_t **data) {
    LOG_INF("Set report callback %d", id);

    /* TODO: Do something */
    int size = *len;

    LOG_HEXDUMP_INF(*data, size, "Set Report");

    switch ((*data)[0]) {
        case 0x02:
            switch ((*data)[1]) {
                case 0x00:
                    gContext.state = STATE_START;
                    break;
                case 0x01:
                    gContext.state = STATE_START2;
                    break;
                default:
                    LOG_ERR("Unknown set report");
                    return -ENOTSUP;
            }
            return 0;
        case 0x00:
            if (*len != 90) {
                LOG_ERR("Unknown set report");
            }
            switch ((*data)[1]) {
                case 0x08:
                case 0x1f:
                    return parse_08_requests(id, dev_data, setup, len, data);
                case 0x0c:
                    gContext.state = STATE_START;
                    break;

                default:
                    return -ENOTSUP;
            }
            break;
        case 0x01:
            return 0;
        case 0x06:
            return 0;
        case 0x03:
            return 0;
        case 0x86:
            return 0;
        case 0x08:
            // This is special ID for requesting information.
            // Check for the ID. 0x82 means serial number
            gContext.state = STATE_RUNNING;
            memcpy(&gContext.current_report, *data, *len);
            return 0;
        case 0x1f:
            gContext.state = STATE_RUNNING;
            memcpy(&gContext.current_report, *data, *len);
            return 0;
        default:
            return -ENOTSUP;
    }


    // Status Trans Packet Proto DataSize Class CMD Args
    // 00     3f    0000   00    06       0f    02  010507000100
    uint8_t dataSize = (*data)[5];
    uint8_t cmd = (*data)[7];
    uint8_t effect = (*data)[10];

    switch (cmd) {
        case 2:
            break;
        case 0x86:
            gContext.state = STATE_RUNNING;
            memcpy(&gContext.current_report, *data, *len);
            return 0;
        case 0x03:
            return 0;
        default:
            LOG_ERR("Unknown command type %02X", cmd);
            return -ENOTSUP;
    }
    if (dataSize < 3) {
        LOG_ERR("DataSize too small %02X", dataSize);
        return -ENOTSUP;
    }

    switch (effect) {
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
            LOG_ERR("Unknown effect %02X", effect);
            return 0;
    }

    return 0;
}

int set_report0(const struct device *dev_data,
                struct usb_setup_packet *setup, int32_t *len,
                uint8_t **data) {
    return set_report(0, dev_data, setup, len, data);
}

int set_report1(const struct device *dev_data,
                struct usb_setup_packet *setup, int32_t *len,
                uint8_t **data) {
    return set_report(1, dev_data, setup, len, data);
}

int set_report2(const struct device *dev_data,
                struct usb_setup_packet *setup, int32_t *len,
                uint8_t **data) {
    return set_report(2, dev_data, setup, len, data);
}

int set_report3(const struct device *dev_data,
                struct usb_setup_packet *setup, int32_t *len,
                uint8_t **data) {
    return set_report(3, dev_data, setup, len, data);
}

static struct hid_ops ops0 = {
        .get_report = get_report_cb0,
        .get_idle = debug_cb,
        .get_protocol = get_protocol,
        .set_report = set_report0,
        .set_idle = set_idle_cb,
};

static struct hid_ops ops1 = {
        .get_report = get_report_cb1,
        .get_idle = debug_cb,
        .get_protocol = get_protocol,
        .set_report = set_report1,
        .set_idle = set_idle_cb,
};

static struct hid_ops ops2 = {
        .get_report = get_report_cb2,
        .get_idle = debug_cb,
        .get_protocol = get_protocol,
        .set_report = set_report2,
        .set_idle = set_idle_cb,
};

static struct hid_ops ops3 = {
        .get_report = get_report_cb3,
        .get_idle = debug_cb,
        .get_protocol = get_protocol,
        .set_report = set_report3,
        .set_idle = set_idle_cb,
};


void main(void) {
    int ret;
    uint8_t report[4] = {0x00};
    const struct device *hid0_dev, *hid1_dev, *hid2_dev, *hid3_dev;

    hid0_dev = device_get_binding("HID_0");
    if (hid0_dev == NULL) {
        LOG_ERR("Cannot get USB HID Device");
        return;
    }
#if CONFIG_USB_HID_DEVICE_COUNT > 1
    hid1_dev = device_get_binding("HID_1");
    if (hid1_dev == NULL) {
        LOG_ERR("Cannot get USB HID Device");
        return;
    }
#endif
#if CONFIG_USB_HID_DEVICE_COUNT > 2
    hid2_dev = device_get_binding("HID_2");
    if (hid2_dev == NULL) {
        LOG_ERR("Cannot get USB HID Device");
        return;
    }
#endif
#if CONFIG_USB_HID_DEVICE_COUNT > 3
    hid3_dev = device_get_binding("HID_3");
    if (hid3_dev == NULL) {
        LOG_ERR("Cannot get USB HID Device");
        return;
    }
#endif

    usb_hid_register_device(hid0_dev,
                            hid0_report_desc, sizeof(hid0_report_desc),
                            &ops0);
    usb_hid_init(hid0_dev);

#if CONFIG_USB_HID_DEVICE_COUNT > 1
    usb_hid_register_device(hid1_dev,
                            hid1_report_desc, sizeof(hid1_report_desc),
                            &ops1);
    usb_hid_set_protocol(hid1_dev, 0, 1);
    usb_hid_init(hid1_dev);
#endif //CONFIG_USB_HID_DEVICE_COUNT 1
#if CONFIG_USB_HID_DEVICE_COUNT > 2
    usb_hid_register_device(hid2_dev,
                            hid2_report_desc, sizeof(hid2_report_desc),
                            &ops2);
    usb_hid_set_protocol(hid2_dev, 1, 1);
    usb_hid_init(hid2_dev);
#endif //CONFIG_USB_HID_DEVICE_COUNT 2
#if CONFIG_USB_HID_DEVICE_COUNT > 3
    usb_hid_register_device(hid3_dev,
                            hid3_report_desc, sizeof(hid3_report_desc),
                            &ops3);
    usb_hid_set_protocol(hid3_dev, 0, 2);
    usb_hid_init(hid3_dev);
#endif //CONFIG_USB_HID_DEVICE_COUNT 3


    ret = usb_enable(status_cb);
    if (ret != 0) {
        LOG_ERR("Failed to enable USB");
        return;
    }

    while (true) {
        report[MOUSE_BTN_REPORT_POS] = status[MOUSE_BTN_REPORT_POS];
        report[MOUSE_X_REPORT_POS] = status[MOUSE_X_REPORT_POS];
        status[MOUSE_X_REPORT_POS] = 0U;
        report[MOUSE_Y_REPORT_POS] = status[MOUSE_Y_REPORT_POS];
        status[MOUSE_Y_REPORT_POS] = 0U;
        ret = hid_int_ep_write(hid0_dev, report, sizeof(report), NULL);
#if CONFIG_USB_HID_DEVICE_COUNT > 1
        ret = hid_int_ep_write(hid1_dev, report, sizeof(report), NULL);
#endif //CONFIG_USB_HID_DEVICE_COUNT 1
#if CONFIG_USB_HID_DEVICE_COUNT > 2
        ret = hid_int_ep_write(hid2_dev, report, sizeof(report), NULL);
#endif //CONFIG_USB_HID_DEVICE_COUNT 2
#if CONFIG_USB_HID_DEVICE_COUNT > 3
        ret = hid_int_ep_write(hid3_dev, report, sizeof(report), NULL);
#endif //CONFIG_USB_HID_DEVICE_COUNT 3

        k_sem_take(&sem, K_FOREVER);

        report[MOUSE_BTN_REPORT_POS] = status[MOUSE_BTN_REPORT_POS];
        report[MOUSE_X_REPORT_POS] = status[MOUSE_X_REPORT_POS];
        status[MOUSE_X_REPORT_POS] = 0U;
        report[MOUSE_Y_REPORT_POS] = status[MOUSE_Y_REPORT_POS];
        status[MOUSE_Y_REPORT_POS] = 0U;
        ret = hid_int_ep_write(hid0_dev, report, sizeof(report), NULL);
        if (ret) {
            LOG_ERR("HID write error, %d", ret);
        }

    }
}


