#include "socket.h"
#include "tls.h"
#include "platform_socket.h"

#define NAME	"PLTSOCK"
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


void sock_initlib()
{
    socket_initlib();
    tls_initlib();
}

int sock_new(sock_proto_t proto)
{
	socket_proto_t sproto;
	switch(proto){
	case SOCK_PROTO_UDP: sproto=PROTO_UDP; break;
	case SOCK_PROTO_TCP: sproto=PROTO_TCP; break;
	default: return -1;
	}
	return socket_new(sproto);
}

int sock_connect(int sock, char * ipaddr, uint16_t port)
{
	return socket_connect(sock,ipaddr,port);
}

bool sock_connected(int sock)
{
	return socket_connected(sock);
}

int sock_close(int sock)
{
	return socket_close(sock);
}

int sock_send(int sock, uint8_t * data, int size)
{
    DBG("%s: Socket %d send %d bytes (x%08X)\n",NAME,sock, size, (int)data);
	return socket_send(sock,data,size);
}

int sock_recv(int sock, uint8_t * data, int size)
{
	return socket_recv(sock,data,size);
}

int sock_bytes_available(int sock)
{
	return socket_bytes_available(sock);
}


// EOF
