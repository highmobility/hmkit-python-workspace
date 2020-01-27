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


void py_hm_api_callback_command_incoming(hmkit_core_device_t *device, uint8_t *data, uint16_t length);

void py_hm_api_callback_command_response(hmkit_core_device_t *device, uint8_t *data, uint16_t length);

void py_hm_api_callback_entered_proximity(hmkit_core_device_t *device);

void py_hm_api_callback_exited_proximity(hmkit_core_device_t *device);

void py_hm_api_callback_telematics_command_incoming(hmkit_core_device_t *device, uint8_t id, uint16_t length, uint8_t *data);

void py_hm_sdk_config_set_issuer_pub(uint8_t *pub);

void py_hm_sdk_config_set_dev_prv(uint8_t *prv);

void py_hm_sdk_config_set_device_certificate(uint8_t *device);

int py_store_accesscertificate(uint8_t *certf, uint8_t len, uint8_t *ser_num);


typedef enum
{
	eCB_KEYWRD_CMDINCOMING=0x0,
	eCB_KEYWRD_CMDRESP,
	eCB_KEYWRD_ENTPROX,
	eCB_KEYWRD_EXTPROX,
	eCB_KEYWRD_TELEINCM,

	eCB_KEYWRD_MAX
}cb_keywrds_t;

