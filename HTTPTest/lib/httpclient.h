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
#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#define CLIENT_MAX_HEADERS  10

typedef void (*client_rx_callback_t)(uint8_t * buf, int size);

typedef struct{
    char * buffer;
    int size;
    int index;
}client_buffer_t;
typedef struct{
    char * key;
    char * value;
}client_headers_t;
typedef struct{
	int sock;
	bool used;
	bool open;
	bool transmitting;
	client_buffer_t tx;
	client_rx_callback_t rx_cb;
	client_headers_t headers[CLIENT_MAX_HEADERS];
	int headers_count;
	char * payload;
	int payload_size;
}client_t;

void client_initlib();

client_t * client_new();

bool client_header_add(client_t * c, char * key, char * value);

bool client_payload_add(client_t * c, char * payload, int size);

int client_getreq(client_t * c, char * path,char * ipaddr, int port, client_rx_callback_t cb);

void client_reqcomplete(client_t *c);

void client_end(client_t * c);

void client_process();

#endif

// EOF
