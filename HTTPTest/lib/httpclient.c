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
#include <stdio.h>

#include "platform_socket.h"
#include "httpclient.h"

#define NAME	"CLIENT"
#if defined(DEBUG) && 0
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
static bool _client_load_uri(client_t * c, char * method, char * uri);
static bool _client_load_boilerplate(client_t * c, char * host);
static bool _client_load_headers(client_t * c);
static bool _client_load_payload(client_t * c);
static bool _client_load_string(client_t * , char * str);
static int _client_req(client_t * c, char * method, char * path,char * host, int port, client_header_callback_t headers_cb,client_payload_callback_t payload_cb);
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
	m_single_client.tx.index = 0;
	m_single_client.rx.buffer = m_rx_buffer;
    m_single_client.rx.size = sizeof(m_rx_buffer);
    m_single_client.rx.index = 0;
    m_single_client.rx_state = RX_STATE_HEADERS;
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

int client_getreq(client_t * c, char * path,char * host, int port, client_header_callback_t headers_cb,client_payload_callback_t payload_cb)
{
    return _client_req(c, "GET", path,host,port,headers_cb,payload_cb);
}
int client_postreq(client_t * c, char * path,char * host, int port, client_header_callback_t headers_cb,client_payload_callback_t payload_cb)
{
    return _client_req(c, "POST", path,host,port,headers_cb,payload_cb);
}
int client_deletereq(client_t * c, char * path,char * host, int port, client_header_callback_t headers_cb,client_payload_callback_t payload_cb)
{
    return _client_req(c, "DELETE", path,host,port,headers_cb,payload_cb);
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

static void _check_txbuffer(client_t * C){
    if((C)->tx.index>=(C)->tx.size-50)
        DBG("%s: WARNING Buffer at %d of %d\n",NAME,(C)->tx.index,(C)->tx.size);
}

static bool _client_load_uri(client_t * c, char * method, char * uri)
{
    // TODO: Add bouncds checking
    if(!c || !(c->tx.buffer)) return false;
    _check_txbuffer(c);
    TXINDEX(c) += sprintf(TXHEAD(c),
            "%s %s HTTP/1.1%s",method,uri,CRLF);
    return true;
}
static bool _client_load_boilerplate(client_t * c,char * host)
{
    // TODO: Add bouncds checking
    if(!c || !(c->tx.buffer)) return false;
    _check_txbuffer(c);
    // Add in the user-agent
    TXINDEX(c) += sprintf(TXHEAD(c),
            "user-agent: %s%s",USER_AGENT,CRLF);
    // Add in the current timestamp

    // Add in the host
    TXINDEX(c) += sprintf(TXHEAD(c),
                "host: %s%s",host,CRLF);
    // Add in ....

    return true;
}
static bool _client_load_headers(client_t * c)
{
    // TODO: Add bouncds checking
    int x;
    if(!c || !(c->tx.buffer)) return false;
    _check_txbuffer(c);

    for(x=0;x<c->headers_count;x++)
    {
        DBG("%s: Add header %d\n",NAME,x);
        TXINDEX(c) += sprintf(TXHEAD(c),"%s: ",c->headers[x].key);
        if(c->headers[x].value)
            TXINDEX(c) += sprintf(TXHEAD(c),"%s",c->headers[x].value);
        TXINDEX(c) += sprintf(TXHEAD(c),"%s",CRLF);
    }
    return true;
}
static bool _client_load_payload(client_t * c)
{
    // TODO: Add bouncds checking
    if(!c || !(c->tx.buffer)) return false;
    _check_txbuffer(c);
    memcpy(TXHEAD(c),c->payload,c->payload_size);
    TXINDEX(c) += c->payload_size;
    return true;
}
static bool _client_load_string(client_t * c, char * str)
{
    // TODO: Add bouncds checking
    if(!c || !(c->tx.buffer)) return false;
    _check_txbuffer(c);
    TXINDEX(c) += sprintf(TXHEAD(c),"%s",str);
    return true;
}

static int _client_req(client_t * c, char * method, char * path,char * host, int port, client_header_callback_t headers_cb,client_payload_callback_t payload_cb)
{
    int ret;
    char ipaddr[16];
    if(!c || !path || !host) return -1;
    if(c->transmitting){ DBG("%s: Transmissing can't start again\n",NAME); return -1;}

    // TODO: Get ip from hostname
    if(sock_hosttoip(host, ipaddr)<0)
    {
        DBG("%s: Error with get IP for host '%s'\n",NAME,host);
        return -1;
    }

    // TODO: Add bouncds checking, is return false?
    _client_load_uri(c, method, path);
    _client_load_boilerplate(c, host);
    _client_load_headers(c);
    _client_load_string(c,CRLF);
    _client_load_payload(c);

    if(!sock_connected(c->sock)){
        if(sock_connect(c->sock,ipaddr,port)<0){
            DBG("%s: Error connecting\n",NAME);
            return -1;
        }
    }
    c->rx_headers_cb = headers_cb;
    c->rx_payload_cb = payload_cb;

    //DBG("REQ: --%s--\n",c->tx.buffer);
    c->transmitting = true;
    c->rx_state = RX_STATE_HEADERS;
    //printf("%s\n",c->tx.buffer);
    DBG("%s: Socket %d send %d bytes (x%08X)\n",NAME,c->sock, c->tx.size, (int)(c->tx.buffer));
    ret = sock_send(c->sock,(uint8_t*)(c->tx.buffer),c->tx.index);
    if(ret<c->tx.index)
    {
        DBG("%s: Only sent %d of %d bytes\n",NAME,ret,c->tx.index);
    }
    return ret;
}

static void _client_socket_rx(int sock, uint8_t * buf, int size)
{
    client_t * c;
    char * line_end;
    int index;
    bool process;
    char * key;
    char * val;

    //for(x=0;x<list;x++
    c = &m_single_client;
    if(c->sock!=sock){
        DBG("%s: Got socket %d, but have socket %d\n",NAME,sock,c->sock);
        return;
    }
    DBG("%s: Got %d bytes for socket %d\n",NAME,size,sock);

    memcpy(&(c->rx.buffer[c->rx.index]),buf,size);
    c->rx.index += size;

    process = true;
    do{
        //pbuf(c->rx.buffer,c->rx.index);
        switch(c->rx_state){
        case RX_STATE_HEADERS:

            line_end = strstr(c->rx.buffer,CRLF);
            if(line_end)
            {
                index = (int)(line_end - c->rx.buffer);
                DBG("%s: Found line end at %d\n",NAME,index);
                // Do things with line
                c->rx.buffer[index]=0;
                if(index==0)
                {
                    c->rx_state = RX_STATE_PAYLOAD;
                    DBG("%s: Headers RX done\n",NAME);
                }else{
                    //printf("----%s\n\n",c->rx.buffer);
                    key = strtok(c->rx.buffer,":");
                    val = strtok(NULL,"");
                    if(c->rx_headers_cb) c->rx_headers_cb(key,val);
                }

                // Move data down
                memmove(c->rx.buffer,&(c->rx.buffer[index+2]), c->rx.index-index-2);
                c->rx.index-=index+2;
                c->rx.buffer[c->rx.index]=0;
            }else{
                process = false;
            }
            break;
        case RX_STATE_PAYLOAD:
            DBG("%s: Payload %d bytes\n",NAME,c->rx.index);
            if(c->rx_payload_cb) c->rx_payload_cb(c->rx.buffer,c->rx.index);
            c->rx.index=0;
            c->rx.buffer[0]=0;
            process=false;
            break;
        }
    }while(process);

    //if(c->rx_cb) c->rx_cb(buf,size);
    return;
}

// EOF
