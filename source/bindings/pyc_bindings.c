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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <gio/gio.h>
#include <assert.h>

#include <Python.h>
#include "hmkit_core_api_callback.h"
#include "hmkit_core_cert.h"
#include "pyc_bindings.h"
#include "hm_sdk_config.h"

static uint8_t isConnected = 0;
static uint8_t connectedSerial[9];

#define PY_MAIN_FUNC_NAME "py_main"

#define PYC_TAG_SEND_CUSTO_COMMAND_FUNC 0x01
#define PYC_TAG_COMMAND_INCOMING_FUNC 0x02

#if 0
// Python objects for callback, module and app
PyObject *pyFunc_cb[ePY_CALLBACK_MAX];
PyObject *pName, *pModule, *pDict, *pFunc_cb, *pFunc_main;
PyObject *pArgs_cb, *pValue, *pValue_cb;
#endif

PyObject *pArgs_cb;
PyObject *pycb_cmdincoming, *pycb_cmdresp, *pycb_entprox, *pycb_extprox, *pycb_teleincmg;

//int call_python_cb(PyObject *pFunc, PyObject *pArgsCb);
int c_py_get_connectedSerial(char *pSerialnum);
int pyc_test_api();
//int callback_func_C_Pyobj();

extern uint32_t hmkit_core_crypto_hal_ecc_add_signature(uint64_t appContxtId, uint8_t *data, uint8_t size, uint8_t *signature);

extern uint32_t hmkit_core_connectivity_hal_set_ble_name(uint8_t *devicename);

//=======================================================================================//
//=============================== Callbacks: C to Python ================================//
//=======================================================================================//

/**
*
*
*/
void py_hm_api_callback_command_incoming(hmkit_core_device_t *device, uint8_t *data, uint16_t length)
{
    PyObject *pArgs, *pArgsTemp;

	// Test
	//char hello[10]={'1', '2', '3', '4', '5', '\0', '6', '7', '8', '9'};

    //printf("# Enter %s()\n", __FUNCTION__);

    BTUNUSED(device);

	if(pycb_cmdincoming != NULL)
	{
    	// call Python callback
    	pArgs = PyTuple_New(1);

		// Test
		pArgsTemp = Py_BuildValue("y#", data, length);
		PyTuple_SetItem(pArgs, 0, pArgsTemp);

    	call_python_cb(pycb_cmdincoming, pArgs);
	}
}

/**
*
*
*/
void py_hm_api_callback_command_response(hmkit_core_device_t *device, uint8_t *data, uint16_t length)
{
    PyObject *pArgs, *pArgsTemp;

    //printf("# Enter %s()\n", __FUNCTION__);
    BTUNUSED(device);

	if(pycb_cmdresp != NULL)
	{
    	// call Python callback
    	pArgs = PyTuple_New(1);

		// Test
		pArgsTemp = Py_BuildValue("y#", data, length);
		PyTuple_SetItem(pArgs, 0, pArgsTemp);

    	call_python_cb(pycb_cmdresp, pArgs);
	}
}

/**
*
*
*/
void py_hm_api_callback_entered_proximity(hmkit_core_device_t *device)
{
    PyObject *pArgs, *pArgsTemp;

    //printf("# Enter %s()\n", __FUNCTION__);
//    BTUNUSED(device);

    if(device->is_authorised == 1){
    	//printf("# %s(), is_authorised == 1\n", __FUNCTION__);
        memcpy(connectedSerial,device->serial_number,9);
        isConnected = 1;
    }

	if(pycb_entprox != NULL)
	{
    	// call Python callback
    	pArgs = PyTuple_New(1);

		pArgsTemp = Py_BuildValue("y#", device->mac, 6);
		PyTuple_SetItem(pArgs, 0, pArgsTemp);

    	call_python_cb(pycb_entprox, pArgs);
	}

}


/**
*
*
*/

void py_hm_api_callback_exited_proximity(hmkit_core_device_t *device)
{
    PyObject *pArgs, *pArgsTemp;

    BTUNUSED(device);
	isConnected = 0;

    //printf("# Enter %s()\n", __FUNCTION__);

	if(pycb_extprox != NULL)
	{
	//    printf("#%s() call python callback\n", __FUNCTION__);
    	// call Python callback
    	pArgs = PyTuple_New(1);

	pArgsTemp = Py_BuildValue("y#", device->mac, 6);
	PyTuple_SetItem(pArgs, 0, pArgsTemp);


    	call_python_cb(pycb_extprox, pArgs);
	}
}


/**
*
*
*/

void py_hm_api_callback_telematics_command_incoming(hmkit_core_device_t *device, uint8_t id, uint16_t length, uint8_t *data){
    //printf("# Enter %s()\n", __FUNCTION__);
    PyObject *pArgs;

    BTUNUSED(device);
    BTUNUSED(id);
    BTUNUSED(length);
    BTUNUSED(data);

	if(pycb_teleincmg != NULL)
	{
    	// call Python callback
    	pArgs = PyTuple_New(0);
    	call_python_cb(pycb_teleincmg, pArgs);
	}
}


#if 0
/**
*
*
*/

int callback_func_C_Pyobj()
{
    PyObject *pArgsTemp, *pArgs, *pValue;
    char hello[10] = {'1', '2', '3', '4', '5', '\0', '6', '7', '8', '9'};

    printf("%s ,%s()\n", __FILE__, __FUNCTION__);

    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();

    /* pFunc is a new reference */
    if (pTest_cb && PyCallable_Check(pTest_cb)) {

        pArgs = PyTuple_New(1);
        pArgsTemp = Py_BuildValue("y#", hello, 10);
        PyTuple_SetItem(pArgs, 0, pArgsTemp);

        pValue = PyObject_CallObject(pTest_cb, pArgs);
        Py_DECREF(pArgs);
        if (pValue != NULL) {
            printf("Result of call: %ld\n", PyLong_AsLong(pValue));
            Py_DECREF(pValue);
        }
        else {
            //Py_DECREF(pycallback);
            PyErr_Print();
            fprintf(stderr,"Call failed\n");
            return 1;
        }
    }
    else
    {
        fprintf(stderr," Function object is invalid\n");
    }

 PyGILState_Release(gstate);

    printf("Returned: %s ,%s()\n", __FILE__, __FUNCTION__);
}
#endif

/**
*
*
*/

int py_hm_register_cb(PyObject *cb, cb_keywrds_t eKeywrd)
{
	int ret = 0;

    //printf("Enter %s(), cb enum = %d\n", __FUNCTION__, eKeywrd);
	
	if(eKeywrd == eCB_KEYWRD_CMDINCOMING)
	{
		pycb_cmdincoming = cb;
	}
	else if(eKeywrd == eCB_KEYWRD_CMDRESP)
	{
		pycb_cmdresp = cb;	
	}
	else if(eKeywrd == eCB_KEYWRD_ENTPROX)
	{
		pycb_entprox = cb;	
	}
	else if(eKeywrd == eCB_KEYWRD_EXTPROX)
	{
		pycb_extprox = cb;	
	}	
	else if(eKeywrd == eCB_KEYWRD_TELEINCM)
	{
		pycb_teleincmg = cb;	
	}
	else
	{
    	printf("C: Error %s(), callback register keyword enum does not match\n", __FUNCTION__);
		ret = -1;
	}

	return ret;
}


/**
*
*
*/

int py_hm_unregister_cb(void)
{
	int ret = 0;

    //printf("Enter %s() \n", __FUNCTION__);

	pycb_cmdincoming = NULL;
	pycb_cmdresp = NULL;
	pycb_entprox = NULL;
	pycb_extprox = NULL;
	pycb_teleincmg = NULL;

	return ret;
}




/**
*
*
*/
int c_py_get_connectedSerial(char *pSerialnum)
{
	int ret =0;

 	//printf("Enter %s, %s()\n", __FILE__, __FUNCTION__);

	if(isConnected)
	{
		 memcpy(pSerialnum, connectedSerial,9);
	}
	else
	{
 	printf("NotConnected  %s, %s()\n", __FILE__, __FUNCTION__);
		ret = -1;
	}

	return ret;
}


/**
*
*
*/
int call_python_cb(PyObject *pFunc, PyObject *pArgsCb)
{
    PyObject *pValue;

	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();

    //printf("Enter %s ,%s()\n", __FILE__, __FUNCTION__);

    /* pFunc is a new reference */
    if(pFunc && PyCallable_Check(pFunc)) {

    //printf("%s(), before PyObject_CallObject\n", __FUNCTION__);

        pValue = PyObject_CallObject(pFunc, pArgsCb);
        Py_DECREF(pArgsCb);
        if (pValue != NULL) {
            //printf("Result of call: %ld\n", PyLong_AsLong(pValue));
            Py_DECREF(pValue);
        }
        else {
            Py_DECREF(pFunc);
            //Py_DECREF(pModule);
            PyErr_Print();
            fprintf(stderr,"Call failed\n");
            return 1;
        }
    }
    else
    {
        fprintf(stderr," Function object is invalid\n");
    }

    PyGILState_Release(gstate);
}

//=======================================================================================//
//=================================== Python to C =======================================//
//=======================================================================================//

/**
*
*
*/
int py_sendcommand(char *pMsg, int len)
{
	int ret =0;
	uint8_t Serial[9];

	//printf("%s(), MsgLen = %d, Msg = %s\n", __FUNCTION__, len, pMsg);
debug_dump(pMsg, len);

	if(!c_py_get_connectedSerial(Serial))
	{
		// TODO, Request ID
		// TODO, Secure Container Version 2
		// TODO, Content Type
		//hmkit_core_api_send_custom_command((uint64_t)0x00, Serial, pMsg, len, NULL, 0, 1);
		hmkit_core_api_send_custom_command((uint64_t)0x00, Serial, 1, pMsg, len, NULL, 0, 2);
		//hmkit_core_api_send_custom_command(uint64_t appContxtId, uint8_t *serial_number, uint8_t *data, uint32_t size, uint8_t *reqID, uint16_t reqID_size, uint8_t version)
	}
	else
	{
 		printf("##### ERROR #### %s() cannot get Serial Num \n", __FUNCTION__);
		ret -1;
	}

 	//printf("return %s, %s()\n", __FILE__, __FUNCTION__);

	return ret;
}

/**
 *
 *
 */
int py_generate_signature(uint8_t *buffer, uint16_t length, PyObject **pSign )
{
	PyObject *pArgs, *pArgsTemp;
	//printf("####### Enter %s, %s() \n", __FILE__, __FUNCTION__);

	// Generate signature
	uint8_t signature[64];

    //printf(" *CORE2 DEBUG %s(), buff = %p, len = %d, sign-p %p, sign-x %x\n", __FUNCTION__, buffer, length, signature, signature);

        int ret = hmkit_core_crypto_hal_ecc_add_signature(0x00, buffer, length, signature);
	//assert(0 == hmkit_core_crypto_hal_ecc_add_signature((unit64_t)0x0000000000, buffer, length, signature));

	// convert the signature to python object
	*pSign = PyTuple_New(1);
	pArgsTemp = Py_BuildValue("y#", signature, 64);
	PyTuple_SetItem(*pSign, 0, pArgsTemp);

#ifdef PYC_DEBUG
	debug_dump(signature, 64);
#endif

	//printf("####### return %s, %s() \n", __FILE__, __FUNCTION__);
}


/**
 *
 *
 */
int py_store_accesscertificate(uint8_t *certf, uint8_t len, uint8_t *ser_num)
{
	uint32_t ret = 0;
    //printf(" CORE2 DEBUG %d %s ,%s()\n", __LINE__, __FILE__, __FUNCTION__);

	// TODO: Error Handling
	ret = hmkit_core_persistence_hal_add_access_certificate((uint64_t)0x00, ser_num, certf, len);

    //printf(" CORE2 DEBUG %d %s ,%s()\n", __LINE__, __FILE__, __FUNCTION__);
	//debug_dump(certf, len);
}

/**
 *
 *
 */
int py_get_accesscertificate(uint8_t *ser_num, PyObject **pAccessCert)
{
	PyObject *pArgs, *pArgsTemp;
	//printf("####### Enter %s, %s() \n", __FILE__, __FUNCTION__);

	// Generate signature
	uint8_t access_cert[256];
	uint16_t length=0;

	hmkit_core_persistence_hal_get_access_certificate((uint64_t)0x00, ser_num, access_cert, &length);

	// convert the signature to python object
	*pAccessCert = PyTuple_New(1);
	pArgsTemp = Py_BuildValue("y#", access_cert, length);
	PyTuple_SetItem(*pAccessCert, 0, pArgsTemp);

#ifdef PYC_DEBUG
	debug_dump(access_cert, length);
#endif
}

/**
 *
 *
 */
int py_delete_accesscertificate(uint8_t *ser_num)
{
	//printf("####### Enter %s, %s() \n", __FILE__, __FUNCTION__);

	hmkit_core_persistence_hal_remove_access_certificate(ser_num);

#ifdef PYC_DEBUG
	debug_dump(ser_num, 9);
#endif
}

/**
*
*/
void py_set_issuer_pub(uint8_t *pub)
{
 	//printf("%s, %s()\n", __FILE__, __FUNCTION__);

	hm_sdk_config_set_issuer_pub(pub);
}

/**
*
*/
void py_set_dev_prv(uint8_t *prv)
{
 	//printf("%s, %s()\n", __FILE__, __FUNCTION__);
	hm_sdk_config_set_dev_prv(prv);
}

/**
*
*/
void py_set_dev_cert(uint8_t *cert)
{
 	//printf("%s, %s()\n", __FILE__, __FUNCTION__);
	hm_sdk_config_set_device_certificate(cert);
}

/**
*
*/
void py_set_ble_device_name(uint8_t *name)
{
    //printf("%s, %s()\n", __FILE__, __FUNCTION__);
    hmkit_core_connectivity_hal_set_ble_name(name);
}

int py_ble_advertisement_start()
{
	int ret = 0;
	uint8_t cert[153];

 	//printf("%s, %s()\n", __FILE__, __FUNCTION__);
	hmkit_core_persistence_hal_get_device_certificate((uint64_t)0x00, cert);

	ret = hmkit_core_connectivity_hal_advertisement_start(cert,cert + 4);

	return ret;
}

int py_ble_advertisement_stop()
{
	int ret = 0;
 	//printf("%s, %s()\n", __FILE__, __FUNCTION__);

	ret = hmkit_core_connectivity_hal_advertisement_stop();

	return ret;
}


/*
void debug_dump(char *ptr, int len)
{
    int i = 0;

    if(ptr == NULL)
    {
        printf("Error %s() input ptr is NULL ", __FUNCTION__);
        return;
    }

    printf("Dump: len = %d :", len);

    for(i=0; i<len; i++)
    {
        printf(" 0x%x, ", ptr[i]);
    }

    printf("\n");
}

*/
