#ifndef ATCMD_PROCESS_H
#define ATCMD_PROCESS_H




int32_t AT_UartProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_ApStaProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_RebootProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_AppVerProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_ExitProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_ResetProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_ScanProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_SetModeProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_GetWifiStatusProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_ConnectApProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_DisconnectApProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
//int32_t AT_SetApParamProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
//int32_t AT_SetWifiParamProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
//int32_t AT_GetWifiParamProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
//int32_t AT_SetApNameProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_AutoConnectProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_DHCP_Processing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_APIP_ConfigProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_STAIP_ConfigProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_Host_Name_Processing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_Dev_DNS_Processing(uint8_t *pBuf,uint16_t len,uint8_t paraNum,uint8_t *rsp);



#endif




