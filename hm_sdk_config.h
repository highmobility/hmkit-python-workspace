/*
The MIT License

Copyright (c) 2014- High-Mobility GmbH (https://high-mobility.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef HM_SDK_CONFIG_H
#define HM_SDK_CONFIG_H

#include <stdint.h>

typedef struct {
    uint8_t serial[9];
    uint8_t public_key[64];
    uint8_t private_key[32];
} hm_car_t;

void hm_sdk_config_get_device_cert_issuer_pub(uint8_t *pub);
void hm_sdk_config_get_access_cert_issuer_pub(uint8_t *pub);
void hm_sdk_config_get_issuer(uint8_t *issuer);
hm_car_t hm_sdk_config_get_serial_public_private(void);
void hm_sdk_config_get_device_certificate(uint8_t *device);
char** hm_sdk_config_get_argv(void);
uint8_t hm_sdk_config_is_usb(void);
char* hm_sdk_config_get_uart_device(void);
void hm_sdk_config_set_issuer_pub(uint8_t *pub);
void hm_sdk_config_set_dev_prv(uint8_t *prv);
void hm_sdk_config_set_device_certificate(uint8_t *device);
#endif //HM_SDK_CONFIG_H
