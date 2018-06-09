
#ifndef __TLS_H__
#define __TLS_H__

void tls_initlib();

int tls_socket_new();

int tls_socket_connect(int sock, char * ipaddr, uint16_t port);

bool tls_socket_connected(int sock);

int tls_socket_close(int sock);

int tls_socket_send(int sock, uint8_t * data, int size);

int tls_socket_recv(int sock, uint8_t * data, int size);

int tls_socket_bytes_available(int sock);

#endif

// EOF
