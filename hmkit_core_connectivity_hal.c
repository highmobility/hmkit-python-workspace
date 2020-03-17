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

#include "hmkit_core_connectivity_hal.h"
#include "hmkit_core_conf_access.h"

#include <fcntl.h>
#include <inttypes.h>
#include <signal.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <inttypes.h>
#include <errno.h>

#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include "btstack_config.h"

#include "btstack.h"
#include "btstack_debug.h"
#include "btstack_event.h"
#include "btstack_link_key_db_fs.h"
#include "btstack_memory.h"
#include "btstack_run_loop.h"
#include "btstack_run_loop_posix.h"
#include "hal_led.h"
#include "hci.h"
#include "hci_dump.h"
#include "gatt_client.h"

#include "hm_sdk_config.h"

#include "hmkit_core_log.h"
#include "hmkit_core_api_callback.h"

#include "hm_service_gatt.h"

//---
#include "bluetooth_company_id.h"
#include "btstack_stdin.h"
#include "btstack_tlv_posix.h"

#include "btstack_chipset_bcm.h"
#include "btstack_chipset_bcm_download_firmware.h"
#include "btstack_control_raspi.h"

#include "raspi_get_model.h"

//------------------------------------------------//

typedef enum  {
    UART_INVALID,
    UART_SOFTWARE_NO_FLOW,
    UART_HARDWARE_NO_FLOW,
    UART_HARDWARE_FLOW
} uart_type_t;

// default config, updated depending on RasperryPi UART configuration
static hci_transport_config_uart_t transport_config = {
    HCI_TRANSPORT_CONFIG_UART,
    115200,
    0,       // main baudrate
    0,       // flow control
    NULL,
};
static btstack_uart_config_t uart_config;

static int main_argc;
static const char ** main_argv;

static btstack_packet_callback_registration_t hci_event_callback_registration;

#define TLV_DB_PATH_PREFIX "/tmp/btstack_"
#define TLV_DB_PATH_POSTFIX ".tlv"
static char tlv_db_path[100];
static const btstack_tlv_t * tlv_impl;
static btstack_tlv_posix_t   tlv_context;

static int led_state = 0;
void hal_led_toggle(void){
    led_state = 1 - led_state;
    printf("LED State %u\n", led_state);
}



//----------------------------------------------//


static hci_transport_config_uart_t config = {
        HCI_TRANSPORT_CONFIG_UART,
        115200,
        0,  // main baudrate
        1,  // flow control
        NULL,
};

typedef struct {
    uint8_t character_id;
    uint8_t is_available;
    uint8_t is_configured;
    gatt_client_characteristic_t characteristic;
}bt_character_t;

typedef struct {
    hci_con_handle_t handle;
    uint8_t mac[6];
    bt_character_t characters[6];
    uint8_t is_empty;
    gatt_client_service_t service;
    gatt_client_notification_t read_char_notification_link;
    gatt_client_notification_t read_char_notification_sensing;
    uint8_t read_buffer_06[500];
    uint16_t read_buffer_06_size;
    uint8_t read_buffer_06_ready;
    uint8_t read_buffer_02[500];
    uint16_t read_buffer_02_size;
    uint8_t read_buffer_02_ready;
}bt_connection_t;

bool blink_write_resp = false;

static uint8_t read_buffer_06_mac[6];
static uint8_t read_buffer_02_mac[6];
uint16_t g_mtu;
uint8_t g_mtu_str[6];

#define CONNECTION_COUNT 10
static bt_connection_t bt_connections[CONNECTION_COUNT];

//PROTOTYPES
static void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
void clear_connection_data(bt_connection_t * connection);
bt_connection_t *get_bt_connection_using_handle(hci_con_handle_t handle);
bt_connection_t *get_bt_connection_using_mac(uint8_t *mac);
bt_character_t *get_bt_character(bt_connection_t *connection, uint8_t character_id);
bt_connection_t *get_bt_connection_by_value_handle(uint16_t value_handle);
bt_character_t *get_bt_not_configure_character(bt_connection_t *connection);
uint8_t is_bt_connection_available(uint8_t *mac);
void create_bt_connection(uint8_t *mac);
void delete_bt_connection(uint8_t *mac);

static void attr_packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
void btstack_inits(void);
void convert_mtu_str(uint16_t mtu, uint8_t *mtu_str);

//--------------------------------------------//

 /* LISTING_START(heartbeat): Hearbeat Handler */
static int  counter = 0;
static char counter_string[30];
static int  counter_string_len;

#define HEARTBEAT_PERIOD_MS 1000
static void  heartbeat_handler(struct btstack_timer_source *ts);

static btstack_timer_source_t heartbeat;

static void heartbeat_handler(struct btstack_timer_source *ts){

    btstack_run_loop_set_timer(ts, HEARTBEAT_PERIOD_MS);
    btstack_run_loop_add_timer(ts);
}


//--------------------------------------------//

void clear_connection_data(bt_connection_t * connection){
  memset(connection->mac,0x00,6);
  connection->is_empty = 0;
  connection->handle = 0;

  connection->characters[0].character_id = hmkit_core_characteristic_link_read;
  connection->characters[1].character_id = hmkit_core_characteristic_link_write;
  connection->characters[2].character_id = hmkit_core_characteristic_alive;
  connection->characters[3].character_id = hmkit_core_characteristic_info;
  connection->characters[4].character_id = hmkit_core_characteristic_sensing_read;
  connection->characters[5].character_id = hmkit_core_characteristic_sensing_write;

  for(uint8_t i = 0 ; i < 6;i++){
    connection->characters[i].is_available = 0;
    connection->characters[i].is_configured = 0;
  }
}

bt_connection_t *get_bt_connection_using_handle(hci_con_handle_t handle){
  for(uint16_t i = 0 ; i < CONNECTION_COUNT; i++){
    if(bt_connections[i].handle == handle){
      return &bt_connections[i];
    }
  }
  return NULL;
}

bt_connection_t *get_bt_connection_using_mac(uint8_t *mac){
  for(uint16_t i = 0 ; i < CONNECTION_COUNT; i++){
    if(memcmp(bt_connections[i].mac,mac,6) == 0){
      return &bt_connections[i];
    }
  }

  return NULL;
}

uint8_t *get_bt_mac(bt_connection_t *connection){

	if(connection != NULL)
	{
		return &(connection->mac);
	}
	else
	{
		return NULL;
	}
}

bt_character_t *get_bt_character(bt_connection_t *connection, uint8_t character_id){
  for(uint8_t i = 0 ; i < 6;i++){
    if(connection->characters[i].character_id == character_id){
      return &connection->characters[i];
    }
  }
  return NULL;
}

bt_connection_t *get_bt_connection_by_value_handle(uint16_t value_handle){
  for(uint16_t i = 0 ; i < CONNECTION_COUNT; i++){
    for(uint8_t ii = 0 ; ii < 6;ii++){
      if(bt_connections[i].characters[ii].characteristic.value_handle == value_handle){
        return &bt_connections[i];
      }
    }
  }
  return NULL;
}

bt_character_t *get_bt_not_configure_character(bt_connection_t *connection){
  for(uint8_t i = 0 ; i < 6;i++){
    if(connection->characters[i].character_id == hmkit_core_characteristic_link_read ||
            connection->characters[i].character_id == hmkit_core_characteristic_sensing_read){
      if(connection->characters[i].is_configured == 0 && connection->characters[i].is_available == 1){
        return &connection->characters[i];
      }
    }
  }

  return NULL;
}

uint8_t is_bt_connection_available(uint8_t *mac){
  for(uint16_t i = 0 ; i < CONNECTION_COUNT; i++){
    if(memcmp(bt_connections[i].mac,mac,6) == 0){
      return 1;
    }
  }
  return 0;
}

void create_bt_connection(uint8_t *mac){

  ///printf("Enter %s() \n", __FUNCTION__ );

  if(is_bt_connection_available(mac) == 1){
  ///printf(" %s(), already created \n", __FUNCTION__ );
    return;
  }

  for(uint16_t i = 0 ; i < CONNECTION_COUNT; i++){
    if(bt_connections[i].is_empty == 0){
      memcpy(bt_connections[i].mac,mac,6);
      bt_connections[i].is_empty = 1;
  //printf(" %s(),  created now \n", __FUNCTION__ );
      return;
    }
  }
}

void delete_bt_connection(uint8_t *mac){
  for(uint16_t i = 0 ; i < CONNECTION_COUNT; i++){
    if(memcmp(bt_connections[i].mac,mac,6) == 0){
      clear_connection_data(&bt_connections[i]);
    }
  }
}


//-------------------------//

// see https://github.com/RPi-Distro/pi-bluetooth/blob/master/usr/bin/btuart
static int raspi_get_bd_addr(bd_addr_t addr){

    FILE *fd = fopen( "/proc/device-tree/serial-number", "r" );
    if( fd == NULL ){
        fprintf(stderr, "can't read serial number, %s\n", strerror( errno ) );
        return -1;
    }
    fscanf( fd, "%*08x" "%*02x" "%02" SCNx8 "%02" SCNx8 "%02" SCNx8, &addr[3], &addr[4], &addr[5] );
    fclose( fd );

    addr[0] =  0xb8; addr[1]  = 0x27; addr[2] =  0xeb;
    addr[3] ^= 0xaa; addr[4] ^= 0xaa; addr[5] ^= 0xaa;

    return 0;
}

// see https://github.com/RPi-Distro/pi-bluetooth/blob/master/usr/bin/btuart
// on UART_INVALID errno is set
static uart_type_t raspi_get_bluetooth_uart_type(void){

    uint8_t deviceUart0[21] = { 0 };
    FILE *fd = fopen( "/proc/device-tree/aliases/uart0", "r" );
    if( fd == NULL ) return UART_INVALID;
    fscanf( fd, "%20s", deviceUart0 );
    fclose( fd );

    uint8_t deviceSerial1[21] = { 0 };
    fd = fopen( "/proc/device-tree/aliases/serial1", "r" );
    if( fd == NULL ) return UART_INVALID;
    fscanf( fd, "%20s", deviceSerial1 );
    fclose( fd );

    // test if uart0 is an alias for serial1
    if( strncmp( (const char *) deviceUart0, (const char *) deviceSerial1, 21 ) == 0 ){
        // HW uart
        size_t count = 0;
        uint8_t buf[16];
        fd = fopen( "/proc/device-tree/soc/gpio@7e200000/uart0_pins/brcm,pins", "r" );
        if( fd == NULL ) return UART_INVALID;
        count = fread( buf, 1, 16, fd );
        fclose( fd );

        // contains assigned pins
        int pins = count / 4;
        if( pins == 4 ){
	    hmkit_core_log(NULL,NULL,HMKIT_CORE_LOG_INFO,"%s() UART_HARDWARE_FLOW \n", __FUNCTION__);
            return UART_HARDWARE_FLOW;
        } else {
	    hmkit_core_log(NULL,NULL,HMKIT_CORE_LOG_INFO,"%s() UART_HARDWARE_NO_FLOW \n", __FUNCTION__);
            return UART_HARDWARE_NO_FLOW;
        }
    } else {
	hmkit_core_log(NULL,NULL,HMKIT_CORE_LOG_INFO,"%s() UART_SOFTWARE_NO_FLOW \n", __FUNCTION__);
        return UART_SOFTWARE_NO_FLOW;
    }
}

static int raspi_speed_to_baud(speed_t baud)
{

    switch (baud) {
        case B9600:
            return 9600;
        case B19200:
            return 19200;
        case B38400:
            return 38400;
        case B57600:
            return 57600;
        case B115200:
            return 115200;
        case B230400:
            return 230400;
        case B460800:
            return 460800;
        case B500000:
            return 500000;
        case B576000:
            return 576000;
        case B921600:
            return 921600;
        case B1000000:
            return 1000000;
        case B1152000:
            return 1152000;
        case B1500000:
            return 1500000;
        case B2000000:
            return 2000000;
        case B2500000:
            return 2500000;
        case B3000000:
            return 3000000;
        case B3500000:
            return 3500000;
        case B4000000:
            return 4000000;
        default:
            return -1;
    }
}

static void raspi_get_terminal_params( hci_transport_config_uart_t *tc )
{
    // open serial terminal and get parameters

    int fd = open( tc->device_name, O_RDONLY );
    if( fd < 0 )
    {
        perror( "can't open serial port" );
        return;
    }
    struct termios tios;
    tcgetattr( fd, &tios );
    close( fd );

    speed_t ospeed = cfgetospeed( &tios );
    int baud = raspi_speed_to_baud( ospeed );
    hmkit_core_log(NULL,NULL,HMKIT_CORE_LOG_INFO,"current serial terminal parameter baudrate: %d, flow control: %s\n", baud, (tios.c_cflag&CRTSCTS)?"Hardware":"None");

    // overwrites the initial baudrate only in case it was likely to be altered before
    if( baud > 9600 )
    {
        tc->baudrate_init = baud;
        tc->flowcontrol = (tios.c_cflag & CRTSCTS)?1:0;
    }

}

static void sigint_handler(int param){
//    UNUSED(param);

    log_info("sigint_handler: shutting down");

    // reset anyway
    btstack_stdin_reset();

    // power down
    hci_power_control(HCI_POWER_OFF);
    hci_close();
    log_info("Good bye, see you.\n");
    exit(0);
}

static void phase2(int status);
int btstack_start(void);

// main
int btstack_start(){

    /// GET STARTED with BTstack ///
    btstack_memory_init();

    // use logger: format HCI_DUMP_PACKETLOGGER, HCI_DUMP_BLUEZ or HCI_DUMP_STDOUT
    const char * pklg_path = "/tmp/hci_dump.pklg";
    hci_dump_open(pklg_path, HCI_DUMP_PACKETLOGGER);
    // to print BtStack logs in STDOUT console use HCI_DUMP_STDOUT instead of HCI_DUMP_PACKETLOGGER
    //hci_dump_open(NULL, HCI_DUMP_STDOUT);

    // setup run loop
    btstack_run_loop_init(btstack_run_loop_posix_get_instance());

    // pick serial port and configure uart block driver
    transport_config.device_name = "/dev/serial1";

    // derive bd_addr from serial number
    bd_addr_t addr = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    raspi_get_bd_addr(addr);

    // set UART config based on raspi Bluetooth UART type
    int bt_reg_en_pin = -1;
    bool power_cycle = true;
    switch (raspi_get_bluetooth_uart_type()){
        case UART_INVALID:
            fprintf(stderr, "can't verify HW uart, %s\n", strerror( errno ) );
            return -1;
        case UART_SOFTWARE_NO_FLOW:
            // ??
            //printf("Software UART without flowcontrol, 460800 baud, H5, BT_REG_EN at GPIO 128l\n");
	    bt_reg_en_pin = 128;
            transport_config.baudrate_main = 460800;
            transport_config.flowcontrol = 0;
            break;
        case UART_HARDWARE_NO_FLOW:
            // Raspberry Pi 3 B
            // Raspberry Pi 3 A
            // Raspberry Pi 3 B
            // power up with H5 and without power cycle untested/unsupported
            bt_reg_en_pin = 128;
	    //printf("Hardware UART without flowcontrol, 921600 baud, H5, BT_REG_EN at GPIOO 128\n");
            transport_config.baudrate_main = 921600;
            transport_config.flowcontrol = 0;
            break;
        case UART_HARDWARE_FLOW:
            // Raspberry Pi Zero W gpio 45, 3 mbps does not work (investigation pending)
            // Raspberry Pi 3A+ vgpio 129 but WLAN + BL
            // Raspberry Pi 3B+ vgpio 129 but WLAN + BL
            transport_config.flowcontrol = 1;
            int model = raspi_get_model();
            if (model == MODEL_ZERO_W){
                bt_reg_en_pin =  45;
                transport_config.baudrate_main =  921600;
            } else {
                bt_reg_en_pin = 129;
                transport_config.baudrate_main = 3000000;
            }

#ifdef ENABLE_CONTROLLER_WARM_BOOT
            power_cycle = false;
#else
            // warn about power cycle on devices with shared reg_en pins
            if (model == MODEL_3APLUS || model == MODEL_3BPLUS){
                printf("Wifi and Bluetooth share a single RESET line and BTstack needs to reset Bluetooth -> SSH over Wifi will fail\n");
                printf("Please add ENABLE_CONTROLLER_WARM_BOOT to btstack_config.h to enable startup without RESET\n");
            }
#endif
            break;
    }
    printf("%s, %u, BT_REG_EN at GPIO %u, %s\n", transport_config.flowcontrol ? "H4":"H5", transport_config.baudrate_main, bt_reg_en_pin, power_cycle ? "Reset Controller" : "Warm Boot");

    // get BCM chipset driver
    const btstack_chipset_t * chipset = btstack_chipset_bcm_instance();
    chipset->init(&transport_config);

    // set path to firmware files
    btstack_chipset_bcm_set_hcd_folder_path("/lib/firmware/brcm");

    // setup UART driver
    const btstack_uart_block_t * uart_driver = btstack_uart_block_posix_instance();

    // extract UART config from transport config
    uart_config.baudrate    = transport_config.baudrate_init;
    uart_config.flowcontrol = transport_config.flowcontrol;
    uart_config.device_name = transport_config.device_name;
    uart_driver->init(&uart_config);

    // HW with FlowControl -> we can use regular h4 mode
    const hci_transport_t * transport;
    if (transport_config.flowcontrol){
        transport = hci_transport_h4_instance(uart_driver);
    } else {
        transport = hci_transport_h5_instance(uart_driver);
    }

    // setup HCI (to be able to use bcm chipset driver)
    hci_init(transport, (void*) &transport_config);
    hci_set_bd_addr( addr );
    hci_set_chipset(btstack_chipset_bcm_instance());

    // inform about BTstack state
    hci_event_callback_registration.callback = &hci_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    // handle CTRL-c
    signal(SIGINT, sigint_handler);

    //main_argc = argc;
    //main_argv = argv;

    // power cycle Bluetooth controller on older models without flowcontrol
    if (power_cycle){
        //printf("**** Doing power_cycle \n");
        btstack_control_raspi_set_bt_reg_en_pin(bt_reg_en_pin);
        btstack_control_t *control = btstack_control_raspi_get_instance();
        control->init(NULL);
        control->off();
        usleep( 100000 );
        control->on();
    }

    if (transport_config.flowcontrol){

        //printf("**** Flow control \n");
        // re-use current terminal speed (if there was no power cycle)
        if (!power_cycle){
            raspi_get_terminal_params( &transport_config );
        }

	btstack_inits();

    } else {

        // assume BCM4343W used in Pi 3 A/B. Pi 3 A/B+ have a newer controller but support H4 with Flowcontrol
        btstack_chipset_bcm_set_device_name("BCM43430A1");

        // phase #1 download firmware
        printf("Phase 1: Download firmware\n");

        // phase #2 start main app
        btstack_chipset_bcm_download_firmware(uart_driver, transport_config.baudrate_main, &phase2);
    }

    // go
    //btstack_run_loop_execute();

    return 0;
}


static void phase2(int status){

    if (status){
        printf("Download firmware failed\n");
        return;
    }

    printf("Phase 2: Main app\n");

    btstack_inits();
}


//-------------------------//



static void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
  UNUSED(channel);
  UNUSED(size);
  bd_addr_t addr;

  if (packet_type != HCI_EVENT_PACKET) return;

  uint8_t event = hci_event_packet_get_type(packet);
  hci_con_handle_t handle = 0;

  switch (event) {
    case HCI_EVENT_LE_META: {
      //printf("%s(),  HCI_EVENT_LE_META: \n", __FUNCTION__ );
      if (hci_event_le_meta_get_subevent_code(packet) !=  HCI_SUBEVENT_LE_CONNECTION_COMPLETE) break;

      hmkit_core_log(NULL,NULL,HMKIT_CORE_LOG_INFO,"[HMCONNECTIVITY] CONNECT COMPLETE");

      bd_addr_t address;
      hci_subevent_le_connection_complete_get_peer_address(packet, address);

      uint8_t status = hci_subevent_le_connection_complete_get_status(packet);

      if (status != 0) {
        return;
      }

      //printf("%s(),  HCI_EVENT_LE_META: create_bt_connection\n", __FUNCTION__ );
      create_bt_connection(address);
      bt_connection_t *connection = get_bt_connection_using_mac(address);
      if(connection == NULL){
      printf("Error %s(),  HCI_EVENT_LE_META: connection = NULL \n", __FUNCTION__ );
        break;
      }

      connection->handle = hci_subevent_le_connection_complete_get_connection_handle(packet);

      hmkit_core_link_connect((uint64_t)0x00, address);

      break;
    }
    case HCI_EVENT_DISCONNECTION_COMPLETE: {
      //printf("%s(),  HCI_EVENT_DISCONNECTION_COMPLETE:\n", __FUNCTION__ );
      hmkit_core_log(NULL,NULL,HMKIT_CORE_LOG_INFO,"[HMCONNECTIVITY] DISCONNECT COMPLETE");
      handle = hci_event_disconnection_complete_get_connection_handle(packet);
      bt_connection_t *connection = get_bt_connection_using_handle(handle);
      if(connection == NULL){
        break;
      }
      hmkit_core_link_disconnect(connection->mac);
      delete_bt_connection(connection->mac);
      break;
    }

    case BTSTACK_EVENT_STATE:
	if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) break;
	gap_local_bd_addr(addr);
	printf("BTstack up and running at %s\n",  bd_addr_to_str(addr));
#if 0
	// setup TLV
	strcpy(tlv_db_path, TLV_DB_PATH_PREFIX);
	strcat(tlv_db_path, bd_addr_to_str(addr));
	strcat(tlv_db_path, TLV_DB_PATH_POSTFIX);
	tlv_impl = btstack_tlv_posix_init_instance(&tlv_context, tlv_db_path);
	btstack_tlv_set_instance(tlv_impl, &tlv_context);
#endif
	break;

        case HCI_EVENT_COMMAND_COMPLETE:
	    //printf("***** %s() HCI_EVENT_COMMAND_COMPLETE \n", __FUNCTION__);
            if (HCI_EVENT_IS_COMMAND_COMPLETE(packet, hci_read_local_name))
	    {
                if (hci_event_command_complete_get_return_parameters(packet)[0]) break;
                // terminate, name 248 chars
                packet[6+248] = 0;
                printf("Local name: %s\n", &packet[6]);

                btstack_chipset_bcm_set_device_name((const char *)&packet[6]);
            }
            break;

    default:
//  printf("********* %s(): default \n", __FUNCTION__);
      break;
  }
}

void hmkit_core_connectivity_hal_delay_ms(uint32_t number_of_ms){
  usleep(1000*number_of_ms);
}

uint32_t hmkit_core_connectivity_hal_scan_start(){
  return 0;
}

uint32_t hmkit_core_connectivity_hal_scan_stop(){
  return 0;
}

static uint8_t adv_data[28] = {
        // Name
        0x09, 0x09, 'P', 'A', 'R', 'K', 'I', 'N', 'G', 'Z',
        // Service
        0x11, 0x07, 0x74, 0x65, 0x73, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint32_t hmkit_core_connectivity_hal_advertisement_start(uint8_t *issuerId, uint8_t *appId){

  UNUSED(issuerId);
  UNUSED(appId);

  hmkit_core_log(NULL,NULL,HMKIT_CORE_LOG_INFO,"[HMCONNECTIVITY] ADV START");

  memcpy(adv_data+12,issuerId,4);
  memcpy(adv_data+15,appId,12);

  // setup advertisements
  uint16_t adv_int_min = 0x0030;
  uint16_t adv_int_max = 0x0030;
  uint8_t adv_type = 0;
  bd_addr_t null_addr;
  memset(null_addr, 0, 6);
  gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
  gap_advertisements_set_data(28, (uint8_t*) adv_data);
  gap_advertisements_enable(1);

  return 0;
}

uint32_t hmkit_core_connectivity_hal_advertisement_stop(){

  hmkit_core_log(NULL,NULL,HMKIT_CORE_LOG_INFO,"[HMCONNECTIVITY] ADV STOP");
  gap_advertisements_enable(0);

  return 0;
}

uint32_t hmkit_core_connectivity_hal_write_data(uint64_t btcontxtId, uint8_t *mac, uint16_t length, uint8_t *data, hmkit_core_characteristic characteristic){

  UNUSED(btcontxtId);
  hmkit_core_log(NULL,NULL,HMKIT_CORE_LOG_INFO,"[HMCONNECTIVITY] Write to %X",characteristic);

  bt_connection_t *connection = get_bt_connection_using_mac(mac);
  if(connection == NULL){
    return 0;
  }

  if(characteristic == hmkit_core_characteristic_sensing_read){
    memcpy(connection->read_buffer_06,data,length);
    connection->read_buffer_06_size = length;
    connection->read_buffer_06_ready = 1;
    memcpy(read_buffer_06_mac,mac,6);
    //att_server_notify(connection->handle, ATT_CHARACTERISTIC_713d0106_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE, (uint8_t*) data, length);
	log_info("HIGH-MOBILITY: before att_server_request_can_send_now_event");
    att_server_request_can_send_now_event(connection->handle);
  }
  else if(characteristic == hmkit_core_characteristic_link_read){
    memcpy(connection->read_buffer_02,data,length);
    connection->read_buffer_02_size = length;
    connection->read_buffer_02_ready = 1;
    memcpy(read_buffer_02_mac,mac,6);
    //att_server_notify(connection->handle, ATT_CHARACTERISTIC_713d0102_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE, (uint8_t*) data, length);
	log_info("HIGH-MOBILITY: () before att_server_request_can_send_now_event");
    att_server_request_can_send_now_event(connection->handle);
  }

  return 0;
}

uint32_t hmkit_core_connectivity_hal_read_data(uint64_t btcontxtId, uint8_t *mac, uint16_t offset, hmkit_core_characteristic characteristic){

  UNUSED(btcontxtId);
  UNUSED(offset);
  UNUSED(mac);
  UNUSED(characteristic);

  return 0;
}

uint32_t hmkit_core_connectivity_hal_read_info(uint64_t btcontxtId, uint8_t *mac, uint16_t offset, hmkit_core_characteristic characteristic){
  UNUSED(btcontxtId);
  UNUSED(offset);
  UNUSED(mac);
  UNUSED(characteristic);

  return 0;
}

uint32_t hmkit_core_connectivity_hal_service_discovery(uint8_t *mac){
  UNUSED(mac);

  return 0;
}

static uint16_t att_read_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size){
  UNUSED(con_handle);
  UNUSED(att_handle);
  UNUSED(offset);
  UNUSED(buffer);
  UNUSED(buffer_size);

  if(att_handle == ATT_CHARACTERISTIC_713d0105_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE){

    if (buffer){
      // example: Android 1.6.4 mMTU104
      // ex: { 'P', 'y','S','d','k',0x00,0x00,0x00,0x00,0x00,'M','T','U','1','0','0'};
      uint8_t info[16] = { 'P', 'y','S','d','k',0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
      bt_connection_t *connection = get_bt_connection_using_handle(con_handle);
      if(connection != NULL)
      {
	// copy the MTU string in the last 6 bytes
	memcpy(info+15-6, g_mtu_str, 6);
      }
      else
      {
	printf("*****  %s() , connection == NULL, MTU not copied to info\n", __FUNCTION__);
      }

      memcpy(buffer, info, 16);
      return 16;
    } else {
      return 16;
    }
  }

  bt_connection_t *connection = get_bt_connection_using_handle(con_handle);
  if(connection == NULL){
    return 0;
  }

  if(att_handle == ATT_CHARACTERISTIC_713d0106_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE){
    //printf("%s() %d  ATT_CHARACTERISTIC_713d0106_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE - 106\n", __FUNCTION__, __LINE__);
    if (buffer){
	hmkit_core_log_data(NULL,NULL,HMKIT_CORE_LOG_INFO,&connection->read_buffer_06[offset], buffer_size,"[HMCONNECTIVITY] 106 READ %X", att_handle);
	memcpy(buffer, &connection->read_buffer_06[offset], buffer_size);

	// printf("**** %s() , buffer valid, size = %d\n", __FUNCTION__, buffer_size);

	// invoke link write response after this read
        att_server_request_can_send_now_event(connection->handle);
	blink_write_resp = 1;
	return buffer_size;
    } else {
	//  printf("**** %s() , buffer null, read_buffer_06_size = %d \n", __FUNCTION__, connection->read_buffer_06_size);
	return connection->read_buffer_06_size;
    }
  }
  else if(att_handle == ATT_CHARACTERISTIC_713d0102_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE)
  {
    //printf("%s() %d ATT_CHARACTERISTIC_713d0102_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE  - 102\n", __FUNCTION__, __LINE__);
    if (buffer){
      hmkit_core_log_data(NULL,NULL,HMKIT_CORE_LOG_INFO,&connection->read_buffer_06[offset], buffer_size,"[HMCONNECTIVITY] 102 READ %X", att_handle);
      memcpy(buffer, &connection->read_buffer_02[offset], buffer_size);
      //printf("**** %s() , buffer valid, size = %d\n", __FUNCTION__, buffer_size);
      return buffer_size;
    } else {
      //printf("**** %s() , buffer null, read_buffer_02_size = %d\n", __FUNCTION__, connection->read_buffer_02_size);
      return connection->read_buffer_02_size;
    }
  }

  return 0;
}

// write requests
static int att_write_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size){
  UNUSED(con_handle);
  UNUSED(offset);
  UNUSED(buffer_size);
  UNUSED(transaction_mode);

  hmkit_core_log_data(NULL,NULL,HMKIT_CORE_LOG_INFO,buffer,buffer_size,"[HMCONNECTIVITY] WRITE %X", att_handle);

  bt_connection_t *connection = get_bt_connection_using_handle(con_handle);
  if(connection == NULL){
    return 0;
  }

  //printf("** %s() , size = %d \n", __FUNCTION__, buffer_size);
  if(att_handle == ATT_CHARACTERISTIC_713d0107_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE){
    hmkit_core_link_incoming_data((uint64_t)0x00, buffer, buffer_size, connection->mac, hmkit_core_characteristic_sensing_write);
  }

  if(att_handle == ATT_CHARACTERISTIC_713d0103_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE){
    hmkit_core_link_incoming_data((uint64_t)0x00, buffer, buffer_size, connection->mac, hmkit_core_characteristic_link_write);
  }

  return 0;
}

static void attr_packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
UNUSED(channel);
int ret =0;
uint16_t mtu = 0;

  switch (packet_type) {
    case HCI_EVENT_PACKET:
	///printf("***** %s() HCI_EVENT_PACKET \n", __FUNCTION__);
      switch (hci_event_packet_get_type(packet)) {

        case ATT_EVENT_CAN_SEND_NOW: {
	  log_info("HIGH-MOBILITY: received ATT_EVENT_CAN_SEND_NOW ");
  
	  //printf("***** %s() ATT_EVENT_CAN_SEND_NOW \n", __FUNCTION__);

          bt_connection_t *connection = get_bt_connection_using_mac(read_buffer_02_mac);
          if(connection == NULL){
            return 0;
          }

          if(connection->read_buffer_02_ready == 1){
            connection->read_buffer_02_ready = 0;
            ret = att_server_notify(connection->handle, ATT_CHARACTERISTIC_713d0102_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE, connection->read_buffer_02, connection->read_buffer_02_size);
            hmkit_core_log(NULL,NULL,HMKIT_CORE_LOG_INFO,"[HMCONNECTIVITY] CAN SEND NOW %x, notify ret = %d",ATT_CHARACTERISTIC_713d0102_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE, ret);
          }

          connection = get_bt_connection_using_mac(read_buffer_06_mac);
          if(connection == NULL){
            return 0;
          }

          if(connection->read_buffer_06_ready == 1){
            connection->read_buffer_06_ready = 0;
            ret = att_server_notify(connection->handle, ATT_CHARACTERISTIC_713d0106_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE, connection->read_buffer_06, connection->read_buffer_06_size);
            hmkit_core_log(NULL,NULL,HMKIT_CORE_LOG_INFO,"[HMCONNECTIVITY] CAN SEND NOW %x, ret = %d",ATT_CHARACTERISTIC_713d0106_503e_4c75_ba94_3148f18d941e_01_VALUE_HANDLE, ret);
          }

	  if(blink_write_resp == true)
	  {
		 //hmkit_core__link_write_response((uint64_t)0x00, NULL, hmkit_core_characteristic_link_write);
		 hmkit_core_link_write_response((uint64_t)0x00, read_buffer_06_mac, hmkit_core_characteristic_link_write);
		 ///printf("***** %s() ATT_EVENT_CAN_SEND_NOW, blink_write_resp \n", __FUNCTION__);
		 blink_write_resp = false;
	  }

          break;
        }

	case ATT_EVENT_MTU_EXCHANGE_COMPLETE:

	    mtu = att_event_mtu_exchange_complete_get_MTU(packet);
	    printf(" ****** ATT MTU = %u\n", mtu);

	    bt_connection_t *connection = get_bt_connection_using_handle(att_event_mtu_exchange_complete_get_handle(packet));
            if(connection == NULL){
	    printf(" ERROR ****** ATT MTU, connection is NULL\n");
              return 0;
            }

	    // set the mtu value to BtCore for Packets chunking 
	    // set the MTU value in the connnection handle
	    uint32_t ret = hmkit_core_set_mtu(get_bt_mac(connection), mtu);
	    if(!ret)
	    { // successfully set in core
		g_mtu = mtu;
	    }
	    else if(ret == 1)
	    { // Error case
		if(mtu > 512)// Max limit imposed in core
		{
		   g_mtu = 512;
		}
		else // failure due to handle not created in core
		{
		   g_mtu = mtu;
		}
            }
	    /*
	    else
	    {   // need change in core
		g_mtu = ret;
            }*/

	    printf("  mtu (in info char)= %u\n", g_mtu);
	    convert_mtu_str(g_mtu, g_mtu_str);

	    //context = connection_for_conn_handle(att_event_mtu_exchange_complete_get_handle(packet));
	    //if (!context) break;
	    //	context->test_data_len = btstack_min(mtu - 3, sizeof(context->test_data));
	    // 	printf("%c: ATT MTU = %u => use test data of len %u\n", context->name, mtu, context->test_data_len);
	    break;

        case SM_EVENT_AUTHORIZATION_REQUEST:

	     break;

        default:
	  //printf("********* %s(): default \n", __FUNCTION__);
          break;
      }
          break;
    default:
      break;
  }
}

void convert_mtu_str(uint16_t mtu, uint8_t *mtu_str)
{

   mtu_str[0] = 'M';
   mtu_str[1] = 'T';
   mtu_str[2] = 'U';

   if(mtu > 99)
   {
     mtu_str[3] = '0'+ (mtu/100);
     mtu_str[4] = '0'+ ((mtu%100)/10);
     mtu_str[5] = '0'+ (mtu%10);
   }
   else
   {
     mtu_str[3] = '0';
     mtu_str[4] = '0'+ (mtu/10);
     mtu_str[5] = '0'+ (mtu%10);
   }

}

uint32_t hmkit_core_connectivity_hal_init(){

  //printf("#### Connectivity_hal; btstack_start() \n");

  for(uint8_t i = 0 ; i < CONNECTION_COUNT ; i ++){
    clear_connection_data(&bt_connections[i]);
  }

  // does all the above operations
  btstack_start();

  return 0;
}

void btstack_inits()
{
   //printf("***** Enter %s()  \n", __FUNCTION__);

   l2cap_init();

   // setup le device db
   le_device_db_init();

   // setup SM: Display only
   sm_init();

   att_server_init(hm_profile_data, att_read_callback, att_write_callback);

    // register for ATT event
    att_server_register_packet_handler(attr_packet_handler);

    // set one-shot timer
    heartbeat.process = &heartbeat_handler;
    btstack_run_loop_set_timer(&heartbeat, HEARTBEAT_PERIOD_MS);
    btstack_run_loop_add_timer(&heartbeat);

    // turn on!
    hci_power_control(HCI_POWER_ON);
}

uint32_t hmkit_core_connectivity_hal_connect(const uint8_t *mac, uint8_t macType){
///printf("***** Enter %s()  \n", __FUNCTION__);
  create_bt_connection((uint8_t *)mac);
  gap_connect((uint8_t *)mac, (bd_addr_type_t)macType);

  return 0;
}

uint32_t hmkit_core_connectivity_hal_disconnect(uint64_t btcontxtId, uint8_t *mac){
  UNUSED(btcontxtId);
///printf("***** Enter %s()  \n", __FUNCTION__);
  bt_connection_t *connection = get_bt_connection_using_mac(mac);
  if(connection == NULL){
    return 0;
  }
  gap_disconnect(connection->handle);

  return 0;
}

uint32_t hmkit_core_connectivity_hal_get_current_date_time(uint8_t *day, uint8_t *month, uint8_t *year, uint8_t *minute, uint8_t *hour){
  return hmkit_core_api_callback_get_current_date_time(day, month, year, minute, hour);
}

uint32_t hmkit_core_connectivity_hal_telematics_send_data(uint64_t appContxtId_Tele, uint8_t *issuer, uint8_t *serial, uint16_t length, uint8_t *data){
  UNUSED(appContxtId_Tele);
  UNUSED(issuer);
  UNUSED(serial);
  UNUSED(length);
  UNUSED(data);
  return 0;
}

uint32_t hmkit_core_connectivity_hal_clock(void)
{
 // TODO:
 // printf("***** Enter %s()  \n", __FUNCTION__);

}
