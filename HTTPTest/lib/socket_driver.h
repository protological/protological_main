
#ifndef __SOCKET_DRIVER_H__
#define __SOCKET_DRIVER_H__

#include "socket.h"

void driver_sock_init();

int driver_sock_new(socket_proto_t proto);

int driver_sock_connect(int sock, char * ipaddr, uint16_t port);

int driver_sock_close(int sock);

int driver_sock_send(int sock, uint8_t * data, int size);

int driver_sock_recv(int sock, uint8_t * data, int size);

void driver_process();

#endif

// EOF
