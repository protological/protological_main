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
static void client_buffer_clear(client_buffer_t * b);

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
	sock = sock_new(SOCK_PROTO_TCP);
	memset(&m_single_client,0,sizeof(client_t));
	m_single_client.sock = sock;
	m_single_client.tx.buffer = m_tx_buffer;
	m_single_client.tx.size = sizeof(m_tx_buffer);
	m_single_client.tx.index = 0;
	m_single_client.rx.buffer = m_rx_buffer;
    m_single_client.rx.size = sizeof(m_rx_buffer);
    m_single_client.rx.index = 0;
	if(sock<0){
		DBG("%s: Error, socket is %d\n",NAME,sock);
		return NULL;
	}
	m_single_client.used=true;
	m_single_client.headers_count = 0;
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
    return true;
}

void client_end(client_t * c)
{
    if(!c) return;
    sock_close(c->sock);
    memset(c,0,sizeof(client_t));
    c->used = false;
    return;
}

int client_getreq(client_t * c, char * path,char * ipaddr, int port)
{
    int ret;

    if(!c || !path || !ipaddr) return -1;
    if(c->transmitting) return -1;

    _client_load_uri(c, "GET", path);
    _client_load_boilerplate(c);
    _client_load_headers(c);
    _client_load_payload(c);
    _client_load_string(c,CRLF);

    if(!sock_connected(c->sock)){
        if(sock_connect(c->sock,ipaddr,port)<0){
            DBG("%s: Error connecting\n",NAME);
            return -1;
        }
    }

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
    return;
}
static void _client_load_string(client_t * c, char * str)
{
    if(!c || !(c->tx.buffer)) return;
    _check_buffer(c);
    TXINDEX(c) += sprintf(TXHEAD(c),"%s",str);
}

static void client_buffer_clear(client_buffer_t * b)
{
    if(!b) return;
    b->index=0;
    return;
}

// EOF
