#include "stdint.h"
#include "soc_types.h"
#include "mac_cfg.h"
#include "fsal.h"
#include "at_cmd.h"
#include "atcmd_process.h"
#include "app_uart.h"
#include "ssl_client1.h"



#define SUCCESS           1
#define ERROR             0



ConfigIB_t CIB;
extern SSV_FS fs_handle;
atcmd_mode_t AtCmdMode = TRANSMIT_MODE;
char atcmdExclusiveChar[] = {' ','-','!','$'};
char atcmdExclusiveCharLen = sizeof(atcmdExclusiveChar)/sizeof(atcmdExclusiveChar[0]);




const atcmd_info_t atcmd_info[] = 
{
    {AT_CMD_UART,		AT_UartProcessing,			4},
    {AT_CMD_APSTA,		AT_ApStaProcessing,			1},
    {AT_CMD_REBOOT,		AT_RebootProcessing,		1},
    {AT_CMD_VER,		AT_AppVerProcessing,		0},
    {AT_CMD_EXIT,		AT_ExitProcessing,			0},
    {AT_CMD_RESET,		AT_ResetProcessing,			0},
    
    {AT_CMD_SCANAP,			AT_ScanProcessing,			0},
    //{AT_CMD_SCAN,			AT_ScanProcessing,			0},
    {AT_CMD_SETMODE,   	 	AT_SetModeProcessing,    	1},
    {AT_CMD_CNT_AP,     	AT_ConnectApProcessing,  	4},
	{AT_CMD_DISCNT_AP,   	AT_DisconnectApProcessing,  0},
	{AT_CMD_SET_AUTOCNT, 	AT_AutoConnectProcessing,  	1},
    {AT_CMD_DHCP_DEF,		AT_DHCP_Processing,			2},
    {AT_CMD_DEV_APIP_CFG,   AT_APIP_ConfigProcessing,	3},
    {AT_CMD_DEV_STAIP_CFG,	AT_STAIP_ConfigProcessing,	3},
    {AT_CMD_DEV_HOSE_NAME,	AT_Host_Name_Processing,	1},
    {AT_CMD_DNS,			AT_Dev_DNS_Processing,		1},
    {AT_CMD_TEST_SSL,       AT_Test_SSL_Processing,		0},
    {AT_CMD_MSG,			AT_Device_Msg_Processing,	0},
    {AT_CMD_CIP_START,		AT_Cfg_SendIp_Processing,	3},
    {AT_CMD_CIP_SEND,		AT_Send_Data_Processing,    4},
    {AT_CMD_CIP_CLOSE,		AT_Close_Comm_Processing,   0},
};




void Serial2WiFiInit(void);
SSV_FILE CIBRead(void);
void CIBInit(void);
void CmdUartRspStatus(int32_t status);
void AppUartProcessing(uint8_t *data,uint16_t len);
uint8_t lower2upper(uint8_t data);
int32_t find_func_Idx(uint8_t *cmd,uint8_t len);
int32_t ATCmdProcessing(uint8_t *buf,uint16_t len);
void atcmd_lower2upper(char* buf,int length);

extern OsTimer cntTimeOut;

/*****************************************************************************
*
* Serial2WiFiInit
*
* @param:null
*         
* @return:null
*
* @brief: serial to WiFi software all init
*
******************************************************************************/
void Serial2WiFiInit(void)
{
  	SSV_FILE fd;
	
	fd = CIBRead();
	printf("fd:%d\r\n",fd);
	app_uart_int();
    if (OS_TimerCreate(&cntTimeOut,5000, (unsigned char)0, NULL, (OsTimerHandler)connect_timeout_handler) != OS_SUCCESS) {
         printf("[%d]:connect ap timer create error!\r\n",__LINE__);
    }
}

/*****************************************************************************
*
* CIBRead
*
* @param:null
*         
* @return:fd
*
* @brief: read CIB data form file system
*
******************************************************************************/
SSV_FILE CIBRead(void)
{
  SSV_FILE fd;

  fd = FS_open(fs_handle,CIB_FILE_NAME,SPIFFS_RDWR,0);
  //printf("%s fd=%d\r\n",__func__,fd);
  if(fd >= 0)
  {
    fd = FS_read(fs_handle,fd,&CIB,sizeof(CIB));
	if (CIB.magic != DEV_MAGIC) {
		CIBInit();
		FS_write(fs_handle,fd,&CIB,sizeof(CIB));
	}
	int ret = FS_close(fs_handle,fd);
	printf("[%d]:CIB read ok fs_handle=%d ret=%d.\r\n",__LINE__,fs_handle,ret);
	//return fd;
  }
  else
  {
  	CIBInit();
    fd = FS_open(fs_handle,CIB_FILE_NAME,SPIFFS_CREAT | SPIFFS_RDWR | SPIFFS_TRUNC,0);
	if(fd >= 0)
    {
      fd = FS_write(fs_handle,fd,&CIB,sizeof(CIB));
	  int ret = FS_close(fs_handle,fd);
	  printf("[%d]:CIB create and write ok fs_handle=%d ret=%d.\r\n",__LINE__,fs_handle,ret);
	}
  }
  return fd;
}

SSV_FILE CIBWrite(void)
{
	SSV_FILE fd;

  	fd = FS_open(fs_handle,CIB_FILE_NAME,SPIFFS_RDWR | SPIFFS_APPEND,0);
  	
  	if(fd >= 0)
  	{
    	fd = FS_write(fs_handle,fd,&CIB,sizeof(CIB));
		int ret = FS_close(fs_handle,fd);
		printf("[%d]:CIB write ok fs_handle=%d ret=%d.\r\n",__LINE__,fs_handle,ret);
  	}
	printf("[%s]:fd=%d fs_handle=%d.\r\n",__func__,fd,fs_handle);
	return fd;
}


/*****************************************************************************
*
* CIBInit
*
* @param:null
*         
* @return:null
*
* @brief: config information base init
*
******************************************************************************/
void CIBInit(void)
{
  //UART parameter default
  CIB.magic = DEV_MAGIC;
  CIB.uartcfg.baudrate = UART_DEFAULT_BAUDRATE;
  CIB.uartcfg.databits = UART_DEFAULT_DATABITS;
  CIB.uartcfg.stopbit = UART_DEFAULT_STOPBIT;
  CIB.uartcfg.datapaity = UART_DEFAULT_DATAPAITY;

  CIB.wifimode = AP_MODE;

  memset(CIB.stainfo.seckey,0,SECURE_KEY_MAX);
  memset(CIB.stainfo.ssid,0,SSID_STRINGS_MAX);

  void *cfg_handle = wifi_cfg_init();
  uint8_t mac_addr[MAC_ADDR_LEN_MAX] = {0};
  wifi_cfg_get_addr1(cfg_handle, (char *)&mac_addr);
  wifi_cfg_deinit(cfg_handle);
  memset(CIB.apinfo.ssid,0,SSID_STRINGS_MAX);
  memset(CIB.apinfo.seckey,0,SECURE_KEY_MAX);
  snprintf(CIB.apinfo.ssid,SSID_STRINGS_MAX,AP_SSID_DEFAULT,mac_addr[4],mac_addr[5]);
  //printf("ap ssid:%s\r\n",CIB.apinfo.ssid);
  strcpy(CIB.apinfo.seckey,AP_SCETYPE_DEFAULT);
  strcpy(CIB.apinfo.seckey,AP_KEY_DEFAULT);

  memset(CIB.mDNS,0,MDNS_MESSAGES_MAX);
  strcpy(CIB.mDNS,MDNS_MESSAGE);

  //socket channel 0
  CIB.socketcfg[0].protype = SOCKET_UDP;
  CIB.socketcfg[0].sockmode = SOCKET_SERVER;
  CIB.socketcfg[0].addr.type = ADDR_IP;
  CIB.socketcfg[0].addr.ip = 0;
  memset(CIB.socketcfg[0].addr.url,0,URL_MAX);
  CIB.socketcfg[0].port = SOCKET1_PORT_NUM;

  //socket channel 1
  CIB.socketcfg[1].protype = SOCKET_TCP;
  CIB.socketcfg[1].sockmode = SOCKET_SERVER;
  CIB.socketcfg[1].addr.type = ADDR_IP;
  CIB.socketcfg[1].addr.ip = 0;
  memset(CIB.socketcfg[0].addr.url,0,URL_MAX);
  CIB.socketcfg[1].port = SOCKET2_PORT_NUM;

  #if 1
  CIB.autoConnectEn = 1;
  memset(CIB.hostName,0,HOST_NAME_MAX+1);
  CIB.deviceIpConfig.devApIpCfg.dhcpEn   	 = 1;
  CIB.deviceIpConfig.devApIpCfg.gateway.u32  = 0;
  CIB.deviceIpConfig.devApIpCfg.ip.u32       = 0;
  CIB.deviceIpConfig.devApIpCfg.netmask.u32  = 0;
  
  CIB.deviceIpConfig.devStaIpCfg.dhcpEn  	 = 1;
  CIB.deviceIpConfig.devStaIpCfg.dns.u32     = 0;
  CIB.deviceIpConfig.devStaIpCfg.dnsEN   	 = 0;
  CIB.deviceIpConfig.devStaIpCfg.gateway.u32 = 0;
  CIB.deviceIpConfig.devStaIpCfg.ip.u32      = 0;
  CIB.deviceIpConfig.devStaIpCfg.netmask.u32 = 0;
  #endif
}


/*****************************************************************************
*
* CmdUartRspStatus
*
* @param:status:CMD_SUCCESS or CMD_ERROR
*         
* @return:null
*
* @brief: at command mode response status
*
******************************************************************************/
void CmdUartRspStatus(int32_t status)
{
   if(status == CMD_SUCCESS)
   {
      app_uart_send(RSP_OK,strlen(RSP_OK));
   }
   else if(status == CMD_ERROR)
   {
      app_uart_send(RSP_ERR,strlen(RSP_ERR));
   }
}

/*****************************************************************************
*
* AppUartProcessing
*
* @param:*data--uart data
*         len--data length
* @return:null
*
* @brief:appliction uart data processing
*
******************************************************************************/
void AppUartProcessing(uint8_t *data,uint16_t len)
{
  	int32_t status;

   	if(AtCmdMode == ATCMD_MODE)
  	{
      	status = ATCmdProcessing(data,len);
      	CmdUartRspStatus(status);
   	}
   	else if(AtCmdMode == TRANSMIT_MODE)
   	{
      	printf("transmit mode\r\n");
   	}
	memset(data,0,APP_UART_BUF_MAX);
}

uint8_t lower2upper(uint8_t data)
{
    if((data >= 'a') && (data <= 'z'))
    {
    	data -= 0x20;
    }
    return data;
}

void atcmd_lower2upper(char* buf,int length)
{
	int index = 0;
	
	while (length > index) {
		*(buf+index) = lower2upper(*(buf+index));
		index++;
 	}
}

void at_command_param_parse(uint8_t* cmd,uint8_t len,uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN])
{
	uint8_t* pcmdH = NULL;
	uint8_t* pcmdT = NULL;
	uint8_t length   = 0;
	uint8_t paramNum = 0;
	uint8_t paramLen = 0;
	
	pcmdH = strchr(cmd,'=');
	pcmdT = strchr(cmd,',');
	
	length = pcmdH - cmd + 1;
	
	while ((pcmdT) && (length < len)) {
		paramLen = pcmdT - pcmdH - 1;
		length  += paramLen + 1;
		if ((*(pcmdH+1) == '"') && (*(pcmdH+2+paramLen) == '"')) {
			memcpy(params[paramNum],pcmdH+2, paramLen-2);
		} else {
			memcpy(params[paramNum],pcmdH+1, paramLen);
		}
		params[paramNum][paramLen] = '\0';
		pcmdH = pcmdT;
		pcmdT = strchr(pcmdH+1,',');
		paramNum += 1;
	}
	
	if (pcmdH) {
		pcmdT = strchr(cmd,'\r');
		paramLen = pcmdT - pcmdH - 1;
		if ((*(pcmdH+1) == '"') && (*(pcmdH+paramLen) == '"')) {
			memcpy(params[paramNum],pcmdH+2, paramLen-2);
		} else {
			memcpy(params[paramNum],pcmdH+1, paramLen);
		}
		params[paramNum][paramLen] = '\0';
	}
}

int atcmd_tail_correct(char* buf,int length)
{
	int ret = -1;
	if ((length > 5) && (buf[length - 2] == CR_ID) && (buf[length - 1] == LF_ID)) {
		ret = SUCCESS;
	}
	return ret;
}

int atcmd_head_correct(char* buf,int length)
{
	int ret = -1;
	if (!(memcmp(buf,AT_CMD_PREFIX,strlen(AT_CMD_PREFIX))))  {
		ret = SUCCESS;
	}
	return ret;
}


int atcmd_format_correct(char* buf,int length)
{
	int ret    = -1;
	int index  = 0;
	char* pBuf = NULL;

	pBuf = buf;

	if ((atcmd_head_correct(buf,length) != SUCCESS) ||\
		(!atcmd_tail_correct(buf,length)) != SUCCESS) {
		return ret;
	}

	//Error exclusion character
	while (length--) {
		while (index < atcmdExclusiveCharLen) {
			if (*pBuf == atcmdExclusiveChar[index]) {
				break;
			}
			index++;
		}
		if ((*pBuf == CR_ID) && (*(pBuf+1) == LF_ID)) {
			ret = 1;
			break;
		}
		pBuf++;
		index = 0;
	}
	return ret;
}

int32_t find_func_Idx(uint8_t *cmd,uint8_t len)
{
    uint8_t i;
	uint8_t num;

    num = sizeof(atcmd_info)/sizeof(atcmd_info_t);
    for (i = 0;i < num;i++) {
		if (!(memcmp(cmd,atcmd_info[i].atCmd,strlen((const char *)atcmd_info[i].atCmd)))) {
    		return i;
    	}
    }
    return CMD_ERROR;
}


atcmd_type_e atcmd_type_get(char *pcmd)
{
	atcmd_type_e cmdType = DEFAULT;

	if (*(pcmd) == '?') {
		cmdType = GET_CURE_PARAM_COMMAND;
	} else if (*(pcmd) == '\r') {
		cmdType = ACTION_COMMAND;
	} else if ((*(pcmd) == '=') && (*(pcmd + 1) == '?')) {
		cmdType = GET_PARAM_COMMAND;
 	} else if ((*(pcmd) == '=') && (*(pcmd + 1) != '?')) {
		cmdType = SET_PARAM_COMMAND;
	}
	
	return cmdType;
}


/*****************************************************************************
*
* ATCmdProcessing
*
* @param *buf-at command's data
*        len-data lenght
* @return CMD_ERROR,CMD_SUCCESS,CMD_READ_OK
*
* @brief Processing at command data
*
******************************************************************************/
int32_t ATCmdProcessing(uint8_t *buf,uint16_t len)
{
    uint8_t *pcmd;
	int cmdFuncIdx;
	int status  = CMD_ERROR;
    uint8_t rspData[200] = {0};
	static uint8_t sslTestEn = 0;

	atcmd_lower2upper(buf,2);
	
	if (atcmd_format_correct(pcmd,len)) {
		pcmd = buf+strlen(AT_CMD_PREFIX);
		
		cmdFuncIdx = find_func_Idx(pcmd,len);
		
		if (cmdFuncIdx != CMD_ERROR) {
			status = atcmd_info[cmdFuncIdx].pfHandle(pcmd+strlen(atcmd_info[cmdFuncIdx].atCmd),len,atcmd_info[cmdFuncIdx].max_parameter,rspData);
			if ((status == CMD_SUCCESS) && (rspData[0])) {
				app_uart_send(rspData,strlen(rspData));
			}
			pcmd = NULL;
		}
	}

	return status;
}



