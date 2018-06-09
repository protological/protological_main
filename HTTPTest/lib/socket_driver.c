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
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include<netdb.h> //hostent

#include "socket_driver.h"
#define NAME	"DRIVER"
#if defined(DEBUG) && 0
#include "debug.h"
#else
#define DBG(...)
#endif

//#define EMPTY_FUNCTIONS

// Types and defines
// ---------------------------------------------------------------------------

// Local prototypes
// ---------------------------------------------------------------------------

// Variables
// ---------------------------------------------------------------------------
static int active_sockets[NUM_SOCKETS];
static uint8_t m_rx_buffer[100];

// Public functions
// ---------------------------------------------------------------------------
#if defined(EMPTY_FUNCTIONS)
void driver_sock_init()
{
    memset(active_sockets,0,sizeof(active_sockets));
}

int driver_sock_hosttoip(char * host, char * ip)
{
    return 0;
}

int driver_sock_new(socket_proto_t proto)
{
    return 0;
}

int driver_sock_connect(int sock, char * ipaddr, uint16_t port)
{
    return 0;
}

int driver_sock_close(int sock)
{
    return 0;
}

int driver_sock_send(int sock, uint8_t * data, int size)
{
    return size;
}

int driver_sock_recv(int sock, uint8_t * data, int size)
{
    return 0;
}

void driver_process()
{
    return;
}
#else
void driver_sock_init()
{
    memset(active_sockets,0,sizeof(active_sockets));
}

// https://www.binarytides.com/hostname-to-ip-address-c-sockets-linux/
int driver_sock_hosttoip(char * host, char * ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ( (he = gethostbyname( host ) ) == NULL)
    {
        // get the host info
        DBG("%s: Error with gethostbyname",NAME);
        return -1;
    }

    addr_list = (struct in_addr **) he->h_addr_list;
#if 1
    if(addr_list[0]!=NULL)
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[0]) );
        return 0;
    }
#else
    for(i = 0; addr_list[i] != NULL; i++)
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }
#endif

    return -1;
}

int driver_sock_new(socket_proto_t proto)
{
	int sock;
	int style;
	int x;

	switch(proto){
	case PROTO_UDP: style=SOCK_PACKET; break;
	case PROTO_TCP: style=SOCK_STREAM; break;
	default:
		DBG("%s: Invalid style %d\n",NAME,proto);
		return -1;
		break;
	}

	if ((sock = socket(AF_INET,style, 0) ) < 0)
	{
		DBG("%s: Unable to create socket\n",NAME);
		return -1;
	}
	for(x=0;x<NUM_SOCKETS;x++)
	{
	    if(active_sockets[x]==0)
	    {
	        active_sockets[x]=sock;
	        DBG("%s: Save socket to slot %d\n",NAME,x);
	        break;
	    }
	}
	if(x==NUM_SOCKETS)
	{
	    DBG("%s: Error, can't save socket to slot\n",NAME);
	    close(sock);
	    return -1;
	}
	DBG("%s: New socket %d\n",NAME,sock);
	return sock;
}

int driver_sock_connect(int sock, char * ipaddr, uint16_t port)
{
    struct sockaddr_in sockinfo;
    sockinfo.sin_family = AF_INET;
    sockinfo.sin_port = htons(port);
    sockinfo.sin_addr.s_addr = inet_addr(ipaddr);
    if(connect(sock,(struct sockaddr*)&sockinfo,sizeof(struct sockaddr))<0)
    {
       DBG("%s: Error connecting\n",NAME);
       return -1;
    }
	return 0;
}

int driver_sock_close(int sock)
{
    int x;
	if(sock<=0) return -1;
    DBG("%s: Close socket %d\n",NAME,sock);
    // Remove from active list
    for(x=0;x<NUM_SOCKETS;x++)
    {
        if(active_sockets[x]==sock){
            active_sockets[x]=0;
            break;
        }
    }
	close(sock);
	return 0;
}

int driver_sock_send(int sock, uint8_t * data, int size)
{
    if(sock<1) return -1;
    DBG("%s: Socket %d send %d bytes (x%08X)\n",NAME,sock, size, (int)data);
    return send(sock,data,size, 0);
}

int driver_sock_recv(int sock, uint8_t * data, int size)
{
	return 0;
}

void driver_process()
{
    int x;
    int sock;
    int size;
    int nbytes;
    for(x=0;x<NUM_SOCKETS;x++)
    {
        if(active_sockets[x])
        {
            // TODO: Make this select wihtout timeout so we
            // know when the server closes
            sock = active_sockets[x];
            //DBG("%s: Check socket %d\n",NAME,sock);
            ioctl( sock, FIONREAD, &size );
            if(size>0){
                DBG("%s: Socket %d has %d bytes\n",NAME,sock,size);
                if(size>sizeof(m_rx_buffer)) size=sizeof(m_rx_buffer);
                nbytes = read(sock,m_rx_buffer,size);
                socket_rx_bytes(sock, m_rx_buffer, nbytes);
            }else if(size<0){
                printf("Error?\n");
            }else{
                //printf("zero\n");
            }
        }
    }
	return;
}
#endif
// Private functions
// ---------------------------------------------------------------------------

// EOF
