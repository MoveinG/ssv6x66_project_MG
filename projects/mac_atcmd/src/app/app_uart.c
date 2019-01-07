

#include "fsal.h"
#include "osal.h"
#include "hsuart/hal_hsuart.h"
#include "hsuart/drv_hsuart.h"
#include "app_uart.h"
#include "at_cmd.h"



#define SEND_BUF_LEN_MAX              4096



AppUartRx_t AppUartRxData1/*,AppUartRxData2*/;
AppUartRx_t *AppUartRx;
OsSemaphore AppUartRxSem = NULL;
char sendBuf[SEND_BUF_LEN_MAX] = {0};




void app_uart_int(void);
void app_uart_set_format(uint32_t baudrate,uint8_t databits,uint8_t stopbit,uint8_t paritybit);
void app_uart_send(uint8_t *data,int32_t len);
void app_uart_isr(void);
void app_uart_rx_task(void *pdata);






/*****************************************************************************
*
* app_uart_int
*
* @param null
*        
* @return null
*
* @brief high speed uart int(gpio5--RTS,gpio6--RXD,gpio7--TXD,gpio9--CTS,"custom_io_hal.h")
*
******************************************************************************/
void app_uart_int(void)
{
	memset(&AppUartRxData1,0,sizeof(AppUartRx_t));
	AppUartRx = &AppUartRxData1;
    drv_hsuart_init ();
    drv_hsuart_sw_rst ();
    drv_hsuart_set_fifo (HSUART_INT_RX_FIFO_TRIG_LV_16);
    drv_hsuart_set_hardware_flow_control (16, 24);

    //drv_hsuart_set_format(115200, HSUART_WLS_8,HSUART_STB_1 , HSUART_PARITY_DISABLE);
    printf("[%d]:app uart band is %d.\r\n",__LINE__,CIB.uartcfg.baudrate);
    app_uart_set_format(CIB.uartcfg.baudrate,CIB.uartcfg.databits,CIB.uartcfg.stopbit,CIB.uartcfg.datapaity);

	drv_hsuart_register_isr(HSUART_RX_DATA_READY_IE, app_uart_isr);
	if(OS_SemInit(&AppUartRxSem, 1, 0) == OS_SUCCESS)
	{
        OS_TaskCreate(app_uart_rx_task, "app uart rx task", 1024, NULL, APP_UART_RX_TASK_PRIORITY, NULL);
	}
	else
	{
       printf("[%d]:app uart rx sem init fail!\r\n",__LINE__);
	}
}

/*****************************************************************************
*
* app_uart_send
*
* @param:*data--uart send data
*        len--data length
* @return:null
*
* @brief:application uart send
*
******************************************************************************/
void app_uart_set_format(uint32_t baudrate,uint8_t databits,uint8_t stopbit,uint8_t paritybit)
{
    HSUART_WLS_E wls;
	HSUART_STB_E stb;
	HSUART_PARITY_E parity;

    switch(databits)
   	{
        case 5:
          wls = HSUART_WLS_5;
		break;

		case 6:
		  wls = HSUART_WLS_6;

		break;

		case 7:
          wls = HSUART_WLS_7;
		break;

		case 8:
		  wls = HSUART_WLS_8;	
		break;
		
        default:
		  wls = HSUART_WLS_8;	
        break;
    }
	switch(stopbit)
	{
        case 1:
          stb = HSUART_STB_1;
		break;

		case 2:
          stb = HSUART_STB_2;
		break;

		default:
		  stb = HSUART_STB_1;	
        break;
	}
	switch(paritybit)
	{
        case 0:
          parity = HSUART_PARITY_DISABLE;
		break;

		case 1:
          parity = HSUART_PARITY_ODD;
		break;

		case 2:
          parity = HSUART_PARITY_EVEN;
		break;

		default:
          parity = HSUART_PARITY_DISABLE;
		break;

	}
    drv_hsuart_set_format((int32_t)baudrate, wls,stb , parity);
}


/*****************************************************************************
*
* app_uart_send
*
* @param:*data--uart send data
*        len--data length
* @return:null
*
* @brief:application uart send
*
******************************************************************************/
void app_uart_send(uint8_t *data,int32_t len)
{
  drv_hsuart_write_fifo(data,len, HSUART_BLOCKING);
}

/*****************************************************************************
*
* app_uart_isr
*
* @param:null
*        
* @return:null
*
* @brief:application uart isr function
*
******************************************************************************/
void app_uart_isr(void)
{
    
    uint32_t len;

    len = drv_hsuart_read_fifo(&AppUartRx->buf[AppUartRx->recv_len], (APP_UART_BUF_MAX - AppUartRx->recv_len), HSUART_NON_BLOCKING);
	if(len > 0)
	{
	   AppUartRx->recv_len += len;
	   if(AppUartRx->recv_len == APP_UART_BUF_MAX)
	   {
          OS_SemSignal(AppUartRxSem);
		  //printf("app uart buf full\r\n");
	   }
	}
	else
	{
       printf("hsuart isr\r\n");
	}
}


void app_tranmission_mode(char* buf,int len)
{
	static int totalLen = 0;

	if (deviceCommMsg.commSsl.sendBufLen != 0) {
		if (deviceCommMsg.commSsl.magic == DEV_MAGIC) {
			totalLen += len;
			strcat(sendBuf,buf);
			if ((totalLen >= deviceCommMsg.commSsl.sendBufLen) ||\
				(buf[len] == '\0')) {
				if (app_ssl_send(sendBuf,((totalLen < deviceCommMsg.commSsl.sendBufLen)?totalLen:deviceCommMsg.commSsl.sendBufLen)) != -1) {
					app_uart_send("SEND OK\r\n",strlen("SEND OK\r\n"));
				} else {
					app_uart_send("SEND FAIL\r\n",strlen("SEND FAIL\r\n"));
				}
				totalLen = 0;
				deviceCommMsg.commSsl.sendBufLen = 0;
			}
		} else {
			deviceCommMsg.commSsl.sendBufLen = 0;
			printf("[%d]:ssl connection invalid!\r\n",__LINE__);
			app_uart_send("ERROR\r\n",strlen("ERROR\r\n"));
		}
	} else if (deviceCommMsg.commTcp.sendBufLen != 0) {
		if (deviceCommMsg.commTcp.magic == DEV_MAGIC) {
			totalLen += len;
			strcat(sendBuf,buf);
			if ((totalLen >= deviceCommMsg.commTcp.sendBufLen) ||\
				(buf[len] == '\0')) {
				if (app_tcp_send(deviceCommMsg.commTcp.id,sendBuf,((totalLen < deviceCommMsg.commTcp.sendBufLen)?totalLen:deviceCommMsg.commTcp.sendBufLen)) != -1) {
					app_uart_send("SEND OK\r\n",strlen("SEND OK\r\n"));
				} else {
					app_uart_send("SEND FAIL\r\n",strlen("SEND FAIL\r\n"));
				}
				totalLen = 0;
				deviceCommMsg.commTcp.sendBufLen = 0;
			}
		} else {
			deviceCommMsg.commTcp.sendBufLen = 0;
			printf("[%d]:tcp connection invalid!\r\n",__LINE__);
			app_uart_send("ERROR\r\n",strlen("ERROR\r\n"));
		}
	}else if (deviceCommMsg.commUdp.sendBufLen != 0) {
		totalLen = 0;
		memset(sendBuf,0,SEND_BUF_LEN_MAX);
		deviceCommMsg.commUdp.sendBufLen = 0;
		//null
	} else {
		printf("[%d]:please set the sending length first!\r\n",__LINE__);
		if (strlen(sendBuf) != 0) {
			totalLen = 0;
			memset(sendBuf,0,SEND_BUF_LEN_MAX);
		}
	}
}


/*****************************************************************************
*
* app_uart_rx_task
*
* @param:*pdata--null
*        
* @return:null
*
* @brief:appliction uart rx task
*
******************************************************************************/
void app_uart_rx_task(void *pdata)
{
   	OS_STATUS ret;
   	uint32_t last_recv_len = 0;
   	bool rx_full_flg = false;
    app_uart_send("ready\r\n",strlen("ready\r\n"));
   while(1)
   {
      ret = OS_SemWait(AppUartRxSem, APP_UART_RX_TIMEOUT/OS_TICK_RATE_MS);
      if(ret == OS_TIMEOUT)
      {
         if(AppUartRx->recv_len > 0)
         {
            //if(AppUartRx->recv_len == last_recv_len)
            {
               printf("[%d]:uart rx:%s\r\n",__LINE__,AppUartRx->buf);
               if (!(memcmp(AppUartRx->buf,AT_CMD_PREFIX,strlen(AT_CMD_PREFIX))) &&\
			   	(rx_full_flg == false)) {
			   		AtCmdMode = ATCMD_MODE;
					AppUartProcessing(AppUartRx->buf,AppUartRx->recv_len);
			   } else if (memcmp((AppUartRx->buf),"+++",3) != 0) {
			   		app_tranmission_mode(AppUartRx->buf,AppUartRx->recv_len);
			   }
               AppUartRx->recv_len = 0;
			   memset(AppUartRx->buf,0,APP_UART_BUF_MAX);
            }
			//last_recv_len = AppUartRx->recv_len;
		 }
		 rx_full_flg = false;
      }
	  else if(ret = OS_SUCCESS)
	  {
	      AppUartProcessing(AppUartRx->buf,AppUartRx->recv_len);
          printf("[%d]:app uart(full):%d\r\n",__LINE__,AppUartRx->recv_len);
          AppUartRx->recv_len = 0;
		  rx_full_flg = true;
	  }
   }

}











