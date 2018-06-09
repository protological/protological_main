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

#include "socket.h"
#include "socket_driver.h"
#define NAME	"SOCKET"
#ifdef DEBUG
#include "debug.h"
#else
#define DBG(...)
#endif

// Types and defines
// ---------------------------------------------------------------------------

#define RX_BUFFER_SIZE	50

#define MAX_IPADDR_SIZE 15

typedef struct{
	char ipaddr[MAX_IPADDR_SIZE];
	uint16_t port;
	int system_socket_id;
	bool used;
	bool connected;
	socket_rx_callback_t cb;
}socket_control_block_t;

// Local prototypes
// ---------------------------------------------------------------------------

// Variables
// ---------------------------------------------------------------------------

socket_control_block_t SCB[NUM_SOCKETS];
uint8_t rx_buffers[NUM_SOCKETS][RX_BUFFER_SIZE];

// Public functions
// ---------------------------------------------------------------------------

void socket_initlib()
{
	int x;
	for(x=0;x<NUM_SOCKETS;x++)
	{
		memset(&(SCB[x]),0,sizeof(socket_control_block_t));
		SCB[x].cb = NULL;
		SCB[x].system_socket_id = -1;
	}
	return;
}

int socket_new(socket_proto_t proto, socket_rx_callback_t cb)
{
	int sys_socket;
	int x;

	for(x=0;x<NUM_SOCKETS;x++)
	{
		if(SCB[x].used==false)
		{
			sys_socket = driver_sock_new(proto);
			if(sys_socket<0){
				DBG("%s: Error sys socket %d\n",NAME,sys_socket);
				return -1;
			}
			SCB[x].used=true;
			SCB[x].connected=false;
			SCB[x].system_socket_id = sys_socket;
			SCB[x].cb = cb;
			DBG("%s: New socket %d to %s:%d\n",NAME,x,SCB[x].ipaddr,SCB[x].port);
			return x;
		}
	}
	return -1;
}

int socket_connect(int sock, char * ipaddr, uint16_t port)
{
    if(sock<0 || sock>=NUM_SOCKETS || SCB[sock].used==false) return -1;
    strncpy((char*)&(SCB[sock].ipaddr),ipaddr,MAX_IPADDR_SIZE);
    SCB[sock].port=port;
    if(driver_sock_connect(SCB[sock].system_socket_id,ipaddr,port)<0)
    {
        DBG("%s: Error connecting\n",NAME);
        return -1;
    }
    SCB[sock].connected = true;
    DBG("%s: Socket %d connect\n",NAME,sock);
	return 0;
}

bool socket_connected(int sock)
{
    if(sock<0 || sock>=NUM_SOCKETS || SCB[sock].used==false) return false;
    return (SCB[sock].connected);
}

int socket_close(int sock)
{
    if(sock<0 || sock>=NUM_SOCKETS || SCB[sock].used==false) return -1;
    DBG("%s: Socket %d close\n",NAME,sock);
    SCB[sock].connected=false;
    SCB[sock].used = false;
    driver_sock_close(SCB[sock].system_socket_id);
    SCB[sock].system_socket_id=-1;
	return 0;
}

int socket_send(int sock, uint8_t * data, int size)
{
    int nbytes;
    if(sock<0 || sock>=NUM_SOCKETS) return -1;
    if(!SCB[sock].used)
    {
        DBG("%s: Socket %d not used\n",NAME,sock);
        return -1;
    }
    if(!SCB[sock].connected)
    {
        DBG("%s: Socket %d not connected\n",NAME, sock);
        return -1;
    }
    DBG("%s: Socket %d send %d bytes (x%08X)\n",NAME,sock, size, (int)data);
    nbytes = driver_sock_send(SCB[sock].system_socket_id,data,size);
	return nbytes;
}

int socket_recv(int sock, uint8_t * data, int size)
{
    int nbytes;
    if(sock<0 || sock>=NUM_SOCKETS || !SCB[sock].used || !SCB[sock].connected) return -1;
    DBG("%s: Socket %d read up to %d bytes\n",NAME,sock,size);
    // if(SCB[sock].rx_buffer_index>0)
    //nbytes = socket_read(SCB[sock].system_socket_id,data,size);
	return 0;
}

int socket_bytes_available(int sock)
{
    return 0;
}

void socket_rx_bytes(int sock, uint8_t * buf, int size)
{
    int x;
    DBG("%s: Socket %d RX %d bytes\n",NAME,sock,size);
    for(x=0;x<NUM_SOCKETS;x++)
    {
        if(SCB[x].system_socket_id==sock)
            if(SCB[x].cb) SCB[x].cb(x,buf,size);
    }
    return;
}

// Private functions
// ---------------------------------------------------------------------------

// EOF
