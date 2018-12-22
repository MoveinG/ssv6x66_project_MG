#include "stdint.h"
#include "atcmd_process.h"
#include "at_cmd.h"
#include "atcmd.h"
#include "wificonf.h"
#include "osal.h"



extern IEEE80211STATUS gwifistatus;
OsTimer cntTimeOut = NULL;




extern void atwificbfunc(WIFI_RSP *msg);
extern void scan_cbfunc(void);



int32_t AT_UartProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_ApStaProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_RebootProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_AppVerProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_ExitProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_ResetProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);

void scan_cb_func()
{
    u8 i;
	char rsp[100] = {0};
	extern u8 getAvailableIndex();
    printf("\nCount:%d\n", getAvailableIndex());
	app_uart_send("+CWLAP:",strlen("+CWLAP:"));
    for(i = 0; i < getAvailableIndex(); i++)
    {
    	sprintf(rsp,"%d,%s,-%d,%.02x:%.02x:%.02x:%.02x:%.02x:%.02x,%d\r\n",\
			ap_list[i].security_type,\
			ap_list[i].name,\
			ap_list[i].rssi,\
			ap_list[i].mac[0],\
			ap_list[i].mac[1],\
			ap_list[i].mac[2],\
			ap_list[i].mac[3],\
			ap_list[i].mac[4],\
			ap_list[i].mac[5],\
			ap_list[i].channel);
		app_uart_send(rsp,strlen(rsp));
		
        printf("%2d - name:%32s, rssi:-%2d CH:%2d mac:%02x-%02x-%02x-%02x-%02x-%02x\n", i, ap_list[i].name, ap_list[i].rssi, ap_list[i].channel
            , ap_list[i].mac[0], ap_list[i].mac[1], ap_list[i].mac[2], ap_list[i].mac[3], ap_list[i].mac[4], ap_list[i].mac[5]);
    }
	app_uart_send(RSP_OK,strlen(RSP_OK));
}


int32_t AT_ScanProcessing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 

	printf("\r\n[%s]:[%d] cmd:%s\r\n",__func__,__LINE__,pBuf);
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == ACTION_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		if ((get_DUT_wifi_mode() == DUT_NONE) ||\
			(get_DUT_wifi_mode() == DUT_SNIFFER)) {
        	printf("\nPlease run AT+DUT_START=1 first\n");
		}
		if (scan_AP(scan_cb_func) == 0) {
        	return CMD_NO_RESPONSE;
    	}
	} else if (cmdType == SET_PARAM_COMMAND) {
		//scan special wifi
	}
    
    return CMD_ERROR;
}

int32_t AT_SetModeProcessing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	int deviceMode       = DEFAULT;
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 

	printf("\r\n[%s]:[%d] cmd:%s\r\n",__func__,__LINE__,pBuf);
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		deviceMode = params[0][0] - '0';
		if (deviceMode > 5) {
			return CMD_ERROR;
		}
		DUT_wifi_start(deviceMode);
		return CMD_SUCCESS;
	} else if (cmdType == GET_CURE_PARAM_COMMAND) {
		deviceMode = get_DUT_wifi_mode();
		sprintf(rsp,"+CWMODE_DEF:%d\r\n",deviceMode);
		return CMD_SUCCESS;
	} else if (cmdType == GET_PARAM_COMMAND) {
		sprintf(rsp,"+CWMODE_DEF=\n1:Station\n2:SoftAP\n3:SoftAP+Station\r\n");
		return CMD_SUCCESS;
	}
	
    return CMD_ERROR;
}

void connect_timeout_handler(void)
{
	extern uint16_t cntErrorCode;
	char rsp[100] = {0};
	printf("connect ap timeout cntErrorCode:%d!\r\n",cntErrorCode);
	sprintf(rsp,"+CWJAP_DEF:%d\r\n",cntErrorCode);
	app_uart_send(rsp,strlen(rsp));
	app_uart_send(RSP_ERR,strlen(RSP_ERR));
}

int32_t AT_ConnectApProcessing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
    char *pSsid    = NULL;
	char *pWifiKey = NULL;
    int ssidLen    = 0;
	int keyLen     = 0;
	int timeOutNum = 5;
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 

	printf("\r\n[%s]:[%d] cmd:%s\r\n",__func__,__LINE__,pBuf);
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		pSsid    = params[0];
		pWifiKey = params[1];
	    ssidLen = strlen (pSsid);
    	if (pWifiKey) {
        	keyLen = strlen(pWifiKey);
    	}
		if (params[4]) {
			timeOutNum = atoi(params[4]);
		} 
		if (cntTimeOut) {
			OS_TimerDelete(cntTimeOut);
			cntTimeOut = NULL;
		}
		if (timeOutNum != 0) {
			if (OS_TimerCreate(&cntTimeOut,timeOutNum, (unsigned char)0, NULL, (OsTimerHandler)connect_timeout_handler) != OS_SUCCESS) {
				printf("connect ap timer create error!\r\n");
			}
		}
		DUT_wifi_start(DUT_STA);
		if (CIB.deviceIpConfig.devStaIpCfg.dhcpEn == 0) {
			set_if_config(CIB.deviceIpConfig.devStaIpCfg.dhcpEn, CIB.deviceIpConfig.devStaIpCfg.ip.u32, CIB.deviceIpConfig.devStaIpCfg.netmask.u32, CIB.deviceIpConfig.devStaIpCfg.gateway.u32, 0);
		}
		if (wifi_connect_active ( pSsid, ssidLen, pWifiKey, keyLen, atwificbfunc) == 0) {
			return CMD_NO_RESPONSE;
		}
	} else if (cmdType == GET_CURE_PARAM_COMMAND) {
		//aleardy connected ap msg
		if (get_wifi_status() == 1) 
		{
			uint8_t ssid[CFG_SOFTAP_SSID_LENGTH+1] = {0};
			memcpy(ssid, gwifistatus.connAP[0].ssid, gwifistatus.connAP[0].ssid_len>CFG_SOFTAP_SSID_LENGTH?CFG_SOFTAP_SSID_LENGTH:gwifistatus.connAP[0].ssid_len);

			sprintf(rsp,"+CWJAP_DEF:%s,%x:%x:%x:%x:%x:%x,%d,-%ddBm\r\n",\
				ssid,\
				gwifistatus.connAP[0].mac[0],\
				gwifistatus.connAP[0].mac[1],\
				gwifistatus.connAP[0].mac[2],\
				gwifistatus.connAP[0].mac[3],\
				gwifistatus.connAP[0].mac[4],\
				gwifistatus.connAP[0].mac[5],\
				gwifistatus.connAP[0].channel,\
				gwifistatus.connAP[0].rssi);
			
			return CMD_SUCCESS;
		} else {
			//printf("please connect STA0 to ap\r\n");
			return CMD_ERROR;
		}
	}

    return CMD_ERROR;
}

int32_t AT_DisconnectApProcessing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 

	printf("\r\n[%s]:[%d] cmd:%s\r\n",__func__,__LINE__,pBuf);
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == ACTION_COMMAND) {
		wifi_disconnect(atwificbfunc);
		return CMD_SUCCESS;
	}
	
	return CMD_ERROR;
}

int32_t AT_AutoConnectProcessing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 

	printf("\r\n[%s]:[%d] cmd:%s\r\n",__func__,__LINE__,pBuf);
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		if ((params[0][0] == '0') ||\
			(params[0][0] == '1')) {
			//set_auto_connect_flag(params[0][0] - '0');
			CIB.autoConnectEn = params[0][0] - '0';
			CIBWrite();
			return CMD_SUCCESS;
		}
	}

    return CMD_ERROR;
}

int32_t AT_SetIPConfigProcessing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 

	printf("\r\n[%s]:[%d] cmd:%s\r\n",__func__,__LINE__,pBuf);
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == ACTION_COMMAND) {
		wifi_disconnect(atwificbfunc);
		return CMD_SUCCESS;
	}

	return CMD_SUCCESS;
}

int32_t AT_GetNetParamProcessing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 

	printf("\r\n[%s]:[%d] cmd:%s\r\n",__func__,__LINE__,pBuf);
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == ACTION_COMMAND) {
		get_wifi_status();
		return CMD_SUCCESS;
	}

    return CMD_SUCCESS;
}

int32_t AT_DHCP_Processing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 

	printf("\r\n[%s]:[%d] cmd:%s\r\n",__func__,__LINE__,pBuf);
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		if (params[0][0] == '0') {
			CIB.deviceIpConfig.devApIpCfg.dhcpEn = params[0][1] - '0';
			CIBWrite();
			return CMD_SUCCESS;
		} else if (params[0][0] == '1') {
			CIB.deviceIpConfig.devStaIpCfg.dhcpEn = params[0][1] - '0';
			CIBWrite();
			return CMD_SUCCESS;
		}
	} else if (cmdType == GET_CURE_PARAM_COMMAND) {
		sprintf(rsp,"+CWDHCP_DEF:%d",CIB.deviceIpConfig.devApIpCfg.dhcpEn|(CIB.deviceIpConfig.devStaIpCfg.dhcpEn << 1));
		return CMD_SUCCESS;
	}
    return CMD_ERROR;
}

int32_t AT_APIP_ConfigProcessing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 
	
	printf("\r\n[%s]:[%d] cmd:%s\r\n",__func__,__LINE__,pBuf);
		
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		ip2int(params[0],&(CIB.deviceIpConfig.devApIpCfg.ip));
		ip2int(params[1],&(CIB.deviceIpConfig.devApIpCfg.gateway));
		ip2int(params[2],&(CIB.deviceIpConfig.devApIpCfg.netmask));
		CIB.deviceIpConfig.devApIpCfg.dhcpEn = 0;
		return CMD_SUCCESS;
	} else if (cmdType == GET_CURE_PARAM_COMMAND) {
		sprintf(rsp,"+CIPAP_DEF:%d.%d.%d.%d\r\n+CIPAP_DEF:%d.%d.%d.%d\r\n+CIPAP_DEF:%d.%d.%d.%d\r\n",\
				CIB.deviceIpConfig.devApIpCfg.ip.u8[0],\
				CIB.deviceIpConfig.devApIpCfg.ip.u8[1],\
				CIB.deviceIpConfig.devApIpCfg.ip.u8[2],\
				CIB.deviceIpConfig.devApIpCfg.ip.u8[3],\
				CIB.deviceIpConfig.devApIpCfg.gateway.u8[0],\
				CIB.deviceIpConfig.devApIpCfg.gateway.u8[1],\
				CIB.deviceIpConfig.devApIpCfg.gateway.u8[2],\
				CIB.deviceIpConfig.devApIpCfg.gateway.u8[3],\
				CIB.deviceIpConfig.devApIpCfg.netmask.u8[0],\
				CIB.deviceIpConfig.devApIpCfg.netmask.u8[1],\
				CIB.deviceIpConfig.devApIpCfg.netmask.u8[2],\
				CIB.deviceIpConfig.devApIpCfg.netmask.u8[3]);
		return CMD_SUCCESS;
	}
	return CMD_ERROR;
}

int32_t AT_STAIP_ConfigProcessing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 
	
	printf("\r\n[%s]:[%d] cmd:%s\r\n",__func__,__LINE__,pBuf);
		
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		ip2int(params[0],&(CIB.deviceIpConfig.devStaIpCfg.ip));
		ip2int(params[1],&(CIB.deviceIpConfig.devStaIpCfg.gateway));
		ip2int(params[2],&(CIB.deviceIpConfig.devStaIpCfg.netmask));
		CIB.deviceIpConfig.devStaIpCfg.dhcpEn = 0;
		CIBWrite();
		return CMD_SUCCESS;
	} else if (cmdType == GET_CURE_PARAM_COMMAND) {
   		uip_ipaddr_t ipaddr, submask, gateway, dnsserver;
		uint8_t dhcpen;
    	get_if_config(&dhcpen, (u32*)&ipaddr, (u32*)&submask, (u32*)&gateway, (u32*)&dnsserver);
		sprintf(rsp,"+CIPSTA_DEF:%d.%d.%d.%d\r\n+CIPSTA_DEF:%d.%d.%d.%d\r\n+CIPSTA_DEF:%d.%d.%d.%d\r\n",\
				ipaddr.u8[0],\
				ipaddr.u8[1],\
				ipaddr.u8[2],\
				ipaddr.u8[3],\
				gateway.u8[0],\
				gateway.u8[1],\
				gateway.u8[2],\
				gateway.u8[3],\
				submask.u8[0],\
				submask.u8[1],\
				submask.u8[2],\
				submask.u8[3]);
		return CMD_SUCCESS;
	}
	return CMD_ERROR;
}



int32_t AT_Host_Name_Processing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 
	
	printf("\r\n[%s]:[%d] cmd:%s\r\n",__func__,__LINE__,pBuf);
		
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		if (params[0][0] == '0') {
			memset(CIB.hostName,0,HOST_NAME_MAX+1);
			memcpy(CIB.hostName,params[0],strlen(params[0]));
			CIBWrite();
			return CMD_SUCCESS;
		}
	} else if (cmdType == GET_CURE_PARAM_COMMAND) {
			sprintf(rsp,"+CWHOSTNAME:%s",CIB.hostName);
			return CMD_SUCCESS;
	}
		return CMD_ERROR;
}

int32_t AT_Dev_DNS_Processing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 
	
	printf("\r\n[%s]:[%d] cmd:%s\r\n",__func__,__LINE__,pBuf);
		
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		if (!params[1][0]) {
			CIB.deviceIpConfig.devStaIpCfg.dnsEN = params[0][0] - '0';
			memcpy(params[1],"208.67.222.222",strlen("208.67.222.222"));
			ip2int(params[1],&(CIB.deviceIpConfig.devStaIpCfg.dns.u8));
		} else {
			CIB.deviceIpConfig.devStaIpCfg.dnsEN = params[0][0] - '0';
			ip2int(params[1],&(CIB.deviceIpConfig.devStaIpCfg.dns.u8));
		}
		CIBWrite();
		return CMD_SUCCESS;
	} else if (cmdType == GET_CURE_PARAM_COMMAND) {
		sprintf(rsp,"+CIPDNS_DEF:%d,\"%d.%d.%d.%d\"",CIB.deviceIpConfig.devStaIpCfg.dnsEN,\
			CIB.deviceIpConfig.devStaIpCfg.dns.u8[0],\
			CIB.deviceIpConfig.devStaIpCfg.dns.u8[1],\
			CIB.deviceIpConfig.devStaIpCfg.dns.u8[2],\
			CIB.deviceIpConfig.devStaIpCfg.dns.u8[3]);
		return CMD_SUCCESS;
	}
		return CMD_ERROR;
}


/*****************************************************************************
*
* AT_UartProcessing
*
* @param *pBuf-command's data
*         len-command's data lenght
*         *rsp-return response data
* @return CMD_SUCCESS,CMD_ERROR,CMD_READ_OK
*
* @brief Uart parameter config,ex:baudrate,data bits,stop bit ,data paity
*
******************************************************************************/
int32_t AT_UartProcessing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 

	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		printf("\r\n-----------------\r\n");
		printf("pBuf     :%s\r\n",pBuf);
		printf("params[0]:%s\r\n",params[0]);
		printf("params[1]:%s\r\n",params[1]);
		printf("params[2]:%s\r\n",params[2]);
		printf("params[3]:%s\r\n",params[3]);
		printf("params[4]:%s\r\n",params[4]);
		printf("\r\n-----------------\r\n");
		printf("\r\n------[%s]:set param     --------\r\n",__func__);
	} else if (cmdType == GET_PARAM_COMMAND) {
		printf("\r\n------[%s]:get param     --------\r\n",__func__);
	} else if (cmdType == GET_CURE_PARAM_COMMAND) {
		printf("\r\n------[%s]:get cure param--------\r\n",__func__);
	} else if (cmdType == ACTION_COMMAND) {
		printf("\r\n------[%s]:action        --------\r\n",__func__);
	}
	
   	return 0;
}

/*****************************************************************************
*
* AT_ApStaProcessing
*
* @param *pBuf-command's data
*         len-command's data lenght
*         *rsp-return response data
* @return CMD_SUCCESS,CMD_ERROR,CMD_READ_OK
*
* @brief switch to ap or station
*
******************************************************************************/
int32_t AT_ApStaProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp)
{
	printf("\r\n----[%d]:%s-----",__LINE__,__func__);
	return 0;
}


/*****************************************************************************
*
* AT_RebootProcessing
*
* @param *pBuf-command's data
*         len-command's data lenght
*         *rsp-return response data
* @return CMD_SUCCESS,CMD_ERROR,CMD_READ_OK
*
* @brief reboot system
*
******************************************************************************/
int32_t AT_RebootProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp)
{
	printf("\r\n----[%d]:%s-----",__LINE__,__func__);
  	return 0;
}


/*****************************************************************************
*
* AT_AppVerProcessing
*
* @param *pBuf-command's data
*         len-command's data lenght
*         *rsp-return response data
* @return CMD_SUCCESS,CMD_ERROR,CMD_READ_OK
*
* @brief application software version
*
******************************************************************************/
int32_t AT_AppVerProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp)
{
	printf("\r\n----[%d]:%s-----",__LINE__,__func__);
   	return 0;
}




/*****************************************************************************
*
* AT_ExitProcessing
*
* @param *pBuf-command's data
*         len-command's data lenght
*         *rsp-return response data
* @return CMD_SUCCESS,CMD_ERROR,CMD_READ_OK
*
* @brief module exit at command mode and enter transmission mode
*
******************************************************************************/
int32_t AT_ExitProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp)
{
  int status = CMD_ERROR;
  if(len == 0)
  {
    AtCmdMode = TRANSMIT_MODE;
	status = CMD_SUCCESS;
  }
  return status;
}



/*****************************************************************************
*
* AT_ResetProcessing
*
* @param *pBuf-command's data
*         len-command's data lenght
*         *rsp-return response data
* @return CMD_SUCCESS,CMD_ERROR,CMD_READ_OK
*
* @brief reset all parameters to factory
*
******************************************************************************/
int32_t AT_ResetProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp)
{
	printf("\r\n----[%d]:%s-----",__LINE__,__func__);
  	return 0;
}










