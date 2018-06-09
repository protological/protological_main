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
#include <string.h>

#include "platform_socket.h"
#include "httpclient.h"

#define NAME	"CLIENT"
#ifdef DEBUG
#include "debug.h"
#else
#define DBG(...)
#endif

// Types and defines
// ---------------------------------------------------------------------------
#define HTTP        "HTTP/1.1"
#define CRLF        "\r\n"
#define USER_AGENT  "uHTTPC-0.1b"

// Local prototypes
// ---------------------------------------------------------------------------
static void _client_load_uri(client_t * c, char * method, char * uri);
static void _client_load_boilerplate(client_t * c);
static void _client_load_headers(client_t * c);
static void _client_load_payload(client_t * c);
static void _client_load_string(client_t * , char * str);
static void _client_buffer_clear(client_buffer_t * b);

static void _client_socket_rx(int sock, uint8_t * buf, int size);

// Variables
// ---------------------------------------------------------------------------
static char m_tx_buffer[1024];
static char m_rx_buffer[1024];
static client_t m_single_client;

// Public functions
// ---------------------------------------------------------------------------


void client_initlib()
{

}

client_t * client_new()
{
	int sock;
	if(m_single_client.used){
	    DBG("%s: Client already used\n",NAME);
	    return NULL;
	}
	sock = sock_new(SOCK_PROTO_TCP, _client_socket_rx);
	memset(&m_single_client,0,sizeof(client_t));
	m_single_client.sock = sock;
	m_single_client.tx.buffer = m_tx_buffer;
	m_single_client.tx.size = sizeof(m_tx_buffer);
	if(sock<0){
		DBG("%s: Error, socket is %d\n",NAME,sock);
		return NULL;
	}
	m_single_client.used=true;
	return &m_single_client;
}

bool client_header_add(client_t * c, char * key, char * value)
{
    if(!c || !key) return false;
    if(c->transmitting) return false;

    if(c->headers_count>=CLIENT_MAX_HEADERS){
        DBG("%s: Headers full\n",NAME);
        return false;
    }
    DBG("%s: Adding header %d\n",NAME,c->headers_count);
    c->headers[c->headers_count].key = key;
    c->headers[c->headers_count].value = value;
    c->headers_count++;
	return true;
}

bool client_payload_add(client_t * c, char * payload, int size)
{
    if(!c) return false;
    c->payload = payload;
    c->payload_size = size;
    return true;
}

int client_getreq(client_t * c, char * path,char * ipaddr, int port, client_rx_callback_t cb)
{
    int ret;

    if(!c || !path || !ipaddr) return -1;
    if(c->transmitting) return -1;

    _client_load_uri(c, "GET", path);
    _client_load_boilerplate(c);
    _client_load_headers(c);
    _client_load_string(c,CRLF);
    _client_load_payload(c);

    if(!sock_connected(c->sock)){
        if(sock_connect(c->sock,ipaddr,port)<0){
            DBG("%s: Error connecting\n",NAME);
            return -1;
        }
    }
    c->rx_cb = cb;

    //DBG("REQ: --%s--\n",c->tx.buffer);
    c->transmitting = true;
    DBG("%s: Socket %d send %d bytes (x%08X)\n",NAME,c->sock, c->tx.size, (int)(c->tx.buffer));
    ret = sock_send(c->sock,(uint8_t*)(c->tx.buffer),c->tx.index);
    if(ret<c->tx.index)
    {
        DBG("%s: Only sent %d of %d bytes\n",NAME,ret,c->tx.index);
    }
    return ret;
}

void client_reqcomplete(client_t *c)
{
    if(!c) return;
    c->transmitting = false;
    c->tx.index=0;
    return;
}

void client_end(client_t * c)
{
    if(!c) return;
    sock_close(c->sock);
    memset(c,0,sizeof(client_t));
    c->used = false;
    return;
}


void client_process()
{
    return;
}

// Private functions
// ---------------------------------------------------------------------------
#define TXINDEX(C)    (C)->tx.index
#define TXHEAD(C)     &((C)->tx.buffer[(C)->tx.index])

static void _check_buffer(client_t * C){
    if((C)->tx.index>=(C)->tx.size-50)
        DBG("%s: WARNING Buffer at %d of %d\n",NAME,(C)->tx.index,(C)->tx.size);
}

static void _client_load_uri(client_t * c, char * method, char * uri)
{
    if(!c || !(c->tx.buffer)) return;
    _check_buffer(c);
    TXINDEX(c) += sprintf(TXHEAD(c),
            "%s %s HTTP/1.1%s",method,uri,CRLF);
    return;
}
static void _client_load_boilerplate(client_t * c)
{
    if(!c || !(c->tx.buffer)) return;
    _check_buffer(c);
    // Add in the user-agent
    TXINDEX(c) += sprintf(TXHEAD(c),
            "user-agent: %s%s",USER_AGENT,CRLF);
    // Add in the current timestamp

    // Add in the host

    // Add in ....

    return;
}
static void _client_load_headers(client_t * c)
{
    int x;
    if(!c || !(c->tx.buffer)) return;
    _check_buffer(c);

    for(x=0;x<c->headers_count;x++)
    {
        DBG("%s: Add header %d\n",NAME,x);
        TXINDEX(c) += sprintf(TXHEAD(c),"%s: ",c->headers[x].key);
        if(c->headers[x].value)
            TXINDEX(c) += sprintf(TXHEAD(c),"%s",c->headers[x].value);
        TXINDEX(c) += sprintf(TXHEAD(c),"%s",CRLF);
    }
    return;
}
static void _client_load_payload(client_t * c)
{
    if(!c || !(c->tx.buffer)) return;
    _check_buffer(c);
    memcpy(TXHEAD(c),c->payload,c->payload_size);
    TXINDEX(c) += c->payload_size;
    return;
}
static void _client_load_string(client_t * c, char * str)
{
    if(!c || !(c->tx.buffer)) return;
    _check_buffer(c);
    TXINDEX(c) += sprintf(TXHEAD(c),"%s",str);
}

static void _client_buffer_clear(client_buffer_t * b)
{
    if(!b) return;
    b->index=0;
    return;
}

static void _client_socket_rx(int sock, uint8_t * buf, int size)
{
    client_t * c;
    //for(x=0;x<list;x++
    c = &m_single_client;
    if(c->sock!=sock){
        DBG("%s: Got socket %d, but have socket %d\n",NAME,sock,c->sock);
        return;
    }
    DBG("%s: Got %d bytes for socket %d\n",NAME,size,sock);
    c->transmitting = false;
    if(c->rx_cb) c->rx_cb(buf,size);
    return;
}

// EOF
