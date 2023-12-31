//
// Created by benjamin on 11/02/2021.
//

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>

#include <usb/usb_device.h>
#include <usb/class/usb_hid.h>

#include "razer.h"
#include "set_report.h"


#if CONFIG_USB_RAZER_TYPE == 1

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

const char serial[90] = {0x02, 0x08, 0x00, 0x00, 0x00, 0x16, 0x00, 0x82, 0x50, 0x4d, 0x32, 0x30, 0x30, 0x31, 0x48, 0x31,
                         0x34,
                         0x34, 0x33, 0x38,
                         0x36, 0x38, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0xc2, 0x00,};

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
#if CONFIG_USB_HID_DEVICE_COUNT > 3
            case 3:
                *report = hid3_report_desc;
                *len = sizeof(hid3_report_desc);
                break;
#endif
        default:
            return false;
    }
    return true;
}


#endif