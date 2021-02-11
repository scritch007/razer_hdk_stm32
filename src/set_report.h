//
// Created by benjamin on 10/02/2021.
//

#ifndef HID_MOUSE_GET_REPORT_H
#define HID_MOUSE_GET_REPORT_H

int parse_08_requests(int id, const struct device *dev_data,
                      struct usb_setup_packet *setup, int32_t *len,
                      uint8_t **data);
#endif //HID_MOUSE_GET_REPORT_H
