#ifndef ATCMD_PROCESS_H
#define ATCMD_PROCESS_H





int32_t AT_UartProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_ApStaProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_RebootProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_AppVerProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_ExitProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_ResetProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);




















#endif
