

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
int32_t AT_UartProcessing(uint8_t *pBuf,uint16_t len,uint8_t max_para,uint8_t *rsp)
{


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

  return 0;
}










