
#ifndef __PLATFORM_SOCKET_H__
#define __PLATFORM_SOCKET_H__

#include <stdbool.h>
#include <stdint.h>

typedef enum{
	SOCK_PROTO_UDP=0,
	SOCK_PROTO_TCP,
}sock_proto_t;

void sock_initlib();

int sock_new(sock_proto_t proto);

int sock_connect(int sock, char * ipaddr, uint16_t port);

bool sock_connected(int sock);

int sock_close(int sock);

int sock_send(int sock, uint8_t * data, int size);

int sock_recv(int sock, uint8_t * data, int size);

int sock_bytes_available(int sock);


#endif

// EOF
