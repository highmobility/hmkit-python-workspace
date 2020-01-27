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

#include "hmkit_core_persistence_hal.h"
#include <string.h>
#include "hm_persistence_sqlite.h"
#include "hmkit_core_log.h"
#include "hm_sdk_config.h"

uint32_t hmkit_core_persistence_hal_get_serial(uint64_t appContxtId, uint8_t *serial){
  BTUNUSED(appContxtId);
  uint8_t cert[153];
//printf("Enter %s() %d \n", __FUNCTION__, __LINE__);
  hm_sdk_config_get_device_certificate(cert);
  memcpy(serial,cert+16,9);
  return 0;
}

uint32_t hmkit_core_persistence_hal_get_local_public_key(uint64_t appContxtId, uint8_t *public){
//printf("Enter %s() %d \n", __FUNCTION__, __LINE__);
  BTUNUSED(appContxtId);
  hm_car_t pubser = hm_sdk_config_get_serial_public_private();
  memcpy(public,pubser.public_key,64);
  return 0;
}

uint32_t hmkit_core_persistence_hal_get_local_private_key(uint64_t appContxtId, uint8_t *public){
//printf("Enter %s() %d Not Implemented \n", __FUNCTION__, __LINE__);
  BTUNUSED(appContxtId);
  BTUNUSED(public);
  return 0;
}

uint32_t hmkit_core_persistence_hal_get_device_certificate(uint64_t appContxtId, uint8_t *cert){
  BTUNUSED(appContxtId);
//printf("Enter %s() %d \n", __FUNCTION__, __LINE__);
  hm_sdk_config_get_device_certificate(cert);
  return 0;
}

uint32_t hmkit_core_persistence_hal_get_ca_public_key(uint64_t appContxtId, uint8_t *public){
  BTUNUSED(appContxtId);
//printf("Enter %s() %d \n", __FUNCTION__, __LINE__);
  hm_sdk_config_get_device_cert_issuer_pub(public);
  return 0;
}

uint32_t hmkit_core_persistence_hal_get_oem_ca_public_key(uint64_t appContxtId, uint8_t *public){
  BTUNUSED(appContxtId);
//printf("Enter %s() %d \n", __FUNCTION__, __LINE__);
  hm_sdk_config_get_access_cert_issuer_pub(public);
  return 0;
}

uint32_t hmkit_core_persistence_hal_add_access_certificate(uint64_t appContxtId, uint8_t *serial,
 uint8_t *cert, uint16_t size)
{
    BTUNUSED(appContxtId);
//printf("Enter %s() %d \n", __FUNCTION__, __LINE__);
	hm_persistence_sqlite_add_access_cert(serial, cert, size);
	return 0;
}

uint32_t hmkit_core_persistence_hal_get_access_certificate(uint64_t appContxtId, uint8_t *serial,uint8_t *cert, uint16_t *size)
{
	BTUNUSED(appContxtId);
	uint8_t size_tmp;

//printf("Enter %s() %d \n", __FUNCTION__, __LINE__);
	hm_persistence_sqlite_get_access_cert(serial, cert, &size_tmp);
	*size = size_tmp;
 
	return 0;
}

uint32_t hmkit_core_persistence_hal_get_access_certificate_by_index(uint64_t appContxtId, uint8_t index, uint8_t *cert, uint16_t *size)
{
	BTUNUSED(appContxtId);
	BTUNUSED(index);
	BTUNUSED(cert);
	BTUNUSED(size);

	return 0;
}

uint32_t hmkit_core_persistence_hal_get_access_certificate_count(uint64_t appContxtId, uint8_t *count){
  BTUNUSED(appContxtId);
  BTUNUSED(count);
  return 0;
}

//uint32_t hmkit_core_persistence_hal_remove_access_certificate(uint64_t appContxtId, uint8_t *serial){
uint32_t hmkit_core_persistence_hal_remove_access_certificate(uint64_t appContxtId, hmkit_core_certificate_t *certificate){

  BTUNUSED(appContxtId);
  BTUNUSED(certificate);
//printf("Enter %s() %d \n", __FUNCTION__, __LINE__);
  // TODO: hm_certificate_t to serial
  //hm_persistence_sqlite_remove_access_cert(serial);

  return 0;
}

uint32_t hmkit_core_persistence_hal_add_stored_certificate(uint64_t appContxtId, uint8_t *cert, uint16_t size){
//printf("Enter %s() %d Not Implemented \n", __FUNCTION__, __LINE__);
  BTUNUSED(appContxtId);
  BTUNUSED(cert);
  BTUNUSED(size);
  return 0;
}

uint32_t hmkit_core_persistence_hal_get_stored_certificate(uint64_t appContxtId, uint8_t *serial, uint8_t *cert, uint16_t *size){
//printf("Enter %s() %d Not Implemented\n", __FUNCTION__, __LINE__);
  BTUNUSED(appContxtId);
  BTUNUSED(serial);
  BTUNUSED(cert);
  BTUNUSED(size);
  return 0;
}

uint32_t hmkit_core_persistence_hal_erase_access_certificate(uint64_t appContxtId, hmkit_core_certificate_t *certificate)
{
  BTUNUSED(appContxtId);
  BTUNUSED(certificate);
}

uint32_t hmkit_core_persistence_hal_erase_stored_certificate(uint64_t appContxtId, uint8_t *serial){
//printf("Enter %s() %d \n", __FUNCTION__, __LINE__);
  BTUNUSED(appContxtId);
  BTUNUSED(serial);
  return 0;
}

uint32_t hmkit_core_persistence_hal_get_appid_for_issuer_count(uint64_t appContxtId, uint8_t *issuer, uint8_t *count){

  BTUNUSED(appContxtId);
  uint8_t issuer_id[4];
  hm_sdk_config_get_issuer(issuer_id);

  if(memcmp(issuer,issuer_id,4) == 0){
    *count = 1;
  }else{
    *count = 0;
  }

  return 0;
}

uint32_t hmkit_core_persistence_hal_get_appid_for_issuer(uint64_t appContxtId, uint8_t *issuer, uint8_t index, uint8_t *appid){
  BTUNUSED(appContxtId);
  BTUNUSED(index);
  BTUNUSED(issuer);
  uint8_t app_id[12] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  memcpy(appid,app_id,12);
  return 0;
}

uint32_t hmkit_core_persistence_hel_set_command_count(uint64_t appContxtId, uint8_t *serial, uint8_t command, uint8_t count){
//printf("Enter %s() %d \n", __FUNCTION__, __LINE__);
  BTUNUSED(appContxtId);
  BTUNUSED(serial);
  BTUNUSED(command);
  BTUNUSED(count);
  return 0;
}

uint32_t hmkit_core_persistence_hel_get_command_count(uint64_t appContxtId, uint8_t *serial, uint8_t command, uint8_t *count){
//printf("Enter %s() %d \n", __FUNCTION__, __LINE__);
  BTUNUSED(appContxtId);
  BTUNUSED(serial);
  BTUNUSED(command);
  BTUNUSED(count);
  return 0;
}

uint32_t hmkit_core_persistence_hel_remove_all_command_counts(uint64_t appContxtId, uint8_t *serial){
  BTUNUSED(appContxtId);
  BTUNUSED(serial);
  return 0;
}
