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
#include <drivers/hwinfo.h>

#include "razer.h"
#include "set_report.h"
#include "strip.h"
#include "effects/spectrum.h"

#define LOG_LEVEL LOG_LEVEL_DBG
LOG_MODULE_REGISTER(main);

static volatile uint8_t status[4];
static K_SEM_DEFINE(sem, 0, 1);    /* starts off "not available" */
static struct gpio_callback callback[4];

#define DELAY_TIME K_MSEC(10)

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

int get_report_cb(int ID, const struct device *dev_data,
                  struct usb_setup_packet *setup, int32_t *len,
                  uint8_t **data) {
    LOG_INF("Get report callback %d", ID);

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

    //LOG_HEXDUMP_INF(*data, size, "Set Report");
    if (*len == 90) {
        switch ((*data)[1]) {
            case 0x08:
            case 0x1f:
            case 0x0c:
            case 0x3F:
            case 0xFF: // Used by openrazer
                return parse_08_requests(id, dev_data, setup, len, data);
                gContext.state = STATE_START;
                break;
            default:
                LOG_ERR("Not supported %02X", (*data)[1]);
                return -ENOTSUP;
        }
    }

    LOG_ERR("Unknown set report %d", *len);

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
            return 0;
        case 0x01:
            return 0;
        case 0x06:
            return 0;
        case 0x03:
            return 0;
        case 0x86:
            return 0;
        default:
            return -ENOTSUP;
    }

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

#if CONFIG_USB_HID_DEVICE_COUNT > 1
static struct hid_ops ops1 = {
        .get_report = get_report_cb1,
        .get_idle = debug_cb,
        .get_protocol = get_protocol,
        .set_report = set_report1,
        .set_idle = set_idle_cb,
};
#endif

#if CONFIG_USB_HID_DEVICE_COUNT > 2

static struct hid_ops ops2 = {
        .get_report = get_report_cb2,
        .get_idle = debug_cb,
        .get_protocol = get_protocol,
        .set_report = set_report2,
        .set_idle = set_idle_cb,
};
#endif
#if CONFIG_USB_HID_DEVICE_COUNT > 3
static struct hid_ops ops3 = {
        .get_report = get_report_cb3,
        .get_idle = debug_cb,
        .get_protocol = get_protocol,
        .set_report = set_report3,
        .set_idle = set_idle_cb,
};
#endif

void generate_serial() {
    hwinfo_get_device_id(&serial[10], 78);
    for (int i = 10; i < 88; i++) {
        if ((serial[i] >= 0x30) && (serial[i] < 0x3A)) {

        } else if ((serial[i] >= 0x41) && (serial[i] < 0x5B)) {
        } else if ((serial[i] >= 0x61) && (serial[i] < 0x6B)) {
        } else if (serial[i] == 0) {
            break;
        } else {
            serial[i] = 0x30 + serial[i] % (10);
        }
    }
    LOG_HEXDUMP_INF(&serial[0], 90, "Serial");
}

void main(void) {
    int ret;
    uint8_t report[4] = {0x00};
    struct led_rgb row[HDK_LED_STRIP_LENGTH * 4];

    generate_serial();
    gContext.current_effect = SPECTRUM;
    gContext.brightness[0] = 200;


    const struct device *hid0_dev, *strip
#if CONFIG_USB_HID_DEVICE_COUNT > 1
    ,*hid1_dev
#endif
#if CONFIG_USB_HID_DEVICE_COUNT > 2
    ,*hid2_dev
#endif
#if CONFIG_USB_HID_DEVICE_COUNT > 3
    ,* hid3_dev
#endif
    ;

    strip = device_get_binding(STRIP_LABEL);
    if (strip) {
        LOG_INF("Found LED strip device %s", STRIP_LABEL);
    } else {
        LOG_ERR("LED strip device %s not found", STRIP_LABEL);
        return;
    }


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
    char *hid_report;
    int len = 0;
    if (!get_report_hid(0, &hid_report, &len)) {
        LOG_ERR("Cannot get report 0");
        return;
    }
    usb_hid_register_device(hid0_dev,
                            hid_report, len,
                            &ops0);
    usb_hid_init(hid0_dev);

#if CONFIG_USB_HID_DEVICE_COUNT > 1
    if (!get_report_hid(1, &hid_report, &len) )
    {
        LOG_ERR("Cannot get report 0");
        return;
    }
    usb_hid_register_device(hid1_dev,
                            hid_report, len,
                            &ops1);
    usb_hid_set_protocol(hid1_dev, 0, 1);
    usb_hid_init(hid1_dev);
#endif //CONFIG_USB_HID_DEVICE_COUNT 1
#if CONFIG_USB_HID_DEVICE_COUNT > 2
    if (!get_report_hid(2, &hid_report, &len) )
    {
        LOG_ERR("Cannot get report 0");
        return;
    }
    usb_hid_register_device(hid2_dev,
                            hid_report, len,
                            &ops2);
#if CONFIG_USB_RAZER_TYPE==1
    usb_hid_set_protocol(hid2_dev, 1, 1);
#endif
#if CONFIG_USB_RAZER_TYPE==0
    usb_hid_set_protocol(hid2_dev, 0, 2);
#endif
    usb_hid_init(hid2_dev);
#endif //CONFIG_USB_HID_DEVICE_COUNT 2
#if CONFIG_USB_HID_DEVICE_COUNT > 3
    if (!get_report_hid(3, &hid_report, &len) )
    {
        LOG_ERR("Cannot get report 0");
        return;
    }
    usb_hid_register_device(hid3_dev,
                            hid_report, len,
                            &ops3);
    usb_hid_set_protocol(hid3_dev, 0, 2);
    usb_hid_init(hid3_dev);
#endif //CONFIG_USB_HID_DEVICE_COUNT 3


    ret = usb_enable(status_cb);
    if (ret != 0) {
        LOG_ERR("Failed to enable USB");
        return;
    }

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


    while (true) {
        k_sleep(DELAY_TIME);
        //LOG_HEXDUMP_INF(&(gContext.row[0][0]), STRIP_NUM_PIXELS*sizeof(struct led_rgb), "Applying");


        int rc;
        switch (gContext.current_effect) {
            case WAVE:
                wave(&gContext.effect.wave, &row[0], STRIP_NUM_PIXELS);
                break;
            case BREATH:
                breath(&gContext.effect.breath, &row[0], STRIP_NUM_PIXELS);
                break;
            case SPECTRUM:
                spectrum(&gContext.effect.spectrum, &row[0], STRIP_NUM_PIXELS);
                break;
            case NONE:
            case STATIC:
            case CUSTOM:
                for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
                    row[i].r = gContext.row[i].r;
                    row[i].g = gContext.row[i].g;
                    row[i].b = gContext.row[i].b;
                }
                break;


            default:
                LOG_ERR("Unknown effect");
        }
        for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
            row[i].r = (row[i].r * gContext.brightness[0]) / 255;
            row[i].g = (row[i].g * gContext.brightness[0]) / 255;
            row[i].b = (row[i].b * gContext.brightness[0]) / 255;
        }
        rc = led_strip_update_rgb(strip, &row[0], STRIP_NUM_PIXELS);

        if (rc) {
            LOG_ERR("couldn't update strip: %d", rc);
        }
    }
}


