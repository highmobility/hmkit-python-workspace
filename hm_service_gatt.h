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

// hm_service_gatt.h generated from hm_service.gatt for BTstack
// att db format version 1

// binary attribute representation:
// - size in bytes (16), flags(16), handle (16), uuid (16/128), value(...)

#include <stdint.h>

const uint8_t hm_profile_data[] =
{
    // ATT DB Version
    1,

    // HM Service
    // 0x0001 PRIMARY_SERVICE-713d0100-503e-4c75-ba94-3148f18d941e
    0x18, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x1e, 0x94, 0x8d, 0xf1, 0x48, 0x31, 0x94, 0xba, 0x75, 0x4c, 0x3e, 0x50, 0x00, 0x01, 0x3d, 0x71, 
    // notify read
    // 0x0002 CHARACTERISTIC-713d0102-503e-4c75-ba94-3148f18d941e-READ | NOTIFY | DYNAMIC
    0x1b, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x12, 0x03, 0x00, 0x1e, 0x94, 0x8d, 0xf1, 0x48, 0x31, 0x94, 0xba, 0x75, 0x4c, 0x3e, 0x50, 0x02, 0x01, 0x3d, 0x71, 
    // 0x0003 VALUE-713d0102-503e-4c75-ba94-3148f18d941e-READ | NOTIFY | DYNAMIC-''
    // READ_ANYBODY
    0x16, 0x00, 0x02, 0x03, 0x03, 0x00, 0x1e, 0x94, 0x8d, 0xf1, 0x48, 0x31, 0x94, 0xba, 0x75, 0x4c, 0x3e, 0x50, 0x02, 0x01, 0x3d, 0x71, 
    // 0x0004 CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0e, 0x01, 0x04, 0x00, 0x02, 0x29, 0x00, 0x00, 
    // write
    // 0x0005 CHARACTERISTIC-713d0103-503e-4c75-ba94-3148f18d941e-WRITE_WITHOUT_RESPONSE | DYNAMIC
    0x1b, 0x00, 0x02, 0x00, 0x05, 0x00, 0x03, 0x28, 0x04, 0x06, 0x00, 0x1e, 0x94, 0x8d, 0xf1, 0x48, 0x31, 0x94, 0xba, 0x75, 0x4c, 0x3e, 0x50, 0x03, 0x01, 0x3d, 0x71, 
    // 0x0006 VALUE-713d0103-503e-4c75-ba94-3148f18d941e-WRITE_WITHOUT_RESPONSE | DYNAMIC-''
    // WRITE_ANYBODY
    0x16, 0x00, 0x04, 0x03, 0x06, 0x00, 0x1e, 0x94, 0x8d, 0xf1, 0x48, 0x31, 0x94, 0xba, 0x75, 0x4c, 0x3e, 0x50, 0x03, 0x01, 0x3d, 0x71, 
    // notify read
    // 0x0007 CHARACTERISTIC-713d0104-503e-4c75-ba94-3148f18d941e-READ | NOTIFY | DYNAMIC
    0x1b, 0x00, 0x02, 0x00, 0x07, 0x00, 0x03, 0x28, 0x12, 0x08, 0x00, 0x1e, 0x94, 0x8d, 0xf1, 0x48, 0x31, 0x94, 0xba, 0x75, 0x4c, 0x3e, 0x50, 0x04, 0x01, 0x3d, 0x71, 
    // 0x0008 VALUE-713d0104-503e-4c75-ba94-3148f18d941e-READ | NOTIFY | DYNAMIC-''
    // READ_ANYBODY
    0x16, 0x00, 0x02, 0x03, 0x08, 0x00, 0x1e, 0x94, 0x8d, 0xf1, 0x48, 0x31, 0x94, 0xba, 0x75, 0x4c, 0x3e, 0x50, 0x04, 0x01, 0x3d, 0x71, 
    // 0x0009 CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0e, 0x01, 0x09, 0x00, 0x02, 0x29, 0x00, 0x00, 
    // read
    // 0x000a CHARACTERISTIC-713d0105-503e-4c75-ba94-3148f18d941e-READ | DYNAMIC
    0x1b, 0x00, 0x02, 0x00, 0x0a, 0x00, 0x03, 0x28, 0x02, 0x0b, 0x00, 0x1e, 0x94, 0x8d, 0xf1, 0x48, 0x31, 0x94, 0xba, 0x75, 0x4c, 0x3e, 0x50, 0x05, 0x01, 0x3d, 0x71, 
    // 0x000b VALUE-713d0105-503e-4c75-ba94-3148f18d941e-READ | DYNAMIC-''
    // READ_ANYBODY
    0x16, 0x00, 0x02, 0x03, 0x0b, 0x00, 0x1e, 0x94, 0x8d, 0xf1, 0x48, 0x31, 0x94, 0xba, 0x75, 0x4c, 0x3e, 0x50, 0x05, 0x01, 0x3d, 0x71, 
    // notify read
    // 0x000c CHARACTERISTIC-713d0106-503e-4c75-ba94-3148f18d941e-READ | NOTIFY | DYNAMIC
    0x1b, 0x00, 0x02, 0x00, 0x0c, 0x00, 0x03, 0x28, 0x12, 0x0d, 0x00, 0x1e, 0x94, 0x8d, 0xf1, 0x48, 0x31, 0x94, 0xba, 0x75, 0x4c, 0x3e, 0x50, 0x06, 0x01, 0x3d, 0x71, 
    // 0x000d VALUE-713d0106-503e-4c75-ba94-3148f18d941e-READ | NOTIFY | DYNAMIC-''
    // READ_ANYBODY
    0x16, 0x00, 0x02, 0x03, 0x0d, 0x00, 0x1e, 0x94, 0x8d, 0xf1, 0x48, 0x31, 0x94, 0xba, 0x75, 0x4c, 0x3e, 0x50, 0x06, 0x01, 0x3d, 0x71, 
    // 0x000e CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0e, 0x01, 0x0e, 0x00, 0x02, 0x29, 0x00, 0x00, 
    // write
    // 0x000f CHARACTERISTIC-713d0107-503e-4c75-ba94-3148f18d941e-WRITE_WITHOUT_RESPONSE | DYNAMIC
    0x1b, 0x00, 0x02, 0x00, 0x0f, 0x00, 0x03, 0x28, 0x04, 0x10, 0x00, 0x1e, 0x94, 0x8d, 0xf1, 0x48, 0x31, 0x94, 0xba, 0x75, 0x4c, 0x3e, 0x50, 0x07, 0x01, 0x3d, 0x71, 
    // 0x0010 VALUE-713d0107-503e-4c75-ba94-3148f18d941e-WRITE_WITHOUT_RESPONSE | DYNAMIC-''
    // WRITE_ANYBODY
    0x16, 0x00, 0x04, 0x03, 0x10, 0x00, 0x1e, 0x94, 0x8d, 0xf1, 0x48, 0x31, 0x94, 0xba, 0x75, 0x4c, 0x3e, 0x50, 0x07, 0x01, 0x3d, 0x71, 

    // END
    0x00, 0x00, 
}; // total size 188 bytes 


//
// list service handle ranges
//
#define ATT_SERVICE_713d0100_503e_4c75_ba94_3148f18d941e_START_HANDLE 0x0001
#define ATT_SERVICE_713d0100_503e_4c75_ba94_3148f18d941e_END_HANDLE 0x0010

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_713d0102_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE 0x0003
#define ATT_CHARACTERISTIC_713d0102_503e_4c75_ba94_3148f18d941e_01_CLIENT_CONFIGURATION_HANDLE 0x0004
#define ATT_CHARACTERISTIC_713d0103_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE 0x0006
#define ATT_CHARACTERISTIC_713d0104_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE 0x0008
#define ATT_CHARACTERISTIC_713d0104_503e_4c75_ba94_3148f18d941e_01_CLIENT_CONFIGURATION_HANDLE 0x0009
#define ATT_CHARACTERISTIC_713d0105_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE 0x000b
#define ATT_CHARACTERISTIC_713d0106_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE 0x000d
#define ATT_CHARACTERISTIC_713d0106_503e_4c75_ba94_3148f18d941e_01_CLIENT_CONFIGURATION_HANDLE 0x000e
#define ATT_CHARACTERISTIC_713d0107_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE 0x0010
