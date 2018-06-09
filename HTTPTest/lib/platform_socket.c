/*
 * Copyright (c) 2018 Protological.com/clisystems.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * Author: Andrew Gaylo <drew@clisystems.com>
 */
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

int sock_new(sock_proto_t proto,socket_rx_callback_t cb)
{
	socket_proto_t sproto;
	switch(proto){
	case SOCK_PROTO_UDP: sproto=PROTO_UDP; break;
	case SOCK_PROTO_TCP: sproto=PROTO_TCP; break;
	default: return -1;
	}
	return socket_new(sproto,cb);
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
