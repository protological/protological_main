#include "socket.h"
#include "tls.h"

#define NAME	"TLS"
#ifdef DEBUG
#include "debug.h"
#else
#define DBG(...)
#endif

// Types and defines
// ---------------------------------------------------------------------------

// Local prototypes
// ---------------------------------------------------------------------------

// Variables
// ---------------------------------------------------------------------------

// Public functions
// ---------------------------------------------------------------------------


void tls_initlib()
{
    // Setup the tls library
    return;
}

int tls_socket_new()
{
    return socket_new(PROTO_TCP);
}

int tls_socket_connect(int sock, char * ipaddr, uint16_t port)
{
    return socket_connect(sock,ipaddr,port);
}

bool tls_socket_connected(int sock)
{
    return socket_connected(sock);
}

int tls_socket_close(int sock)
{
    return socket_close(sock);
}

int tls_socket_send(int sock, uint8_t * data, int size)
{
    return socket_send(sock,data,size);
}

int tls_socket_recv(int sock, uint8_t * data, int size)
{
    return socket_recv(sock,data,size);
}

int tls_socket_bytes_available(int sock)
{
    return socket_bytes_available(sock);
}
// EOF
