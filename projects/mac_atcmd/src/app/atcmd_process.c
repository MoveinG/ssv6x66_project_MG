

#include "stdint.h"
#include "atcmd_process.h"
#include "at_cmd.h"



int32_t AT_UartProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_ApStaProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_RebootProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_AppVerProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_ExitProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);
int32_t AT_ResetProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp);


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










