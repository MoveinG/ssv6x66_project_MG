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

#define SERVER_PORT "443"
#define SERVER_IP    "14.215.177.38"   //"114.143.22.138"
#define SERVER_NAME   "www.baidu.com"   //"test.mosambee.in"
#define GET_REQUEST "GET / HTTP/1.1\r\nHost:www.baidu.com\r\n\r\n"

#define DEBUG_LEVEL 0


/*
const char ssl_test_cas_pem[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIIGrzCCBZegAwIBAgIIccFup0l5XQowDQYJKoZIhvcNAQELBQAwgbQxCzAJBgNV\r\n"
"BAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQHEwpTY290dHNkYWxlMRow\r\n"
"GAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjEtMCsGA1UECxMkaHR0cDovL2NlcnRz\r\n"
"LmdvZGFkZHkuY29tL3JlcG9zaXRvcnkvMTMwMQYDVQQDEypHbyBEYWRkeSBTZWN1\r\n"
"cmUgQ2VydGlmaWNhdGUgQXV0aG9yaXR5IC0gRzIwHhcNMTgwNTE1MDkwODA5WhcN\r\n"
"MjAwNTE1MDkwODA5WjA7MSEwHwYDVQQLExhEb21haW4gQ29udHJvbCBWYWxpZGF0\r\n"
"ZWQxFjAUBgNVBAMMDSoubW9zYW1iZWUuaW4wggEiMA0GCSqGSIb3DQEBAQUAA4IB\r\n"
"DwAwggEKAoIBAQCxpBH5sxESpszoClsksS54jkDLwFxLIl6mmZAzWZ56G5aKvtiK\r\n"
"s0uuQ2yp2nGbAE2GtGSMwGark3dpZdg9oyferDAmmAiiPyBha8utkRMUiwVI5fl0\r\n"
"zpVDfv8fVoREjyuP7ERSL3NMz+slhki0ofgujj1BdFrAokBYfD8B3JwS1LnC2PMj\r\n"
"wWDj5vgdsmMfupxzwsIs9s/S7xkJ0htGjz7ecxrTQwjPFTeMts93H9z+nJVKi/1p\r\n"
"mNzpNvpjtRNSaeyRqhQgNR633fRdMBoY6L2tzhf2nUeWcATd8x7+x5irDCMkNsq3\r\n"
"6ow1k7J0Ii9EnA6SSvspDOEiVnr+lKNmdtX/AgMBAAGjggM7MIIDNzAMBgNVHRMB\r\n"
"Af8EAjAAMB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjAOBgNVHQ8BAf8E\r\n"
"BAMCBaAwNwYDVR0fBDAwLjAsoCqgKIYmaHR0cDovL2NybC5nb2RhZGR5LmNvbS9n\r\n"
"ZGlnMnMxLTgyOS5jcmwwXQYDVR0gBFYwVDBIBgtghkgBhv1tAQcXATA5MDcGCCsG\r\n"
"AQUFBwIBFitodHRwOi8vY2VydGlmaWNhdGVzLmdvZGFkZHkuY29tL3JlcG9zaXRv\r\n"
"cnkvMAgGBmeBDAECATB2BggrBgEFBQcBAQRqMGgwJAYIKwYBBQUHMAGGGGh0dHA6\r\n"
"Ly9vY3NwLmdvZGFkZHkuY29tLzBABggrBgEFBQcwAoY0aHR0cDovL2NlcnRpZmlj\r\n"
"YXRlcy5nb2RhZGR5LmNvbS9yZXBvc2l0b3J5L2dkaWcyLmNydDAfBgNVHSMEGDAW\r\n"
"gBRAwr0njsw0gzCiM9f7bLPwtCyAzjAlBgNVHREEHjAcgg0qLm1vc2FtYmVlLmlu\r\n"
"ggttb3NhbWJlZS5pbjAdBgNVHQ4EFgQU2tD41PPWpfjQ+R6CfmQVKwF3nhgwggF/\r\n"
"BgorBgEEAdZ5AgQCBIIBbwSCAWsBaQB3AKS5CZC0GFgUh7sTosxncAo8NZgE+Rvf\r\n"
"uON3zQ7IDdwQAAABY2MPc+sAAAQDAEgwRgIhAJLbgoCZHoGkE/qHS8YzM5trs8wv\r\n"
"t+fDEPLpCYbprd2RAiEAphSSTz7BbhT63/vDdJTcK4UHxdfjIIGEJLQ69SjwBzsA\r\n"
"dgDuS723dc5guuFCaR+r4Z5mow9+X7By2IMAxHuJeqj9ywAAAWNjD3VjAAAEAwBH\r\n"
"MEUCIQCFj9jBrfPSXbCpNScT4Vhpfdqb+DeiMrPR7wkUROOGRgIgXDnzsZpl7UnX\r\n"
"ZBW63W92NNNQASvPdgSvze7TRnkQdJUAdgBep3P531bA57U2SH3QSeAyepGaDISh\r\n"
"EhKEGHWWgXFFWAAAAWNjD3aUAAAEAwBHMEUCIDceESeMg/ovDO0RsWRgCsou2r6Y\r\n"
"0HR57gQ0VrepXla0AiEA/okP7s7TkVt//Re11lUOfLPcGsjDl+ufv/aWU/zO5BMw\r\n"
"DQYJKoZIhvcNAQELBQADggEBAFndB63uds+rnGC5zJ5Ha5+UgpcCASQm17JH8BJZ\r\n"
"uFQSTf6eLqLpgxMSKn+UTVFKb/z6EMTWn6pUXl7kBetRRU2prIGQD/uAKTEs4a2E\r\n"
"5c5YFbAtJbNJfmGEiLQTq5Wx+lHh02syvF+YLLLrfAGLINDHsABUfniL4TwC5mdi\r\n"
"oHSdHElm0AlIyUlYXXI5HC1tnNwwrEr4UPIGyD4nFmA5i/Md1c83XAjkx86n0zUo\r\n"
"yI3bjlpY7rqstSHl+zGrYy+5TeWWZBSBX5y1jGDp1yJTrxcIZ5AGNK+DHRrRU2Cx\r\n"
"H5ONOex5Qe5fXvGR/lMGo/KK5Rz9CwNJbKduO1yFaG2c5kc=\r\n"
"-----END CERTIFICATE-----\r\n";
*/
const char ssl_test_cas_pem[] =
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


const uint32_t ssl_test_cas_pem_len = sizeof(ssl_test_cas_pem);


void ssl_test_init(void);



static void my_debug(void *ctx, int level,const char *file, int line,const char *str)
{
	printf("%s:%04d: %s", file, line, str );
}

uint8_t read_buf[4096];
int read_buf_len;



mbedtls_net_context 	 serverId;
mbedtls_ssl_context 	 ssl;
mbedtls_ssl_config		 conf;
mbedtls_x509_crt		 cacert;
mbedtls_entropy_context  entropy;
mbedtls_ctr_drbg_context ctrDrbg;

int app_ssl_init(uint32_t ip,uint32_t port)
{
	printf("\r\n-----------[%d]:%s------------\r\n",__LINE__,__func__);

	int ret;
	const char *pers = "app ssl init";
	
	mbedtls_net_init( &serverId );
    mbedtls_ssl_init( &ssl );
    mbedtls_ssl_config_init( &conf );
    mbedtls_x509_crt_init( &cacert );
    mbedtls_ctr_drbg_init( &ctrDrbg );
	mbedtls_entropy_init( &entropy );

	ret = mbedtls_ctr_drbg_seed(&ctrDrbg,mbedtls_entropy_func,&entropy,(const unsigned char *)pers,strlen( pers ));
	if (ret != 0) {
		return -1;
    }
	
	ret = mbedtls_x509_crt_parse(&cacert,(const unsigned char *)ssl_test_cas_pem,ssl_test_cas_pem_len );
    if (ret < 0) {
        printf( " failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret );
        return -1;
    }
	
	ret = mbedtls_net_connect(&serverId, SERVER_IP,SERVER_PORT, MBEDTLS_NET_PROTO_TCP);
    if (ret != 0) {
        printf( " failed\n  ! mbedtls_net_connect returned %d\n\n", ret );
        return -1;
    }

    if (( ret = mbedtls_ssl_config_defaults(&conf,MBEDTLS_SSL_IS_CLIENT,MBEDTLS_SSL_TRANSPORT_STREAM,MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        printf( " failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret );
        return -1;
    }

	mbedtls_ssl_conf_authmode( &conf, MBEDTLS_SSL_VERIFY_OPTIONAL );
    mbedtls_ssl_conf_ca_chain( &conf, &cacert, NULL );
    mbedtls_ssl_conf_rng( &conf, mbedtls_ctr_drbg_random, &ctrDrbg );
    mbedtls_ssl_conf_dbg( &conf, my_debug, stdout );
    if (( ret = mbedtls_ssl_setup( &ssl, &conf )) != 0 ) {
        printf( " failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret );
        return -1;
    }
    if (( ret = mbedtls_ssl_set_hostname( &ssl, SERVER_NAME )) != 0 ) {
        printf( " failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret );
        return -1;
    }
    mbedtls_ssl_set_bio( &ssl, &serverId, mbedtls_net_send, mbedtls_net_recv, NULL );
	
	while ((ret = mbedtls_ssl_handshake( &ssl )) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            printf( " failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret );
            return -1;
        }
    }

    if (( ret = mbedtls_ssl_get_verify_result( &ssl ) ) != 0) {
        char vrfy_buf[512];
        mbedtls_x509_crt_verify_info( vrfy_buf, sizeof( vrfy_buf ), "  ! ", ret);
		return -1;
    }
	
	return 0;
}


void ssl_func( void *pdata )
{
	printf("\r\n-----------[%d]:%s------------\r\n",__LINE__,__func__);
    int ret, len;
	uint32_t flags;
    unsigned char buf[1024];

	app_ssl_init(0,0);


    printf( "  > Write to server:" );
    len = sprintf( (char *) buf, GET_REQUEST );
    while ((ret = mbedtls_ssl_write( &ssl, buf, len)) <= 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret);
            goto exit;
        }
    }

    len = ret;
    printf( " %d bytes written\n\n%s", len, (char *) buf );

    printf( "  < Read from server:" );
    do {
    	printf("\r\n-----------[%d]:%s------------\r\n",__LINE__,__func__);
        read_buf_len = sizeof( read_buf ) - 1;
        memset( read_buf, 0, sizeof( read_buf ) );
        ret = mbedtls_ssl_read( &ssl, read_buf, read_buf_len );

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

    mbedtls_ssl_close_notify( &ssl );

exit:
	printf("\r\n-----------[%d]:%s------------\r\n",__LINE__,__func__);
    mbedtls_net_free( &serverId );
    mbedtls_x509_crt_free( &cacert );
    mbedtls_ssl_free( &ssl );
    mbedtls_ssl_config_free( &conf );
    mbedtls_ctr_drbg_free( &ctrDrbg );
    mbedtls_entropy_free( &entropy );
    OS_TaskDelete(NULL);
}


void ssl_test_init(void)
{
    OS_TaskCreate(ssl_func, "ssl_func", 4096, NULL, 2, NULL);
}




