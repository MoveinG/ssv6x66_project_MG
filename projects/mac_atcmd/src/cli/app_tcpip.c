#include "soc_types.h"
#include "atcmd.h"
#include "error.h"
#include "Cabrio-conf.h"
#include "netstack_def.h"
#include "ssv_lib.h"
#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "osal.h"
#include <string.h>
#include "app_tcpip.h"
#include "at_cmd.h"




/************************************************/
#define SOCKET_RECV_TIMEOUT            100
#define SOCKET_BUF_SIZE_MAX            256
#define READ_SSL_BUF_LEN_MAX           4096
#define SEND_SSL_BUF_LEN_MAX           1024

#define APP_DEBUG_LOG_ENABLE           2
#define APP_DEBUG_LOG_DISABLE          0

#define SERVER_PORT   "443"
#define SERVER_IP     "14.215.177.38"   //"114.143.22.138"
#define SERVER_NAME   "www.baidu.com"   //"test.mosambee.in"
#define GET_REQUEST   "GET / HTTP/1.1\r\nHost:www.baidu.com\r\n\r\n"



/************************************************/
OsTaskHandle sslTaskHandle;
OsTaskHandle tcpTaskHandle;

char readBuf[READ_SSL_BUF_LEN_MAX] = {0};
char sendbuf[SEND_SSL_BUF_LEN_MAX] = {0};


mbedtls_ssl_context 	 ssl;
mbedtls_net_context 	 serverId;
mbedtls_ssl_config		 conf;
mbedtls_x509_crt		 cacert;
mbedtls_entropy_context  entropy;
mbedtls_ctr_drbg_context ctrDrbg;


const char app_ssl_cas_pem[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIIEADCCAuigAwIBAgIBADANBgkqhkiG9w0BAQUFADBjMQswCQYDVQQGEwJVUzEh\r\n"
"MB8GA1UEChMYVGhlIEdvIERhZGR5IEdyb3VwLCBJbmMuMTEwLwYDVQQLEyhHbyBE\r\n"
"YWRkeSBDbGFzcyAyIENlcnRpZmljYXRpb24gQXV0aG9yaXR5MB4XDTA0MDYyOTE3\r\n"
"MDYyMFoXDTM0MDYyOTE3MDYyMFowYzELMAkGA1UEBhMCVVMxITAfBgNVBAoTGFRo\r\n"
"ZSBHbyBEYWRkeSBHcm91cCwgSW5jLjExMC8GA1UECxMoR28gRGFkZHkgQ2xhc3Mg\r\n"
"MiBDZXJ0aWZpY2F0aW9uIEF1dGhvcml0eTCCASAwDQYJKoZIhvcNAQEBBQADggEN\r\n"
"ADCCAQgCggEBAN6d1+pXGEmhW+vXX0iG6r7d/+TvZxz0ZWizV3GgXne77ZtJ6XCA\r\n"
"PVYYYwhv2vLM0D9/AlQiVBDYsoHUwHU9S3/Hd8M+eKsaA7Ugay9qK7HFiH7Eux6w\r\n"
"wdhFJ2+qN1j3hybX2C32qRe3H3I2TqYXP2WYktsqbl2i/ojgC95/5Y0V4evLOtXi\r\n"
"EqITLdiOr18SPaAIBQi2XKVlOARFmR6jYGB0xUGlcmIbYsUfb18aQr4CUWWoriMY\r\n"
"avx4A6lNf4DD+qta/KFApMoZFv6yyO9ecw3ud72a9nmYvLEHZ6IVDd2gWMZEewo+\r\n"
"YihfukEHU1jPEX44dMX4/7VpkI+EdOqXG68CAQOjgcAwgb0wHQYDVR0OBBYEFNLE\r\n"
"sNKR1EwRcbNhyz2h/t2oatTjMIGNBgNVHSMEgYUwgYKAFNLEsNKR1EwRcbNhyz2h\r\n"
"/t2oatTjoWekZTBjMQswCQYDVQQGEwJVUzEhMB8GA1UEChMYVGhlIEdvIERhZGR5\r\n"
"IEdyb3VwLCBJbmMuMTEwLwYDVQQLEyhHbyBEYWRkeSBDbGFzcyAyIENlcnRpZmlj\r\n"
"YXRpb24gQXV0aG9yaXR5ggEAMAwGA1UdEwQFMAMBAf8wDQYJKoZIhvcNAQEFBQAD\r\n"
"ggEBADJL87LKPpH8EsahB4yOd6AzBhRckB4Y9wimPQoZ+YeAEW5p5JYXMP80kWNy\r\n"
"OO7MHAGjHZQopDH2esRU1/blMVgDoszOYtuURXO1v0XJJLXVggKtI3lpjbi2Tc7P\r\n"
"TMozI+gciKqdi0FuFskg5YmezTvacPd+mSYgFFQlq25zheabIZ0KbIIOqPjCDPoQ\r\n"
"HmyW74cNxA9hi63ugyuV+I6ShHI56yDqg+2DzZduCLzrTia2cyvk0/ZM/iZx4mER\r\n"
"dEr/VxqHD3VILs9RaRegAhJhldXRQLIQTO7ErBBDpqWeCtWVYpoNz4iCxTIM5Cuf\r\n"
"ReYNnyicsbkqWletNw+vHX/bvZ8=\r\n"
"-----END CERTIFICATE-----\r\n";

const uint32_t app_ssl_cas_pem_len = sizeof(app_ssl_cas_pem);





/************************************************/
int  app_tcp_create(comm_t* comm_tcp);
int  app_tcp_send(int socketId, char* buf, int len);
int  app_tcp_close(comm_t* comm_tcp);
int  app_ssl_create(void);
int  app_ssl_send(char* buf, int len);
void app_ssl_close(void);



/************************************************/
static void app_debug(void *ctx, int level,const char *file, int line,const char *str)
{
	printf("%s:%04d: %s", file, line, str );
}


int app_udp_create(int ip, int port)
{
	int socketId = -1;
	return socketId;
}

int app_udp_send(int socketId, char* buf, int len)
{
	int ret = -1;
	ret = send(socketId, buf, len, 0);
	return ret;
}

int app_udp_close(int socketId)
{
	int ret = -1;
	ret = close(socketId);
	return ret;
}

static void app_tcp_task(comm_t *comm_tcp)
{
	/*************** init ***************/
	int timeOut   = 0;
	int ret       = 0;
	char ip[17] = {0};
	char buf[SOCKET_BUF_SIZE_MAX] = {0};
	struct sockaddr_in userSocket;
	
	memset(&userSocket, 0, sizeof(userSocket));
    userSocket.sin_family = AF_INET;
    userSocket.sin_port = htons(comm_tcp->port);
	sprintf(ip,"%d.%d.%d.%d",comm_tcp->ip.u8[0],comm_tcp->ip.u8[1],comm_tcp->ip.u8[2],comm_tcp->ip.u8[3]);
	inet_aton(ip, &userSocket.sin_addr);
    userSocket.sin_len = sizeof(userSocket);

	comm_tcp->id = socket(AF_INET, SOCK_STREAM, 0);
    if (comm_tcp->id < 0) {
        printf("[%d]:failed! tcp to create socket\r\n",__LINE__);
		goto exit;
    }

	ret = connect(comm_tcp->id, (struct sockaddr *) &userSocket, sizeof(userSocket));
    if (ret < 0) {
        printf("[%d]:failed! tcp to connect ret=%d\r\n",__LINE__,ret);
		goto exit;
    }

    timeOut = SOCKET_RECV_TIMEOUT;
    setsockopt(comm_tcp->id, SOL_SOCKET, SO_RCVTIMEO, &timeOut, sizeof(timeOut));

	comm_tcp->magic = DEV_MAGIC;
	app_uart_send("OK\r\n",strlen("OK\r\n"));
	printf("[%d]:create app tcp task ok\r\n",__LINE__);
	/*************** recv ***************/
	while (1) {
		if (comm_tcp->magic == 0) {
			printf("[%d]:tcp magic be clean!\r\n",__LINE__);
			break;
		} 
		ret = recv(comm_tcp->id, buf, SOCKET_BUF_SIZE_MAX,0);
		if (ret > 0) {
			printf("[%d]:tcp recv:%s\r\n",__LINE__,buf);
			app_uart_send(buf,strlen(buf));
			memset(buf,0,SOCKET_BUF_SIZE_MAX);
		} else if (ret == 0) {
			printf("[%d]:tcp be disconnected\r\n",__LINE__);
			goto exit;
		} 
		vTaskDelay(50 / portTICK_RATE_MS);
	}
exit:
	printf("[%d]:delete app tcp task!\r\n",__LINE__);
	comm_tcp->magic = 0;
	close(comm_tcp->id);
	vTaskDelete(tcpTaskHandle);
}

int app_tcp_create(comm_t* comm_tcp)
{
	int ret = -1;
	ret = OS_TaskCreate(app_tcp_task, "app_tcp_task", 1024, comm_tcp, tskIDLE_PRIORITY + 2, &tcpTaskHandle);
	OS_MsDelay(100);
	if (ret != 1) {
		app_uart_send("ERROR\r\n",strlen("ERROR\r\n"));
		printf("[%d]:app tcp task create failure!\r\n",__LINE__);
	} 
	return ret;
}

int app_tcp_send(int socketId, char* buf, int len)
{
	int ret = -1;
	ret = send(socketId, buf, len, 0);
	return ret;
}

int app_tcp_close(comm_t* comm_tcp)
{
	int ret = -1;
	if (comm_tcp->magic == DEV_MAGIC) {
		printf("[%d]:delete app tcp task!\r\n",__LINE__);
		comm_tcp->magic = 0;
		close(comm_tcp->id);
		vTaskDelete(tcpTaskHandle);
	}
	return 0;
}

int app_ssl_init(void)
{
	int ret;
	char ip[17]  = {0};
	char port[6] = {0};
	const char *pers = "app ssl init";

	mbedtls_net_init(&serverId);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_ctr_drbg_init(&ctrDrbg);
	mbedtls_entropy_init(&entropy);
	
	sprintf(ip,"%d.%d.%d.%d",deviceCommMsg.commSsl.ip.u8[0],\
			deviceCommMsg.commSsl.ip.u8[1],\
			deviceCommMsg.commSsl.ip.u8[2],\
			deviceCommMsg.commSsl.ip.u8[3]);
	sprintf(port,"%d",deviceCommMsg.commSsl.port);
	printf("[%d]:ip=%s port=%s domainName=%s\r\n",__LINE__,ip,port,deviceCommMsg.commSsl.domainName);

	mbedtls_debug_set_threshold(APP_DEBUG_LOG_DISABLE);

	ret = mbedtls_ctr_drbg_seed(&ctrDrbg,mbedtls_entropy_func,&entropy,(const unsigned char *)pers,strlen( pers ));
	if (ret != 0) {
		return -1;
    }
	
	ret = mbedtls_x509_crt_parse(&cacert,(const unsigned char *)app_ssl_cas_pem,app_ssl_cas_pem_len);
    if (ret < 0) {
        printf("[%d]:failed! mbedtls_x509_crt_parse returned -0x%x\r\n",__LINE__ ,-ret);
        return -1;
    }
	
	ret = mbedtls_net_connect(&serverId, ip,port, MBEDTLS_NET_PROTO_TCP);
    if (ret != 0) {
        printf("[%d]:failed! mbedtls_net_connect returned %d\r\n",__LINE__, ret);
        return -1;
    }

    if ((ret = mbedtls_ssl_config_defaults(&conf,MBEDTLS_SSL_IS_CLIENT,MBEDTLS_SSL_TRANSPORT_STREAM,MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        printf("[%d]:failed! mbedtls_ssl_config_defaults returned %d\r\n",__LINE__, ret);
        return -1;
    }

	mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctrDrbg);
    mbedtls_ssl_conf_dbg(&conf, app_debug, stdout);
    if ((ret = mbedtls_ssl_setup( &ssl, &conf )) != 0) {
        printf("[%d]:failed! mbedtls_ssl_setup returned %d\r\n",__LINE__, ret);
        return -1;
    }
    if ((ret = mbedtls_ssl_set_hostname(&ssl, deviceCommMsg.commSsl.domainName)) != 0) {
        printf("[%d]:failed! mbedtls_ssl_set_hostname returned %d\r\n",__LINE__,ret);
        return -1;
    }
    mbedtls_ssl_set_bio( &ssl, &serverId, mbedtls_net_send, mbedtls_net_recv, NULL);
	
	while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            printf("[%d]:failed! mbedtls_ssl_handshake returned -0x%x\r\n",__LINE__, -ret);
            return -1;
        }
    }

    if ((ret = mbedtls_ssl_get_verify_result( &ssl)) != 0) {
        char vrfy_buf[512];
        mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", ret);
		return -1;
    }
	
	return 0;
}

void app_ssl_task(void* arg)
{
	/*************** init ***************/
	int ret = -1;
	int len = 0;
	int readBufLen = 0;

	app_ssl_init();

	#if 0
	memset(sendbuf,0,SEND_SSL_BUF_LEN_MAX);
	len = sprintf( (char *) sendbuf, GET_REQUEST );
	while ((ret = mbedtls_ssl_write( &ssl, sendbuf, len)) <= 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret);
            goto exit;
        }
    }
	#endif
	
	deviceCommMsg.commSsl.magic = DEV_MAGIC;
	readBufLen = READ_SSL_BUF_LEN_MAX - 1;
	app_uart_send("OK\r\n",strlen("OK\r\n"));
	printf("[%d]:app ssl task create ok.\r\n",__LINE__);
	/*************** recv ***************/
	while (1) {
		if (deviceCommMsg.commSsl.magic != DEV_MAGIC) {
			printf("[%d]:ssl magic be clean!\r\n",__LINE__);
			break;
		} else if ((ret = mbedtls_ssl_read(&ssl, readBuf, readBufLen)) > 0) {
			readBufLen = ret;
			app_uart_send(readBuf,readBufLen);
			uint16_t cnt = readBufLen / 255 + 1;
			for (int i = 0;i < cnt;i++) {
            	printf("%s",readBuf+i*255);
			}
			memset(readBuf, 0, sizeof(readBuf));
		}
		if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            continue;
        } else if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
        	printf("[%d]:ssl close notify!\r\n",__LINE__);
            break;
        } else if (ret < 0) {
            printf("[%d]:failed! ssl mbedtls_ssl_read returned %d\r\n",__LINE__, ret);
            break;
        }
		//vTaskDelay(50 / portTICK_RATE_MS);
	}
exit:
	printf("[%d]:delete app ssl task!\r\n",__LINE__);
	deviceCommMsg.commSsl.magic = 0;
	mbedtls_ssl_close_notify( &ssl );
	mbedtls_net_free( &serverId );
    mbedtls_x509_crt_free( &cacert );
    mbedtls_ssl_free( &ssl );
    mbedtls_ssl_config_free( &conf );
    mbedtls_ctr_drbg_free( &ctrDrbg );
    mbedtls_entropy_free( &entropy );
	OS_TaskDelete(sslTaskHandle);
}

int app_ssl_create(void)
{
	int ret = -1;
	
	ret = OS_TaskCreate(app_ssl_task, "app_ssl_task", 4096, NULL, 2, &sslTaskHandle);
	OS_MsDelay(100);
	if (ret != 1) {
		app_uart_send("ERROR\r\n",strlen("ERROR\r\n"));
		printf("[%d]:create app ssl task failure!\r\n",__LINE__);
	} 
	return ret;
}

int app_ssl_send(char* buf,int len)
{
	int ret = -1;
	memset(sendbuf,0,SEND_SSL_BUF_LEN_MAX);
	len = sprintf( (char *) sendbuf, buf );
	while ((ret = mbedtls_ssl_write( &ssl, sendbuf, len)) <= 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            printf("[%d]:failed! mbedtls_ssl_write returned %d\r\n",__LINE__, ret);
            return -1;
        }
    }
	return ret;
}

void app_ssl_close(void)
{
	if (deviceCommMsg.commSsl.magic == DEV_MAGIC) {
		printf("[%d]:delete app ssl task!\r\n",__LINE__);
		deviceCommMsg.commSsl.magic = 0;
		mbedtls_ssl_close_notify( &ssl );
		mbedtls_net_free( &serverId );
    	mbedtls_x509_crt_free( &cacert );
    	mbedtls_ssl_free( &ssl );
    	mbedtls_ssl_config_free( &conf );
    	mbedtls_ctr_drbg_free( &ctrDrbg );
    	mbedtls_entropy_free( &entropy );
		OS_TaskDelete(sslTaskHandle);
	}
}






