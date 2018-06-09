
#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <stdbool.h>
#include <stdint.h>

#define NUM_SOCKETS 3

typedef enum{
	PROTO_UDP=0,
	PROTO_TCP,
}socket_proto_t;

void socket_initlib();

int socket_new(socket_proto_t proto);

int socket_connect(int sock, char * ipaddr, uint16_t port);

// Called by client to check if connected
bool socket_connected(int sock);

int socket_close(int sock);

// Called by client to write bytes to socket
int socket_send(int sock, uint8_t * data, int size);

// Called by client to read 'size' bytes from buffer
int socket_recv(int sock, uint8_t * data, int size);

// Called by client to get the number of bytes available
int socket_bytes_available(int sock);

// Called by driver to load bytes in for a socket
void socket_rx_bytes(int sock, uint8_t * buf, int size);

#endif

// EOF
