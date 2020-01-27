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

#include "hm_sdk_config.h"
#include <string.h>
#include <stdio.h>

//-------------------------------------------------//
uint8_t m_device_cert_issuer_pub[64];
hm_car_t hm_car;
uint8_t m_link_device_certificate[153];

#define DEV_CERT_SERIAL_POS 16
#define DEV_CERT_PUB_KEY_POS 25

#define PUB_KEY_SIZE 64
#define PRV_KEY_SIZE 32
#define DEV_CERT_SERIAL_SIZE 9
#define DEV_CERT_PUB_KEY_SIZE PUB_KEY_SIZE


void hm_sdk_config_get_device_cert_issuer_pub(uint8_t *pub)
{
	printf("******** %s, %s()\n", __FILE__, __FUNCTION__);
    memcpy(pub,m_device_cert_issuer_pub, DEV_CERT_PUB_KEY_SIZE);
}

void hm_sdk_config_get_access_cert_issuer_pub(uint8_t *pub)
{
//	printf("********* %s, %s()\n", __FILE__, __FUNCTION__);
    memset(pub,0x00,PUB_KEY_SIZE);
}

void hm_sdk_config_get_issuer(uint8_t *issuer)
{
  // printf("******** %s, %s()\n", __FILE__, __FUNCTION__);
    memset(issuer,0x00,4);
}

hm_car_t hm_sdk_config_get_serial_public_private()
{
#ifdef PYC_DEBUG
//	printf("********** %s, %s()\n", __FILE__, __FUNCTION__);
//	debug_dump(&(m_link_device_certificate[DEV_CERT_PUB_KEY_POS]), DEV_CERT_PUB_KEY_SIZE);
//	debug_dump(&(m_link_device_certificate[DEV_CERT_SERIAL_POS]), DEV_CERT_SERIAL_SIZE);
#endif

	// Copy the Pub key from Device certificate
	memcpy(hm_car.public_key, &(m_link_device_certificate[DEV_CERT_PUB_KEY_POS]), DEV_CERT_PUB_KEY_SIZE);

	// Copy the Serial from Device certificate
	memcpy(hm_car.serial, &(m_link_device_certificate[DEV_CERT_SERIAL_POS]), DEV_CERT_SERIAL_SIZE);

	return hm_car;
}

void hm_sdk_config_get_device_certificate(uint8_t *device){
#ifdef PYC_DEBUG
//	printf("********** Enter %s, %s()\n", __FILE__, __FUNCTION__);
//	debug_dump(m_link_device_certificate, 153);
#endif

    memcpy(device,m_link_device_certificate,153);
}

char** hm_sdk_config_get_argv()
{
    return NULL;
}

//===============================================================//
//-------------- Set Api()s will be called from Python ----------//
//===============================================================//

void hm_sdk_config_set_issuer_pub(uint8_t *pub)
{
//	printf(" %s, %s()\n", __FILE__, __FUNCTION__);

    memcpy(m_device_cert_issuer_pub, pub, DEV_CERT_PUB_KEY_SIZE);

#ifdef PYC_DEBUG
	debug_dump(m_device_cert_issuer_pub, DEV_CERT_PUB_KEY_SIZE);
#endif
}

void hm_sdk_config_set_dev_prv(uint8_t *prv)
{
//	printf(" %s, %s()\n", __FILE__, __FUNCTION__);
	memcpy(hm_car.private_key, prv, PRV_KEY_SIZE);

#ifdef PYC_DEBUG
	debug_dump(hm_car.private_key, PRV_KEY_SIZE);
#endif
	
}

void hm_sdk_config_set_device_certificate(uint8_t *device)
{
//printf(" %s, %s()\n", __FILE__, __FUNCTION__);
    memcpy(m_link_device_certificate, device, 153);

#ifdef PYC_DEBUG
	debug_dump(m_link_device_certificate, 153);
#endif
}

void debug_dump(char *ptr, int len)
{
	int i = 0;

	printf("SDK Config: Dump: ");

	for(i=0; i<len; i++)
	{
		printf(" 0x%x, ", ptr[i]);
	}

	printf("\n");
}

//==============================================================//

uint8_t hm_sdk_config_is_usb()
{
#ifdef HM_HCI_TYPE_USB
    return 1;
#else
    return 0;
#endif
}

char* hm_sdk_config_get_uart_device()
{
#ifdef HM_HCI_UART_DEVICE
    return HM_HCI_UART_DEVICE;
#else
     return NULL;
#endif
}
