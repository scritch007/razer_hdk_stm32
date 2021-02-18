//
// Created by benjamin on 09/02/2021.
//

#ifndef HID_MOUSE_RAZER_H
#define HID_MOUSE_RAZER_H

#include <drivers/led_strip.h>

#include "strip.h"

union transaction_id_union {
    unsigned char id;
    struct transaction_parts {
        unsigned char device: 3;
        unsigned char id: 5;
    } parts;
};

union command_id_union {
    unsigned char id;
    struct command_id_parts {
        unsigned char direction: 1;
        unsigned char id: 7;
    } parts;
};

/* Status:
 * 0x00 New Command
 * 0x01 Command Busy
 * 0x02 Command Successful
 * 0x03 Command Failure
 * 0x04 Command No Response / Command Timeout
 * 0x05 Command Not Support
 *
 * Transaction ID used to group request-response, device useful when multiple devices are on one usb
 * Remaining Packets is the number of remaining packets in the sequence
 * Protocol Type is always 0x00
 * Data Size is the size of payload, cannot be greater than 80. 90 = header (8B) + data + CRC (1B) + Reserved (1B)
 * Command Class is the type of command being issued
 * Command ID is the type of command being send. Direction 0 is Host->Device, Direction 1 is Device->Host. AKA Get LED 0x80, Set LED 0x00
 *
 * */

struct razer_report {
    unsigned char status;
    union transaction_id_union transaction_id; /* */
    unsigned short remaining_packets; /* Big Endian */
    unsigned char protocol_type; /*0x0*/
    unsigned char data_size;
    unsigned char command_class;
    union command_id_union command_id;
    unsigned char arguments[80];
    unsigned char crc;/*xor'ed bytes of report*/
    unsigned char reserved; /*0x0*/
};
#define HDK_LED_STRIP_LENGTH 16


#include "effects/breath.h"

union effect_union {
    breath_effect breath;
};

#define BREATH 1
#define CUSTOM 2

struct razer_context {
    bool serial_requested;
    struct razer_report current_report;
    uint8_t state;
    char brightness[18];
    struct led_rgb row[HDK_LED_STRIP_LENGTH*4];
    union effect_union effect;
    uint8_t current_effect;
};

struct rgb {
    uint8_t r, g, b;
};

struct row_req {
    char skip[2];
    char row;
    char first, last;
    struct rgb leds[STRIP_NUM_PIXELS];
};

extern struct razer_context gContext;

#define STATE_INIT 0
#define STATE_START 1
#define STATE_RUNNING 2
#define STATE_START2 3

bool get_report_hid(int reportID, char **report, int *len);

#define RAZER_REPORT_LENGTH 90
extern char serial[RAZER_REPORT_LENGTH];

#endif //HID_MOUSE_RAZER_H
