# Makefile for libusb based examples
BTSTACK_ROOT = ./btstack
PYC_BINDINGS = ./source/bindings
WORKSPACE_HOME  := $(shell pwd)

#Include openssl crypto
HM_OPENSSL_C  := $(shell cd ./hmkit-crypto-c/; pwd)
#Include hmkit core
HM_BT_CORE  := $(shell cd ./hmkit-core/; pwd)
#Include HM AUTO API
#Include HM PERSISTENCE SQLITE
HM_PERSISTENCE_SQLITE  := $(shell cd ./hmkit-persistence-sqlite-c/; pwd)

SDK_LOCATION := ./hmkit-python

CORE += ad_parser.c

#COMMON  += hci_transport_h2_libusb.c btstack_run_loop_posix.c hci_transport_h4.c btstack_uart_block_posix.c le_device_db_fs.c btstack_link_key_db_fs.c
COMMON  += btstack_run_loop_posix.c hci_transport_h4.c btstack_uart_block_posix.c le_device_db_fs.c btstack_link_key_db_fs.c

VPATH += ./
VPATH += $(WORKSPACE_HOME)
VPATH += $(HM_OPENSSL_C)
VPATH += $(HM_BT_CORE)
VPATH += $(HM_PERSISTENCE_SQLITE)
VPATH += $(PYC_BINDINGS)

VPATH += ${BTSTACK_ROOT}/src
VPATH += ${BTSTACK_ROOT}/src/ble
VPATH += ${BTSTACK_ROOT}/src/ble/gatt-service
VPATH += ${BTSTACK_ROOT}/src/classic
VPATH += ${BTSTACK_ROOT}/3rd-party/mbedtls/library
VPATH += ${BTSTACK_ROOT}/3rd-party/bluedroid/decoder/srce
VPATH += ${BTSTACK_ROOT}/3rd-party/bluedroid/encoder//srce
VPATH += ${BTSTACK_ROOT}/3rd-party/micro-ecc

CFLAGS += -I.
CFLAGS += -I$(WORKSPACE_HOME)
CFLAGS += -I$(HM_OPENSSL_C)
CFLAGS += -I$(HM_BT_CORE)
CFLAGS += -I$(HM_PERSISTENCE_SQLITE)

# python C bindings debug prints
#CFLAGS += -DPYC_DEBUG

CFLAGS += -I${BTSTACK_ROOT}/src/ble
CFLAGS += -I${BTSTACK_ROOT}/src/classic
CFLAGS += -I${BTSTACK_ROOT}/src/ble/gatt-service
CFLAGS += -I${BTSTACK_ROOT}/src
CFLAGS += -I${BTSTACK_ROOT}/3rd-party/mbedtls/include
CFLAGS += -I${BTSTACK_ROOT}/3rd-party/bluedroid/decoder/include
CFLAGS += -I${BTSTACK_ROOT}/3rd-party/bluedroid/encoder/include
CFLAGS += -I${BTSTACK_ROOT}/3rd-party/tinydir/
CFLAGS += -I${BTSTACK_ROOT}/3rd-party/micro-ecc/
CFLAGS += -I${BTSTACK_ROOT}/port/raspi/
CFLAGS += -I$(PYC_BINDINGS)/

# for CVSD/SBC PLC
LDFLAGS += -lm

CORE += \
	btstack_memory.c            \
	btstack_linked_list.c	    \
	btstack_memory_pool.c       \
	btstack_run_loop.c		    \
	btstack_util.c 	            \
	btstack_chipset_bcm.c \
	btstack_chipset_bcm_download_firmware.c \
	btstack_control_raspi.c \
	btstack_link_key_db_fs.c \
	btstack_run_loop_posix.c \
	btstack_tlv_posix.c \
	btstack_uart_block_posix.c \
	btstack_slip.c \
	hci_transport_h4.c \
	hci_transport_h5.c \
	le_device_db_fs.c \
	wav_util.c                  \
	btstack_stdin_posix.c \
	raspi_get_model.c \

COMMON += \
	hci.c			            \
	hci_cmd.c		            \
	hci_dump.c		            \
	l2cap.c			            \
	l2cap_signaling.c	        \

CLASSIC += \
	btstack_link_key_db_memory.c   \
	sdp_util.c	                \
	spp_server.c  				\
	rfcomm.c	                \
	bnep.c	                    \
	sdp_server.c			            \
	device_id_server.c          \

SDP_CLIENT += \
	sdp_client.o		        \
	sdp_client_rfcomm.o		    \

ATT	+= \
	att_dispatch.c       	    \

GATT_SERVER += \
	att_db.c 				 	    \
	att_server.c        	    \

GATT_CLIENT += \
	ad_parser.c                 \
	gatt_client.c        	    \

SM += \
	sm.c 				 	    \

PAN += \
	pan.c \

MBEDTLS = 					\
	btstack_crypto.c \
	btstack_tlv.c \
	uECC.c \

# List of files for Bluedroid SBC codec
include ${BTSTACK_ROOT}/3rd-party/bluedroid/decoder/Makefile.inc
include ${BTSTACK_ROOT}/3rd-party/bluedroid/encoder/Makefile.inc

SBC_DECODER += \
	btstack_sbc_plc.c \
	btstack_sbc_bludroid.c \

SBC_ENCODER += \
	btstack_sbc_bludroid.c \
	hfp_msbc.c \

CVSD_PLC = \
	btstack_cvsd_plc.c \

#BTCORE
SRC += hm_main.c
SRC += hm_sdk_config.c

SRC += hmkit_core.c
SRC += hmkit_core_cert.c
SRC += hmkit_core_conf_access.c
SRC += hmkit_core_api.c
SRC += hmkit_core_log.c

SRC += Crypto.c

SRC += hmkit_core_crypto_hal.c
SRC += hmkit_core_debug_hal.c
SRC += hmkit_core_connectivity_hal.c
SRC += hmkit_core_persistence_hal.c
SRC += hmkit_core_api_callback.c
SRC += pyc_bindings.c

SRC += hm_persistence_sqlite.c

OBJDIR =./_build

# .o for .c
CORE_OBJ    = $(CORE:%.c=$(OBJDIR)/%.o)
COMMON_OBJ  = $(COMMON:%.c=$(OBJDIR)/%.o)
CLASSIC_OBJ  = $(CLASSIC:%.c=$(OBJDIR)/%.o)
SM_OBJ = $(SM:%.c=$(OBJDIR)/%.o) $(MBEDTLS:%.c=$(OBJDIR)/%.o)
ATT_OBJ     = $(ATT:%.c=$(OBJDIR)/%.o)
GATT_CLIENT_OBJ = $(GATT_CLIENT:%.c=$(OBJDIR)/%.o)
GATT_SERVER_OBJ = $(GATT_SERVER:%.c=$(OBJDIR)/%.o)
PAN_OBJ = $(PAN:%.c=$(OBJDIR)/%.o)
SBC_DECODER_OBJ  = $(SBC_DECODER:%.c=$(OBJDIR)/%.o)
SBC_ENCODER_OBJ  = $(SBC_ENCODER:%.c=$(OBJDIR)/%.o)
CVSD_PLC_OBJ = $(CVSD_PLC:%.c=$(OBJDIR)/%.o)
SRC_OBJ := $(SRC:%.c=$(OBJDIR)/%.o)

#Make the Directories
directories:
	@mkdir -p $(OBJDIR)

default_target: all

hmservice: ${CORE_OBJ} ${ATT_OBJ} ${SRC_OBJ} ${COMMON_OBJ} ${GATT_SERVER_OBJ} ${SM_OBJ} ${CLASSIC_OBJ}
	${CC} $^ -fPIC ${CFLAGS} ${LDFLAGS} -lpthread -lssl -lcrypto -lsqlite3 -lglib-2.0 -lgio-2.0 -lm -shared -o libhmlink.so

#Compile
$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

install:
	sudo cp libhmlink.so /usr/lib/

bindings:
	cd ${PYC_BINDINGS}/ && sudo ./bindings_build_install.sh
	cd ${WORKSPACE_HOME}

clean:
	rm -f  *.o *.out *.hex *.exe
	rm -f  hmlink
	rm -rf *.dSYM
	rm -rf ${BTSTACK_ROOT}/src/*.o
	rm -rf ${BTSTACK_ROOT}/src/ble/*.o
	rm -rf ${BTSTACK_ROOT}/src/ble/gatt-service/*.o
	rm -rf ${BTCORE_OBJ}/*.o
	rm -rf $(HM_OPENSSL_C)/*.o
	rm -rf $(OBJDIR)
	#rm -rf $(SDK_LOCATION)/hmkit/hm_pyc/*.py
	#rm -rf $(SDK_LOCATION)/hmkit/hm_pyc/libhmlink.so

# CC = gcc
#CC = arm-linux-gnueabihf-gcc
CFLAGS  += -g -std=gnu99 -Wall -Wmissing-prototypes -Wstrict-prototypes -Wshadow -Wunused-parameter
# CFLAGS += -Werror

CFLAGS += -I${BTSTACK_ROOT}/platform/posix\
		  -I${BTSTACK_ROOT}/platform/embedded\
		  -I$(BTSTACK_ROOT)/chipset/bcm\
		  -I$(HM_BT_CORE)\

VPATH += ${BTSTACK_ROOT}/platform/embedded
VPATH += ${BTSTACK_ROOT}/platform/posix
VPATH += ${BTSTACK_ROOT}/chipset/bcm
VPATH += ${BTSTACK_ROOT}/port/raspi


# use pkg-config
LIBS = glib-2.0 gio-2.0 gio-unix-2.0
CFLAGS += -I. `pkg-config --cflags $(LIBS)`

CFLAGS += $(shell python3.7-config --cflags)
LDFLAGS += $(shell python3.7-config --ldflags) -L/usr/lib/arm-linux-gnueabihf/
CFLAGS += -I/usr/local/lib/python3.7/dist-packages/numpy/core/include/

all: directories hmservice install bindings

gatt:

# compile .gatt descriptions
	python ${BTSTACK_ROOT}/tool/compile_gatt.py hm_service.gatt hm_service_gatt.h

#Create SDK
#sdk: clean all
sdk: all
	cp $(PYC_BINDINGS)/../bindings/pyc_extension_module.c  $(SDK_LOCATION)/hmkit/hm_pyc/
	#cp -rf $(PYC_BINDINGS)/build/*  $(SDK_LOCATION)/libs
	# Copy the dependencies builtin library
	cp libhmlink.so  $(SDK_LOCATION)/hmkit/hm_pyc/
	# Copy the python c extension hm_pyc module library
	cp $(PYC_BINDINGS)/build/lib.linux-armv*l-3.*/hmkit/hm_pyc.cpython*.so  $(SDK_LOCATION)/hmkit/hm_pyc/

