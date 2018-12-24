#ifndef AT_CMD_H
#define AT_CMD_H
#include "wificonf.h"
#include "fsal.h"

#define   DEV_MAGIC                                 20181224
#define   SERIAL_WIFI_APP_VER                       "1.0"
/******************************************************************************
 * MACRO DEFINITION  for AT Comamnd
 *
 * AT Command string must be upper case letters
 ******************************************************************************/
#define   AT_CMD_PREFIX                             "AT+"      //AT#

/* AT command type */
/* AT#UART -B115200 -D8 -S1 -P0 */
#define   AT_CMD_UART                               "UART"     //Uart parameters
/* AT#APSTA -M0 */
#define   AT_CMD_APSTA                              "APSTA"    //AP or STA mode change
/* AT#STAID -Srf_star */
#define   AT_CMD_STASSID                            "STAID"    //STA mode SSID
/* AT#STASEC -T2 -K12345678 */
#define   AT_CMD_STASEC                             "STASEC"   //STA mode secure type and secure key
/* AT#APIDHN -E1 */
#define   AT_CMD_APID_HIDDEN                        "APIDHN"   //Hidden AP SSID
/* AT#APID -Srf_star */
#define   AT_CMD_APSSID                             "APID"     //AP mode SSID
/* AT#APSEC -T2 -K12345678 */
#define   AT_CMD_APSEC                              "APSEC"  //AP mode Secure type and secure key
/* AT#SOCKET */
#define   AT_CMD_SOCK                               "SOCKET"   //Create TCP/UDP socket or connect TCP/UDP socket
/* AT#MDNS */
#define   AT_CMD_MDNS                               "MDNS"
/* AT#NWK */
#define   AT_CMD_NWK                                "NWK"
/* AT#REBOOT */
#define   AT_CMD_REBOOT                             "REBOOT"   //Reboot module
/* AT#RESET  */
#define   AT_CMD_RESET                              "RESET"    //Reset to default
/* AT#EXIT */
#define   AT_CMD_EXIT                               "EXIT"     //Exit AT command mode and enter transmission mode
/* AT#WRITE */
#define   AT_CMD_WRITE                              "WRITE"    //save to flash
/* AT#HELP */
#define   AT_CMD_HELP                               "HELP"     //help of AT command
/* AT#VER */
#define   AT_CMD_VER                                "VER"	   //APP version
/* AT#CHIP */
#define   AT_CMD_CHIP                               "CHIP"     //Chip information,include chip id,MAC address.
/* AT#DELSTAID */
#define   AT_CMD_DELSTAID                           "DELSTAID"  //delete station ssid info.
/* AT#SMTCFG */
#define   AT_CMD_SMTCFG                             "SMTCFG"    //Enable smartconfig function
/* AT#SCAN */
#define   AT_CMD_SCAN                               "SCAN"		//Enable scan Wi-Fi
/* AT#LINK */
#define   AT_CMD_CONNECT                            "CONNECT"		//Connect to the scan AP

#define AT_CMD_SCANAP                        		"CWLAP"			//scan wifi
#define AT_CMD_SETMODE                       		"CWMODE_DEF"	//set wifi mode and store in flash
#define AT_CMD_CNT_AP								"CWJAP_DEF"
#define AT_CMD_DISCNT_AP							"CWQAP"
#define AT_CMD_SET_WIFI_PARAM						"CWSAP_DEF"
#define AT_CMD_SET_AUTOCNT							"CWAUTOCONN"
#define AT_CMD_DHCP_DEF                             "CWDHCP_DEF"
#define AT_CMD_DEV_APIP_CFG                         "CIPAP_DEF"
#define AT_CMD_DEV_STAIP_CFG                        "CIPSTA_DEF"
#define AT_CMD_DEV_HOSE_NAME						"CWHOSTNAME"
#define AT_CMD_DNS									"CIPDNS_DEF"






#define   RSP_OK                                    "OK\r\n"
#define   RSP_FAIL                                  "FAIL\r\n"
#define   RSP_ERR                                   "ERROR\r\n"

/*AT command ������Ƿ�,��ĸ���Դ�д��ʾ*/
#define   SPLIT_ID                                  '-'
#define   BAUDRATE_ID                               'B'
#define   DATABIT_ID                                'D'
#define   STOPBIT_ID                                'S'
#define   DATAPAITY_ID                              'P'
#define   MODE_ID                                   'M'
#define   SSID_HIDDEN_ID                            'E'
#define   SSID_ID                                   'S'
#define   SECTYPE_ID                                'T'
#define   SECKEY_ID                                 'K'
#define   MDNSMESSAGE_ID                            'M'
/* socket parameters,include channel,protocol,mode,ip addr,port */
#define   SOCKCHAN_ID                               'C'
#define   SOCKPRO_ID                                'T'
#define   SOCKMODE_ID                               'M'
#define   SOCKADDR_ID                               'A'
#define   SOCKDOMAINS_ID                            'D'
#define   SOCKPORT_ID                               'P'
/* network info,include ip addr,sub net mask,network gateway,network DNS,device mac addr */
#define   NWKADDR_ID                                'A'
#define   NWKSUBMSK_ID                              'S'
#define   NWKGATEWAY_ID                             'G'
#define   NWKDNS_ID                                 'D'
#define   DEVMAC_ID                                 'M'
#define   MACSPLIT_ID                               ':'
#define   IPSPLIT_ID                                '.'
#define   SPACE_ID                                  ' '
#define   VER_ID                                    'V'
//#define   CHIP_ID                                   'I'
#define   DEL_ID                                    'D'
#define   SMTCFGEN_ID                               'E'
#define   SCAN_TIME_ID                              'T'
#define   SSID_LEN_ID                               'L'
#define   RSSI_ID                                   'R'
#define   NEGATIVE_ID                               '-'
#define   SCAN_LIST_ID                              'I'

#define   CR_ID                                     '\r'
#define   LF_ID                                     '\n'

#define   CMD_ERROR                                  (-1)
#define   CMD_SUCCESS                                0
#define   CMD_READ_OK                                1
#define   CMD_NO_RESPONSE                            2

#define   AT_CMD_STRINGS_MAX                         20
#define   SSID_STRINGS_MAX                           33
#define   SECURE_KEY_MAX                             65
#define   SOCKET_CHAN_MAX                            2
#define   MDNS_MESSAGES_MAX                          64
#define   URL_MAX                                    128
#define   MAC_ADDR_LEN_MAX                           6
#define   SOCKET1_PORT_NUM                           5000
#define   SOCKET2_PORT_NUM                           5001

//UART parameter default
#define   UART_DEFAULT_BAUDRATE                      115200
#define   UART_DEFAULT_DATABITS                      8
#define   UART_DEFAULT_STOPBIT                       1
#define   UART_DEFAULT_DATAPAITY                     0

//AP mode default info
#define   AP_SSID_DEFAULT                            "FC-6166-%02X%02X"
#define   AP_KEY_DEFAULT                             "12345678"
#define   AP_SCETYPE_DEFAULT                         TYPE_OPEN

#define   CIB_FILE_NAME                              "CIB.txt"

#define   MDNS_MESSAGE                               "mDNS message"

#define   AT_FUNC_PARAMS_MAX_NUM                     6
#define   AT_FUNC_PARAMS_MAX_LEN                     20
#define   HOST_NAME_MAX								 32

typedef int32_t (*ATcmdProcessMsg_t)(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);

typedef struct
{
	uint8_t *atCmd;
	ATcmdProcessMsg_t pfHandle;
    uint8_t max_parameter;
} atcmd_info_t;


typedef enum 
{
   TRANSMIT_MODE,
   ATCMD_MODE
}atcmd_mode_t;


/* typedef */
typedef enum
{
	STA_MODE = 1,
	AP_MODE = 2,
	STA2_MODE = 4
}wifi_mode_t;

typedef enum
{
	TYPE_OPEN,
	TYPE_WEP,
	TYPE_WPA
}sec_type_t;

typedef enum
{
	SOCKET_TCP,
	SOCKET_UDP
}tran_type_t;

typedef enum
{
	SOCKET_SERVER,
	SOCKET_CLIENT
}sock_mode_t;

typedef struct
{
	uint8_t dhcpEn;
	uint8_t dnsEN;
	uip_ip4addr_t ip;
	uip_ip4addr_t gateway;
	uip_ip4addr_t netmask;
	uip_ip4addr_t dns;
}deviceStaIpConfig_t;

typedef struct
{
	uint8_t dhcpEn;
	uip_ip4addr_t ip;
	uip_ip4addr_t gateway;
	uip_ip4addr_t netmask;
}deviceApIpConfig_t;

typedef struct
{
   deviceStaIpConfig_t devStaIpCfg;
   deviceApIpConfig_t  devApIpCfg;
}deviceIpConfig_t;


typedef struct
{
   uint32_t baudrate;
   uint8_t databits;
   uint8_t stopbit;
   uint8_t datapaity;
}uart_parameter_t;

typedef struct
{
   uint8_t ssid[SSID_STRINGS_MAX];
   sec_type_t sectype;
   uint8_t seckey[SECURE_KEY_MAX];
}ssid_info_t;

typedef enum
{
	ADDR_IP,
	ADDR_DOMAIN
}addr_type_t;

typedef struct
{
	addr_type_t type;
	uint32_t ip;
	uint8_t url[URL_MAX];
}addr_info_t;

typedef struct
{
	int32_t status;
	tran_type_t protype;
	sock_mode_t sockmode;
	addr_info_t addr;
	uint16_t port;
}socket_cfg_t;

typedef struct
{
   uint32_t magic;
   uart_parameter_t uartcfg;
   wifi_mode_t wifimode;
   ssid_info_t stainfo;
   ssid_info_t apinfo;
   socket_cfg_t socketcfg[SOCKET_CHAN_MAX];
   uint8_t mDNS[MDNS_MESSAGES_MAX];
   deviceIpConfig_t deviceIpConfig;
   uint8_t hostName[HOST_NAME_MAX+1];
   uint8_t autoConnectEn;
}ConfigIB_t;


typedef enum 
{
   SET_PARAM_COMMAND      = 1,
   GET_PARAM_COMMAND      = 2,
   GET_CURE_PARAM_COMMAND = 3,
   ACTION_COMMAND         = 4,
   DEFAULT                = 0xff,
}atcmd_type_e;




extern atcmd_mode_t AtCmdMode;
//extern SSV_FS CIBFsHandle;
extern ConfigIB_t CIB;


extern void Serial2WiFiInit(void);
extern void CIBInit(void);
extern SSV_FILE CIBWrite(void);
extern void AppUartProcessing(uint8_t *data,uint16_t len);
extern void CmdUartRspStatus(int32_t status);
//获取字符串中的函数参数
void at_command_param_parse(uint8_t* cmd,uint8_t len,uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN]);
//获取指令类型
atcmd_type_e atcmd_type_get(char *pcmd);


#endif



