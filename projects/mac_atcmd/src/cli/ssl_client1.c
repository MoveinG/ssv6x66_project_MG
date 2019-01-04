/*
 *  SSL client demonstration program
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if 0
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
#include "ssl_client1.h"

#define SERVER_PORT   "443"
#define SERVER_IP     "114.143.22.138"   //"114.143.22.138"
#define SERVER_NAME   "test.mosambee.in"   //"test.mosambee.in"
#define GET_REQUEST   "GET / HTTP/1.1\r\ntest.mosambee.in\r\n\r\n"

#define DEBUG_LEVEL 2

const char ssl_cas_pem[] =
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


const uint32_t ssl_cas_pem_len = sizeof(ssl_cas_pem);


void ssl_test_init(void);



static void my_debug(void *ctx, int level,const char *file, int line,const char *str)
{
	printf("%s:%04d: %s", file, line, str );
}

uint8_t read_buf[4096];
int read_buf_len;



mbedtls_net_context 	 serverIdTest;
mbedtls_ssl_context 	 sslTest;
mbedtls_ssl_config		 confTest;
mbedtls_x509_crt		 cacertTest;
mbedtls_entropy_context  entropyTest;
mbedtls_ctr_drbg_context ctrDrbgTest;


int ssl_init(uint32_t ip,uint32_t port)
{
	printf("\r\n-----------[%d]:%s------------\r\n",__LINE__,__func__);

	int ret;
	const char *pers = "app ssl init";

	
	mbedtls_net_init( &serverIdTest );
    mbedtls_ssl_init( &sslTest );
    mbedtls_ssl_config_init( &confTest );
    mbedtls_x509_crt_init( &cacertTest );
    mbedtls_ctr_drbg_init( &ctrDrbgTest );
	mbedtls_entropy_init( &entropyTest );

	mbedtls_debug_set_threshold(DEBUG_LEVEL);

	ret = mbedtls_ctr_drbg_seed(&ctrDrbgTest,mbedtls_entropy_func,&entropyTest,(const unsigned char *)pers,strlen( pers ));
	if (ret != 0) {
		return -1;
    }
	
	ret = mbedtls_x509_crt_parse(&cacertTest,(const unsigned char *)ssl_cas_pem,ssl_cas_pem_len );
    if (ret < 0) {
        printf( " failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret );
        return -1;
    }
	
	ret = mbedtls_net_connect(&serverIdTest, SERVER_IP,SERVER_PORT, MBEDTLS_NET_PROTO_TCP);
    if (ret != 0) {
        printf( " failed\n  ! mbedtls_net_connect returned %d\n\n", ret );
        return -1;
    }

    if (( ret = mbedtls_ssl_config_defaults(&confTest,MBEDTLS_SSL_IS_CLIENT,MBEDTLS_SSL_TRANSPORT_STREAM,MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        printf( " failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret );
        return -1;
    }

	mbedtls_ssl_conf_authmode( &confTest, MBEDTLS_SSL_VERIFY_OPTIONAL );
    mbedtls_ssl_conf_ca_chain( &confTest, &cacertTest, NULL );
    mbedtls_ssl_conf_rng( &confTest, mbedtls_ctr_drbg_random, &ctrDrbgTest );
    mbedtls_ssl_conf_dbg( &confTest, my_debug, stdout );
    if (( ret = mbedtls_ssl_setup( &sslTest, &confTest )) != 0 ) {
        printf( " failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret );
        return -1;
    }
    if (( ret = mbedtls_ssl_set_hostname( &sslTest, SERVER_NAME )) != 0 ) {
        printf( " failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret );
        return -1;
    }
    mbedtls_ssl_set_bio( &sslTest, &serverIdTest, mbedtls_net_send, mbedtls_net_recv, NULL );
	
	while ((ret = mbedtls_ssl_handshake( &sslTest )) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            printf( " failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret );
            return -1;
        }
    }

    if (( ret = mbedtls_ssl_get_verify_result( &sslTest ) ) != 0) {
        char vrfy_buf[512];
        mbedtls_x509_crt_verify_info( vrfy_buf, sizeof( vrfy_buf ), "  ! ", ret);
		return -1;
    }
	
	return 0;
}


void ssl_func( void *pdata )
{
    int ret, len;
	uint32_t flags;
    unsigned char buf[1024];

	ssl_init(0,0);


    printf( "  > Write to server:" );
    len = sprintf( (char *) buf, GET_REQUEST );
    while ((ret = mbedtls_ssl_write( &sslTest, buf, len)) <= 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret);
            goto exit;
        }
    }

    len = ret;
    printf( " %d bytes written\n\n%s", len, (char *) buf );

    printf( "  < Read from server:" );
    do {
        read_buf_len = sizeof( read_buf ) - 1;
        memset( read_buf, 0, sizeof( read_buf ) );
        ret = mbedtls_ssl_read( &sslTest, read_buf, read_buf_len );

        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
            continue;

        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
            break;

        if (ret < 0) {
            printf( "failed\n  ! mbedtls_ssl_read returned %d\n\n", ret );
            break;
        }

        if (ret == 0) {
            printf( "\n\nEOF\n\n" );
            break;
        }

        read_buf_len = ret;
        printf( " %d bytes read\n", read_buf_len );
		uint16_t cnt = read_buf_len / 255 + 1;
		for (int i = 0;i < cnt;i++) {
            printf("%s",read_buf+i*255);
		}
		printf("\r\n\r\n");
    }
    while( 1 );

    mbedtls_ssl_close_notify( &sslTest );

exit:
	printf("delete ssl test task.\r\n");
    mbedtls_net_free( &serverIdTest );
    mbedtls_x509_crt_free( &cacertTest );
    mbedtls_ssl_free( &sslTest );
    mbedtls_ssl_config_free( &confTest );
    mbedtls_ctr_drbg_free( &ctrDrbgTest );
    mbedtls_entropy_free( &entropyTest );
    OS_TaskDelete(NULL);
}


void ssl_test_init(void)
{
    OS_TaskCreate(ssl_func, "ssl_func", 4096, NULL, 2, NULL);
}


#endif

