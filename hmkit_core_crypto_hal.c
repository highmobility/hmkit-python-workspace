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

#include "hmkit_core_crypto_hal.h"
#include <string.h>
#include "hmkit_core_persistence_hal.h"
#include "hmkit_core_cert.h"
#include "Crypto.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "hm_sdk_config.h"

#define CERT_SIZE 256 // variable size, need to modify

uint32_t hmkit_core_crypto_hal_aes_ecb_block_encrypt(uint64_t appContxtId, uint8_t *key, uint8_t *cleartext, uint8_t *cipertext){
  BTUNUSED(appContxtId);
  return hm_crypto_openssl_aes_iv(key, cleartext, cipertext);
}

uint32_t hmkit_core_crypto_hal_ecc_get_ecdh(uint64_t appContxtId, uint8_t *serial, uint8_t *ecdh){
	uint8_t cert[CERT_SIZE];
	uint16_t size = 0; 
	hmkit_core_certificate_t certificate;
    //printf("Enter CORE2 DEBUG %d %s ,%s()\n", __LINE__, __FILE__, __FUNCTION__);

	if(hmkit_core_persistence_hal_get_access_certificate(appContxtId, serial, cert, &size) == 1)
	{
		return 1;
	}

	hmkit_core_cert_get_as_struct(cert, &certificate);

	hm_car_t pubser = hm_sdk_config_get_serial_public_private();
	return hm_crypto_openssl_dh(pubser.private_key, certificate.public_key, ecdh);
}

uint32_t hmkit_core_crypto_hal_ecc_add_signature(uint64_t appContxtId, uint8_t *data, uint8_t size, uint8_t *signature){

    BTUNUSED(appContxtId);
    hm_car_t pubser = hm_sdk_config_get_serial_public_private();

    //printf(" CORE2 DEBUG: data-x %x, sign %x \n", data, signature);
    //printf(" TEST: CORE2 DEBUG: data-x %x, data size %d, (uint8_t) size %d,  sign %x \n", data_test, size, size, signature);

     //return hm_crypto_openssl_signature(data, (uint8_t)size, pubser.private_key, signature);
     return hm_crypto_openssl_signature(data, size, pubser.private_key, signature);
}

uint32_t hmkit_core_crypto_hal_ecc_validate_signature(uint64_t appContxtId, uint8_t *data, uint8_t size, uint8_t *signature, uint8_t *serial){
	uint8_t cert[CERT_SIZE];
	uint16_t csize = 0;
	hmkit_core_certificate_t certificate;

	if(hmkit_core_persistence_hal_get_access_certificate(appContxtId, serial, cert, &csize) == 1){
	return 1;
	}

	hmkit_core_cert_get_as_struct(cert, &certificate);

	return hm_crypto_openssl_verify(data, size, certificate.public_key, signature);
}

uint32_t hmkit_core_crypto_hal_ecc_validate_all_signatures(uint64_t appContxtId, uint8_t *data, uint8_t size, uint8_t *signature){
  BTUNUSED(appContxtId);
  BTUNUSED(data);
  BTUNUSED(size);
  BTUNUSED(signature);
  //TODO if needed
  return 0;
}

uint32_t hmkit_core_crypto_hal_ecc_validate_ca_signature(uint64_t appContxtId, uint8_t *data, uint8_t size, uint8_t *signature){
  uint8_t ca_pub[64];
  hmkit_core_persistence_hal_get_ca_public_key(appContxtId, ca_pub);
  return hm_crypto_openssl_verify(data, size, ca_pub, signature);
}

uint32_t hmkit_core_crypto_hal_ecc_validate_oem_ca_signature(uint64_t appContxtId, uint8_t *data, uint8_t size, uint8_t *signature){
  uint8_t ca_pub[64];
  hmkit_core_persistence_hal_get_oem_ca_public_key(appContxtId, ca_pub);
  return hm_crypto_openssl_verify(data, size, ca_pub, signature);
}


uint32_t hmkit_core_crypto_hal_hmac(uint64_t appContxtId, uint8_t *key, uint8_t *data, uint16_t size, uint8_t *hmac){
  BTUNUSED(appContxtId);
  return hm_crypto_openssl_hmac(data, size, key, hmac);
 }


uint32_t hmkit_core_crypto_hal_generate_nonce(uint64_t appContxtId, uint8_t *nonce){
    //printf("Enter CORE2 DEBUG %d %s ,%s()\n", __LINE__, __FILE__, __FUNCTION__);
  BTUNUSED(appContxtId);
  srand(time(NULL));
  nonce[0] = rand();
  nonce[1] = rand();
  nonce[2] = rand();
  nonce[3] = rand();
  nonce[4] = rand();
  nonce[5] = rand();
  nonce[6] = rand();
  nonce[7] = rand();
  nonce[8] = rand();

  return 0;
}
