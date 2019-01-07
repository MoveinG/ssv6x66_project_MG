#include "stdint.h"
#include "netdb.h"
#include "atcmd_process.h"
#include "at_cmd.h"
#include "atcmd.h"
#include "wificonf.h"
#include "osal.h"
#include "ssl_client1.h"
#include "sw_version.h"
#include "app_tcpip.h"
#include <string.h>





extern IEEE80211STATUS gwifistatus;
OsTimer cntTimeOut = NULL;
deviceComm_t deviceCommMsg;





extern void atwificbfunc(WIFI_RSP *msg);
extern void scan_cbfunc(void);




int32_t AT_UartProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_ApStaProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_RebootProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_AppVerProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_ExitProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_ResetProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);






int32_t AT_Device_Msg_Processing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == ACTION_COMMAND) {
		sprintf(rsp,"+GMR\r\nAT version:%s\r\nSDK version:%s\r\ncompile time:%s %s\r\n",AT_VERSION,version,__DATE__,__TIME__);
		return CMD_SUCCESS;
	}
    printf("[%d]:param error!\r\n",__LINE__);
    return CMD_ERROR;	
}


void scan_cb_func()
{
    u8 i;
	char rsp[100] = {0};
	extern u8 getAvailableIndex();
	app_uart_send("+CWLAP:\r\n",strlen("+CWLAP:\r\n"));
    for(i = 0; i < getAvailableIndex(); i++)
    {
    	sprintf(rsp,"%d,\"%s\",-%d,\"%.02x:%.02x:%.02x:%.02x:%.02x:%.02x\",%d\r\n",\
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
		
        printf("%2d - name:%32s, rssi:-%2d CH:%2d mac:%02x-%02x-%02x-%02x-%02x-%02x\n",\
			i, ap_list[i].name, ap_list[i].rssi, ap_list[i].channel,\
			ap_list[i].mac[0], ap_list[i].mac[1], ap_list[i].mac[2],\
			ap_list[i].mac[3], ap_list[i].mac[4], ap_list[i].mac[5]);
    }
	app_uart_send(RSP_OK,strlen(RSP_OK));
}


int32_t AT_ScanProcessing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == ACTION_COMMAND) {
		if ((get_DUT_wifi_mode() == DUT_NONE) ||\
			(get_DUT_wifi_mode() == DUT_SNIFFER)) {
        	printf("\nPlease run AT+DUT_START=1 first\n");
		}
		if (scan_AP(scan_cb_func) == 0) {
        	return CMD_NO_RESPONSE;
    	}
	} else if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		//scan special wifi
	}
    printf("[%d]:param error!\r\n",__LINE__);
    return CMD_ERROR;
}

int32_t AT_SetModeProcessing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	int deviceMode       = DEFAULT;
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		deviceMode = params[0][0] - '0';
		if ((deviceMode > 5) || (strlen(params[0]) > 1)) {
			printf("[%d]:param error!\r\n",__LINE__);
			return CMD_ERROR;
		}
		if ((get_DUT_wifi_mode() == DUT_STA) && (deviceMode == DUT_AP)) {
			wifi_disconnect(NULL);
			CIB.wifimode = DUT_AP;
			CIBWrite();
			DUT_wifi_start(DUT_AP);
		} else if ((get_DUT_wifi_mode() == DUT_AP) && (deviceMode == DUT_STA)) {
			CIB.wifimode = DUT_STA;
			CIBWrite();
			DUT_wifi_start(DUT_STA);
			OS_MsDelay(100);
			if (CIB.autoConnectEn) {
				wifi_connect_active ( gwifistatus.connAP[0].ssid,\
									strlen(gwifistatus.connAP[0].ssid),\
									gwifistatus.connAP[0].key,\
									strlen(gwifistatus.connAP[0].key),\
									atwificbfunc);
			}
		}
		return CMD_SUCCESS;
	} else if (cmdType == GET_CURE_PARAM_COMMAND) {
		deviceMode = get_DUT_wifi_mode();
		sprintf(rsp,"+CWMODE_DEF:%d\r\n",deviceMode);
		return CMD_SUCCESS;
	} else if (cmdType == GET_PARAM_COMMAND) {
		sprintf(rsp,"+CWMODE_DEF:\n1:Station\n2:SoftAP\n3:SoftAP+Station\r\n");
		return CMD_SUCCESS;
	}
	printf("[%d]:param error!\r\n",__LINE__);
    return CMD_ERROR;
}

void connect_timeout_handler(void)
{
	extern uint16_t cntErrorCode;
	char rsp[100] = {0};
	printf("connect ap timeout error reason:%d!\r\n",cntErrorCode);
	sprintf(rsp,"+CWJAP_DEF:%d\r\n",cntErrorCode);
	app_uart_send(rsp,strlen(rsp));
	app_uart_send(RSP_FAIL,strlen(RSP_FAIL));
	wifi_disconnect(atwificbfunc);
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
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		pSsid    = params[0];
		pWifiKey = params[1];
		printf("ssid:%s,key:%s\r\n",pSsid,pWifiKey);
	    ssidLen = strlen (pSsid);
    	if (pWifiKey) {
        	keyLen = strlen(pWifiKey);
    	}
		if ((keyLen >= AT_FUNC_PARAMS_MAX_LEN) || (ssidLen >= AT_FUNC_PARAMS_MAX_LEN)) {
			printf("[%d]:param length error!\r\n",__LINE__);
			return CMD_ERROR;
		}
		if ((params[4][0] != 0) && (strlen(params[4]) < AT_FUNC_PARAMS_MAX_LEN)) {
			params[4][strlen(params[4])] = '\0';
			timeOutNum = atoi(params[4]);
		} 
 		if (cntTimeOut) {
		    OS_TimerStop(cntTimeOut);
			OS_TimerSet(cntTimeOut,(timeOutNum ? (timeOutNum*1000) : 5000),0,NULL);
            OS_TimerStart(cntTimeOut);
		}
		if (get_DUT_wifi_mode() != DUT_STA) {
			DUT_wifi_start(DUT_STA);
		}
		if (CIB.deviceIpConfig.devStaIpCfg.dhcpEn == 0) {
			set_if_config(CIB.deviceIpConfig.devStaIpCfg.dhcpEn,\
				CIB.deviceIpConfig.devStaIpCfg.ip.u32,\
				CIB.deviceIpConfig.devStaIpCfg.netmask.u32,\
				CIB.deviceIpConfig.devStaIpCfg.gateway.u32, 0);
		}
		wifi_disconnect(NULL);
		vTaskDelay(200);
		if (wifi_connect_active ( pSsid, ssidLen, pWifiKey, keyLen, atwificbfunc) == 0) {
			return CMD_NO_RESPONSE;
		} else {
			printf("[%d]:connect ap failure!\r\n",__LINE__);
		}
	} else if (cmdType == GET_CURE_PARAM_COMMAND) {
		//aleardy connected ap msg
		if (get_wifi_status() == 1) 
		{
			uint8_t ssid[CFG_SOFTAP_SSID_LENGTH+1] = {0};
			uint8_t key[65] = {0};
			memcpy(ssid, gwifistatus.connAP[0].ssid, gwifistatus.connAP[0].ssid_len>CFG_SOFTAP_SSID_LENGTH?CFG_SOFTAP_SSID_LENGTH:gwifistatus.connAP[0].ssid_len);
			memcpy(key, gwifistatus.connAP[0].key, gwifistatus.connAP[0].key_len>64?64:gwifistatus.connAP[0].key_len);

			sprintf(rsp,"+CWJAP_DEF:\"%s\",\"%s\",\"%x:%x:%x:%x:%x:%x\",%d,-%ddBm\r\n",\
				ssid,\
				key,\
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
			printf("[%d]:device not connect ap!\r\n",__LINE__);
			return CMD_ERROR;
		}
	}
	printf("[%d]:param error!\r\n",__LINE__);
    return CMD_ERROR;
}

int32_t AT_DisconnectApProcessing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == ACTION_COMMAND) {
		wifi_disconnect(NULL);
		return CMD_SUCCESS;
	}
	printf("[%d]:param error!\r\n",__LINE__);
	return CMD_ERROR;
}

int32_t AT_AutoConnectProcessing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		if (((params[0][0] == '0') || (params[0][0] == '1')) &&\
			(strlen(params[0]) == 1)) {
			CIB.autoConnectEn = params[0][0] - '0';
			CIBWrite();
			if (get_wifi_status() == 0) {
				app_wifi_auto_connect();
			}
			return CMD_SUCCESS;
		}
	} else if (cmdType == GET_CURE_PARAM_COMMAND) {
		sprintf(rsp,"+CWAUTOCONN:%d\r\n",CIB.autoConnectEn);
		return CMD_SUCCESS;
	}
	printf("[%d]:param error!\r\n",__LINE__);
    return CMD_ERROR;
}

int32_t AT_DHCP_Processing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		if ((strlen(params[0]) > 1) ||\
			(strlen(params[1]) > 1) ||\
			(strlen(params[2]) != 0)) {
			printf("[%d]:param error!\r\n",__LINE__);
			return CMD_ERROR;
		}
		if ((params[0][0] == '0') && (params[1][0] != 0)) {
			CIB.deviceIpConfig.devApIpCfg.dhcpEn = params[1][0] - '0';
			CIBWrite();
			return CMD_SUCCESS;
		} else if ((params[0][0] == '1') && (params[1][0] != 0)) {
			CIB.deviceIpConfig.devStaIpCfg.dhcpEn = params[1][0] - '0';
			CIBWrite();
			return CMD_SUCCESS;
		}
	} else if (cmdType == GET_CURE_PARAM_COMMAND) {
		sprintf(rsp,"+CWDHCP_DEF:%d\r\n",CIB.deviceIpConfig.devApIpCfg.dhcpEn|(CIB.deviceIpConfig.devStaIpCfg.dhcpEn << 1));
		return CMD_SUCCESS;
	}
	printf("[%d]:param error!\r\n",__LINE__);
    return CMD_ERROR;
}

int32_t AT_APIP_ConfigProcessing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 
		
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		if ((strlen(params[0]) > 15) ||\
			(strlen(params[1]) > 15) ||\
			(strlen(params[2]) > 15) ||\
			(strlen(params[3]) != 0)) {
			printf("[%d]:param error!\r\n",__LINE__);
			return CMD_ERROR;
		}
		ip2int(params[0],&(CIB.deviceIpConfig.devApIpCfg.ip));
		if (params[1][0] != 0) {
			ip2int(params[1],&(CIB.deviceIpConfig.devApIpCfg.gateway));
		}
		if (params[2][0] != 0) {
			ip2int(params[2],&(CIB.deviceIpConfig.devApIpCfg.netmask));
		}
		CIB.deviceIpConfig.devApIpCfg.dhcpEn = 0;
		CIBWrite();
		return CMD_SUCCESS;
	} else if (cmdType == GET_CURE_PARAM_COMMAND) {
		sprintf(rsp,"+CIPAP_DEF:\"%d.%d.%d.%d\",\"%d.%d.%d.%d\",\"%d.%d.%d.%d\"\r\n",\
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
	printf("[%d]:param error!\r\n",__LINE__);
	return CMD_ERROR;
}

int32_t AT_STAIP_ConfigProcessing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 
		
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		if ((strlen(params[0]) > 15) ||\
			(strlen(params[1]) > 15) ||\
			(strlen(params[2]) > 15) ||\
			(strlen(params[3]) != 0)) {
			printf("[%d]:param error!\r\n",__LINE__);
			return CMD_ERROR;
		}
		ip2int(params[0],&(CIB.deviceIpConfig.devStaIpCfg.ip));
		if (params[1][0] != 0) {
			ip2int(params[1],&(CIB.deviceIpConfig.devStaIpCfg.gateway));
		}
		if (params[2][0] != 0) {
			ip2int(params[2],&(CIB.deviceIpConfig.devStaIpCfg.netmask));
		}
		CIB.deviceIpConfig.devStaIpCfg.dhcpEn = 0;
		CIBWrite();
		return CMD_SUCCESS;
	} else if (cmdType == GET_CURE_PARAM_COMMAND) {
   		uip_ipaddr_t ipaddr, submask, gateway, dnsserver;
		uint8_t dhcpen;
    	get_if_config(&dhcpen, (u32*)&ipaddr, (u32*)&submask, (u32*)&gateway, (u32*)&dnsserver);
		sprintf(rsp,"+CIPSTA_DEF:\"%d.%d.%d.%d\",\"%d.%d.%d.%d\",\"%d.%d.%d.%d\"\r\n",\
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
	printf("[%d]:param error!\r\n",__LINE__);
	return CMD_ERROR;
}



int32_t AT_Host_Name_Processing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 

	if (get_DUT_wifi_mode() != DUT_STA) {
		printf("[%d]:device mode is't sta!\r\n",__LINE__);
		return CMD_ERROR;
	}
			
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		if ((params[0][0] != 0) && (strlen(params[0]) <= HOST_NAME_MAX)) {
			memset(CIB.hostName,0,HOST_NAME_MAX+1);
			memcpy(CIB.hostName,params[0],strlen(params[0]));
			CIBWrite();
			return CMD_SUCCESS;
		}
	} else if (cmdType == GET_CURE_PARAM_COMMAND) {
			sprintf(rsp,"+CWHOSTNAME:\"%s\"\r\n",CIB.hostName);
			return CMD_SUCCESS;
	}
	printf("[%d]:param error!\r\n",__LINE__);
	return CMD_ERROR;
}

int32_t AT_Dev_DNS_Processing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 
		
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
		gwifistatus.connAP[0].ipconf.dns_server.u32;
		if (CIB.deviceIpConfig.devStaIpCfg.dnsEN == 0) {
			sprintf(rsp,"+CIPDNS_DEF:%d,\"%d.%d.%d.%d\"\r\n",CIB.deviceIpConfig.devStaIpCfg.dnsEN,\
				gwifistatus.connAP[0].ipconf.dns_server.u8[0],\
				gwifistatus.connAP[0].ipconf.dns_server.u8[1],\
				gwifistatus.connAP[0].ipconf.dns_server.u8[2],\
				gwifistatus.connAP[0].ipconf.dns_server.u8[3]);
		} else {
			sprintf(rsp,"+CIPDNS_DEF:%d,\"%d.%d.%d.%d\"\r\n",CIB.deviceIpConfig.devStaIpCfg.dnsEN,\
				CIB.deviceIpConfig.devStaIpCfg.dns.u8[0],\
				CIB.deviceIpConfig.devStaIpCfg.dns.u8[1],\
				CIB.deviceIpConfig.devStaIpCfg.dns.u8[2],\
				CIB.deviceIpConfig.devStaIpCfg.dns.u8[3]);
		}
		return CMD_SUCCESS;
	}
	printf("[%d]:param error!\r\n",__LINE__);
	return CMD_ERROR;
}

int32_t AT_Test_SSL_Processing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 
		
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == ACTION_COMMAND) {
		//ssl_test_init();
		app_ssl_create();
		return CMD_SUCCESS;
	}
	printf("[%d]:param error!\r\n",__LINE__);
	return CMD_ERROR;
}

int32_t AT_Cfg_SendIp_Processing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	uint32_t port = 0;
	uip_ip4addr_t ipaddr;
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		port = atoi(params[2]);
		if ((port <= 0) || (port > 0xffff)) {
			printf("[%d]:prot error!\r\n",__LINE__);
			return CMD_ERROR;
		}
		if (memcmp(params[0],"TCP",strlen("TCP")) == 0) {
			if (deviceCommMsg.commTcp.magic != DEV_MAGIC) {
				memset(&(deviceCommMsg.commTcp),0,sizeof(comm_t));
				ip2int(params[1],&(ipaddr));
				deviceCommMsg.commTcp.ip    = ipaddr;
				deviceCommMsg.commTcp.port  = port;
				if (app_tcp_create(&(deviceCommMsg.commTcp)) != 1) {
					printf("[%d]:tcp communication create failure!\r\n");
					return CMD_NO_RESPONSE;
				}
				return CMD_NO_RESPONSE;
			} else {
				app_uart_send("ALREADY CONNECTED\r\n",strlen("ALREADY CONNECTED\r\n"));
				return CMD_NO_RESPONSE;
			}
		} else if (memcmp(params[0],"UDP",strlen("UDP")) == 0) {
			if (deviceCommMsg.commUdp.magic != DEV_MAGIC) {
				ip2int(params[1],&(ipaddr));
				memset(&(deviceCommMsg.commUdp),0,sizeof(comm_t));
				//deviceCommMsg.commUdp.id = create_udp_communication();
				if (deviceCommMsg.commUdp.id != -1) {
					deviceCommMsg.commUdp.magic = DEV_MAGIC;
					deviceCommMsg.commUdp.ip    = ipaddr;
					deviceCommMsg.commUdp.port  = port;
					return CMD_SUCCESS;
				}
			} else {
				app_uart_send("ALREADY CONNECTED\r\n",strlen("ALREADY CONNECTED\r\n"));
				return CMD_NO_RESPONSE;
			}
		} else if (memcmp(params[0],"SSL",strlen("SSL")) == 0) {
			if (deviceCommMsg.commSsl.magic != DEV_MAGIC) {
				memset(&(deviceCommMsg.commSsl),'\0',sizeof(comm_t));
				//deviceCommMsg.commSsl.ip    = ipaddr;
				char str[32] = {'\0'};
				char ipStr[32] = {'\0'};
				struct hostent *host;
    			if((host=gethostbyname(params[1])) == NULL) {
					printf("[%d]:get host name failure!\r\n",__LINE__);
        			return CMD_ERROR;
   				}
				inet_ntop((host->h_addrtype),(host->h_addr_list[0]),str,sizeof(str));
				ip2int(str,deviceCommMsg.commSsl.ip.u8);
				memcpy(deviceCommMsg.commSsl.domainName,params[1],strlen(params[1]));
				deviceCommMsg.commSsl.port  = port;
				app_ssl_create();
				return CMD_NO_RESPONSE;
			} else {
				app_uart_send("ALREADY CONNECTED\r\n",strlen("ALREADY CONNECTED\r\n"));
				return CMD_NO_RESPONSE;
			}
		}
	}
	printf("[%d]:param error!\r\n",__LINE__);
	return CMD_ERROR;
}

int32_t AT_Send_Data_Processing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == SET_PARAM_COMMAND) {
		at_command_param_parse(pBuf,len,params);
		len = atoi(params[0]);
		if (len) {
			if (params[1][0] == 0) {
				if (deviceCommMsg.commTcp.magic == DEV_MAGIC) {
					deviceCommMsg.commTcp.sendBufLen = len;
					app_uart_send("\r\n>",strlen("\r\n>"));
					return CMD_NO_RESPONSE;
				} else if (deviceCommMsg.commSsl.magic == DEV_MAGIC) {
					deviceCommMsg.commSsl.sendBufLen = len;
					app_uart_send("\r\n>",strlen("\r\n>"));
					return CMD_NO_RESPONSE;
				} else if (deviceCommMsg.commUdp.magic == DEV_MAGIC) {
					deviceCommMsg.commUdp.sendBufLen = len;
					app_uart_send("\r\n>",strlen("\r\n>"));
					return CMD_NO_RESPONSE;
				} else {
					printf("[%d]:param[0] error!\r\n",__LINE__);
				}
			} else {
				printf("[%d]:param[1] is't NULL!\r\n",__LINE__);
			} 
		} else {
			printf("[%d]:length error!\r\n",__LINE__);
		}
	}
	return CMD_ERROR;
}
int32_t AT_Close_Comm_Processing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp)
{
	atcmd_type_e cmdType = DEFAULT;
	uint8_t params[AT_FUNC_PARAMS_MAX_NUM][AT_FUNC_PARAMS_MAX_LEN] = {0}; 
	
	cmdType = atcmd_type_get(pBuf);
	if (cmdType == ACTION_COMMAND) {
		if (deviceCommMsg.commTcp.magic == DEV_MAGIC) {
			app_tcp_close(&(deviceCommMsg.commTcp));
			deviceCommMsg.commTcp.magic = 0;
			return CMD_SUCCESS;
		} else if (deviceCommMsg.commUdp.magic == DEV_MAGIC) {
			//close_comm_udp();
			deviceCommMsg.commUdp.magic = 0;
			return CMD_SUCCESS;
		} else if (deviceCommMsg.commSsl.magic == DEV_MAGIC) {
			app_ssl_close();
			deviceCommMsg.commSsl.magic = 0;
			return CMD_SUCCESS;
		} else {
			printf("[%d]:all interface has been closed!\r\n",__LINE__);
		}
	}
	printf("[%d]:param error!\r\n",__LINE__);
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










