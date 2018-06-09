
#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#define CLIENT_MAX_HEADERS  10

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
	client_buffer_t rx;
	client_headers_t headers[CLIENT_MAX_HEADERS];
	int headers_count;
}client_t;

void client_initlib();

client_t * client_new();

bool client_header_add(client_t * c, char * key, char * value);

bool client_payload_add(client_t * c, char * payload, int size);

void client_end(client_t * c);

int client_getreq(client_t * c, char * path,char * ipaddr, int port);

void client_process();

#endif

// EOF
