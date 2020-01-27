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

#include "hmkit_core_api_callback.h"
#include "hmkit_core_cert.h"
#include <string.h>
#include <gio/gio.h>
#include <stdio.h>

#ifdef ENABLE_AUTOAPI
#include "hm_auto_api.h"
#endif

#ifdef ENABLE_PARSER
#include "hm_auto_parse_constrct.h"
#endif

#include "pyc_bindings.h"


//----------------------------
#ifdef ENABLE_PARSER

uint16_t len = 0, out_len =0;
#define TEST_BUFF_SIZE 1024
uint8_t testbytes[TEST_BUFF_SIZE];
hm_parsed_msg_t Parsed_msg;

#endif

static uint8_t isConnected = 0;
static uint8_t connectedSerial[9];

//--------------------------

void hmkit_core_api_callback_init(void)
{
    //Start console input
    //g_thread_new ("InputThread", inputThread, NULL);
}

void hmkit_core_api_callback_clock(void)
{
//	printf("*** Enter %s()\n", __FUNCTION__);

}

void hmkit_core_api_callback_ping(void)
{
	printf("*** Enter %s()\n", __FUNCTION__);

}

void hmkit_core_api_callback_entered_proximity(uint64_t appContxtId, hmkit_core_device_t *device)
{
    BTUNUSED(appContxtId);
	///printf("*** Enter %s()\n", __FUNCTION__);

    if(device->is_authorised == 1){
        memcpy(connectedSerial,device->serial_number,9);
        isConnected = 1;
    }

   py_hm_api_callback_entered_proximity(device);
}

void hmkit_core_api_callback_proximity_measured(uint64_t appContxtId, hmkit_core_device_t *device, uint8_t receiver_count, hmkit_core_receiver_t *receivers)
{
	printf("*** Enter %s()\n", __FUNCTION__);

    BTUNUSED(appContxtId);
    BTUNUSED(device);
    BTUNUSED(receiver_count);
    BTUNUSED(receivers);
}

void hmkit_core_api_callback_exited_proximity(uint64_t appContxtId, hmkit_core_device_t *device)
{
    BTUNUSED(appContxtId);
	///printf("*** Enter %s()\n", __FUNCTION__);
    isConnected = 0;

	py_hm_api_callback_exited_proximity(device);
}

void hmkit_core_api_callback_command_incoming(uint64_t appContxtId, hmkit_core_device_t *device, uint8_t content_type, uint8_t *data, uint32_t length, uint8_t *respID, uint16_t respID_size, uint8_t version)
{
    BTUNUSED(appContxtId);
    BTUNUSED(content_type);
    BTUNUSED(respID);
    BTUNUSED(respID_size);
    BTUNUSED(version);
	///printf("*** Enter %s()\n", __FUNCTION__);

	py_hm_api_callback_command_incoming(device, data, length);
}

void hmkit_core_api_callback_command_response(uint64_t appContxtId, hmkit_core_device_t *device, uint8_t content_type, uint8_t *data, uint32_t length, uint8_t *respID, uint16_t respID_size, uint8_t version){
    BTUNUSED(appContxtId);
    BTUNUSED(respID);
    BTUNUSED(respID_size);
    BTUNUSED(version);
    BTUNUSED(content_type);
	///printf("*** Enter %s()\n", __FUNCTION__);

	py_hm_api_callback_command_response(device, data, length);
}

void hmkit_core_api_callback_command_response_error(uint64_t appContxtId, hmkit_core_device_t *device, uint8_t errortype)
{
    printf("*** Enter %s()\n", __FUNCTION__);

    BTUNUSED(device);
    BTUNUSED(errortype);
    BTUNUSED(appContxtId);
}

uint32_t hmkit_core_api_callback_get_device_certificate_failed(uint64_t appContxtId, hmkit_core_device_t *device, uint8_t *nonce)
{
    printf("*** Enter %s()\n", __FUNCTION__);

    BTUNUSED(appContxtId);
    BTUNUSED(device);
    BTUNUSED(nonce);
    return 0;
}

void hmkit_core_api_callback_access_certificate_registered(uint64_t appContxtId, hmkit_core_device_t *device, uint8_t *public_key, uint8_t error)
{
	printf("*** Enter %s()\n", __FUNCTION__);

    BTUNUSED(appContxtId);
    BTUNUSED(device);
    BTUNUSED(public_key);
    BTUNUSED(error);
}

uint32_t hmkit_core_api_callback_pairing_requested(uint64_t appContxtId, hmkit_core_device_t *device){
	printf("*** Enter %s()\n", __FUNCTION__);
    BTUNUSED(appContxtId);
    BTUNUSED(device);
    return 0;
}

void hmkit_core_api_callback_telematics_command_incoming(uint64_t appContxtId, hmkit_core_device_t *device, uint8_t id, uint8_t content_type, uint32_t length, uint8_t *data, uint8_t *respID, uint16_t respID_size, uint8_t version){

    BTUNUSED(appContxtId);
    BTUNUSED(content_type);
    BTUNUSED(respID);
    BTUNUSED(respID_size);
    BTUNUSED(version);
	printf("*** Enter %s()\n", __FUNCTION__);

	py_hm_api_callback_telematics_command_incoming(device, id, length, data);
}

uint32_t hmkit_core_api_callback_get_current_date_time(uint8_t *day, uint8_t *month, uint8_t *year, uint8_t *minute, uint8_t *hour){
	printf("*** Enter %s()\n", __FUNCTION__);

    BTUNUSED(day);
    BTUNUSED(month);
    BTUNUSED(year);
    BTUNUSED(minute);
    BTUNUSED(hour);

    return 0;
}

void hmkit_core_api_callback_revoke_response(uint64_t appContxtId, hmkit_core_device_t *device, uint8_t *data, uint16_t length, uint8_t status)
{
    BTUNUSED(appContxtId);
    BTUNUSED(device);
    BTUNUSED(data);
    BTUNUSED(length);
    BTUNUSED(status);

    printf("*** Enter %s()\n", __FUNCTION__);
    // TODO: Expose the callback to Python

}

void hmkit_core_api_callback_revoke_incoming(uint64_t appContxtId, hmkit_core_device_t *device, uint8_t *data, uint16_t *size)
{
    BTUNUSED(appContxtId);
    BTUNUSED(device);
    BTUNUSED(data);
    BTUNUSED(size);
    printf("*** Enter %s()\n", __FUNCTION__);
    // TODO: Expose the callback to Python

}

void hmkit_core_api_callback_error_command_incoming(uint64_t appContxtId, hmkit_core_device_t *device, uint8_t command, uint8_t errorType)
{
    BTUNUSED(appContxtId);
    BTUNUSED(device);
    BTUNUSED(command);
    BTUNUSED(errorType);
    printf("*** Enter %s()\n", __FUNCTION__);
    // TODO: Expose the callback to Python

}
