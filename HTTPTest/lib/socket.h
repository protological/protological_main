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
#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <stdbool.h>
#include <stdint.h>

#define NUM_SOCKETS 3

typedef enum{
	PROTO_UDP=0,
	PROTO_TCP,
}socket_proto_t;

typedef void (*socket_rx_callback_t)(int socket, uint8_t * buf, int size);

void socket_initlib();

int socket_new(socket_proto_t proto, socket_rx_callback_t cb);

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
