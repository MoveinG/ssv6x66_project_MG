

#include "stdint.h"
#include "soc_types.h"
#include "mac_cfg.h"
#include "fsal.h"
#include "at_cmd.h"
#include "atcmd_process.h"
#include "app_uart.h"


atcmd_mode_t AtCmdMode = TRANSMIT_MODE;
ConfigIB_t CIB;

extern SSV_FS fs_handle;

const atcmd_info_t atcmd_info[] = 
{
    {AT_CMD_UART,AT_UartProcessing,4},
    {AT_CMD_APSTA,AT_ApStaProcessing,1},
    {AT_CMD_REBOOT,AT_RebootProcessing,1},
    {AT_CMD_VER,AT_AppVerProcessing,0},
    {AT_CMD_EXIT,AT_ExitProcessing,0},
    {AT_CMD_RESET,AT_ResetProcessing,0},

};

void Serial2WiFiInit(void);
SSV_FILE CIBRead(void);
void CIBInit(void);
void CmdUartRspStatus(int32_t status);
void AppUartProcessing(uint8_t *data,uint16_t len);
uint8_t lower2upper(uint8_t data);
int32_t FindFuncIdx(uint8_t *cmd,uint8_t len);
int32_t ATCmdProcessing(uint8_t *buf,uint16_t len);


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

	CIBInit();
	fd = CIBRead();
	printf("fd:%d\r\n",fd);
	app_uart_int();

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
	FS_close(fs_handle,fd);
	printf("CIB read ok\r\n");
	//return fd;
  }
  else
  {
    fd = FS_open(fs_handle,CIB_FILE_NAME,SPIFFS_CREAT | SPIFFS_RDWR,0);
	if(fd >= 0)
    {
      fd = FS_write(fs_handle,fd,&CIB,sizeof(CIB));
	  FS_close(fs_handle,fd);
	  printf("CIB create and write ok\r\n");
	}
  }
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
  //printf("%s=%02X:%02X:%02X:%02X:%02X:%02X\n", ATCMD_RADIO_READ_MAC1, (uint8_t)mac_addr[0], (uint8_t)mac_addr[1], (uint8_t)mac_addr[2], (uint8_t)mac_addr[3], (uint8_t)mac_addr[4], (uint8_t)mac_addr[5]);
  memset(CIB.apinfo.ssid,0,SSID_STRINGS_MAX);
  memset(CIB.apinfo.seckey,0,SECURE_KEY_MAX);
  snprintf(CIB.apinfo.ssid,SSID_STRINGS_MAX,AP_SSID_DEFAULT,mac_addr[4],mac_addr[5]);
  printf("ap ssid:%s\r\n",CIB.apinfo.ssid);
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
}

/*****************************************************************************
*
* low2cap
*
* @param  data-lower case
*
* @return capital letter data
*
* @brief  Convert lowercase to capital letter
*
******************************************************************************/
uint8_t lower2upper(uint8_t data)
{
    if((data >= 'a') && (data <= 'z'))
    {
    	data -= 0x20;
    }
    return data;
}

/*******************************************************************************
*
* FindFuncIdx
*
* @param *cmd-at command
*        len-at command lenght
*
* @return CMD_ERROR is not found function index;Other is function index.
*
* @brief Find function index
*
******************************************************************************/
int32_t FindFuncIdx(uint8_t *cmd,uint8_t len)
{
    uint8_t i;
	uint8_t num;

    num = sizeof(atcmd_info)/sizeof(atcmd_info_t);
    for(i = 0;i < num;i++)
    {
    	if((!(memcmp(cmd,atcmd_info[i].atCmd,len))) && (len == (strlen((const char *)atcmd_info[i].atCmd))))
    	{
    		return i;
    	}
    }
    return CMD_ERROR;
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
    int32_t status = CMD_ERROR;
    uint8_t *p;
    uint8_t *pcmd;
    uint8_t cmd_len = 0;
    uint8_t read_flg = 0;
    uint8_t data_len = 0;
	int cmd_func_idx;
    static uint8_t rsp_data[100];

	if((len > 5) && (buf[len - 2] == CR_ID) && (buf[len - 1] == LF_ID))
	{
		p = buf;
		*p = lower2upper(*p);
		p++;
		*p = lower2upper(*p);
		p += 2;
		if(!(memcmp(buf,AT_CMD_PREFIX,strlen(AT_CMD_PREFIX))))
		{
			len -= strlen(AT_CMD_PREFIX);
			data_len = len;
			pcmd = p;
			while(len--)
			{
				if((*p == ' ') || (*p == '-'))
				{
					break;
				}
				else if((*p == CR_ID) && (*(p+1) == LF_ID))
				{
					read_flg = 1;
					break;
				}
				*p = lower2upper(*p);
				p++;
				cmd_len++;
			}
            cmd_func_idx = FindFuncIdx(pcmd,cmd_len);
            if(cmd_func_idx != CMD_ERROR)
            {
                if(read_flg)
                {
                   pcmd = NULL;
				   data_len = 0;
                }
				status = atcmd_info[cmd_func_idx].pfHandle(pcmd,data_len,atcmd_info[cmd_func_idx].max_parameter,rsp_data);
                if(status == CMD_READ_OK)
                {
					app_uart_send(rsp_data,strlen(rsp_data));
                }
            }
		}

	}

	return status;
}



