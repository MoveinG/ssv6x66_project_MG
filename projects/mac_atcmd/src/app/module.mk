
LIB_SRC := main.c
LIB_SRC += ssv_lib.c
LIB_SRC += app_uart.c
LIB_SRC += at_cmd.c
LIB_SRC += atcmd_process.c

LIB_ASRC :=
LIBRARY_NAME := wavbmp
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=

LOCAL_INC := -I$(TOPDIR)/components/inc/bsp/$(CHIP_NAME)
LOCAL_INC := -I$(TOPDIR)/components/inc/bsp
LOCAL_INC += -I$(TOPDIR)/components/drv
LOCAL_INC += -I$(TOPDIR)/components/softmac
LOCAL_INC += -I$(TOPDIR)/components/iotapi
LOCAL_INC += -I$(TOPDIR)/components/netstack_wrapper
LOCAL_INC += -I$(TOPDIR)/components/third_party/fdk_aac_dec_andes/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/fdk_aac_dec_andes/libAACdec/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/fdk_aac_dec_andes/libSYS/include
LOCAL_INC += -I$(TOPDIR)/components/inc

LOCAL_INC += -I$(TOPDIR)/components/net/tcpip/lwip-1.4.0/src/include
LOCAL_INC += -I$(TOPDIR)/components/net/tcpip/lwip-1.4.0/src/include/ipv4
LOCAL_INC += -I$(TOPDIR)/components/net/tcpip/lwip-1.4.0/src/include/lwip
LOCAL_INC += -I$(TOPDIR)/components/net/tcpip/lwip-1.4.0/ports/icomm/include
LOCAL_INC += -I$(TOPDIR)/projects/mac_atcmd/src/cli
LOCAL_INC += -I$(TOPDIR)/components/softmac

ifeq ($(strip $(SUPPORT_PARTITION_CFG_TABLE)), 1)
LOCAL_INC += -I$(PROJ_DIR)/src/cfg
endif

$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
