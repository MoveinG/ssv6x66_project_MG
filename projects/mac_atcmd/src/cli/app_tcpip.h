#ifndef _APP_TCPIP_H_
#define _APP_TCPIP_H_
#include "at_cmd.h"


extern int  app_tcp_create(comm_t* comm_tcp);
extern int  app_tcp_send(int socketId, char* buf, int len);
extern int  app_tcp_close(comm_t* comm_tcp);
extern int  app_ssl_create(void);
extern int  app_ssl_send(char* buf, int len);
extern void app_ssl_close(void);





#endif /* end of include guard:  */


