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
#include "defs.h"

#include "app.h"
#include "debug.h"

#include "httpclient.h"

#include <string.h>
#include <stdlib.h>

#define NAME	"APP"

#define APP_RX_BUFFER_MAX   512

typedef enum{
    STATE_INIT = 0,
    STATE_MAIN,
}app_states_e;


static app_states_e m_state;
static client_t * m_client;
static char rx_buffer[APP_RX_BUFFER_MAX];
static int rx_length = 0;
static int rx_total = 0;


void app_headers_rx(char * key,char * value)
{
    int result;
    if(cmp(key,"HTTP")){
        char * pch;
        pch = strtok(key," ");
        pch = strtok(NULL," ");
        result = atoi(pch);
        DBG("%s: result '%s'\n",NAME,pch);
    }else if(cmp(key,"Content-Length")){
        int len=atoi(value);
        DBG("%s: length is %d\n",NAME,len);
        rx_total = 0;
        rx_length = len;
    }else{
        DBG("%s: Got header '%s':'%s'\n",NAME,key,value);
    }
}
void app_payload_rx(char * buf, int size)
{
    DBG("%s: Got payload of %d bytes\n",NAME,size);
    if(rx_length>0)
    {
        memcpy(&(rx_buffer[rx_total]),buf,size);
        rx_total += size;
        if(rx_total>=rx_length)
        {
            DBG("%s: Got full message, %d of %d bytes\n",NAME,rx_total,rx_length);
            printf("%s\n",rx_buffer);
            client_reqcomplete(m_client);
        }
    }

}
void app_init()
{
    return;
}
void app_test()
{
    int x;
    for(x=0;x<5;x++)
    {
        m_client = client_new();
        client_header_add(m_client,"Testing","1234");
        client_end(m_client);
    }

    m_client = client_new();
    client_header_add(m_client,"Testing","1234");
    //for(x=0;x<CLIENT_MAX_HEADERS;x++)
        //client_header_add(m_client,"x-auth-key","adf4238a-882b-9ddc-4a9d-5b6758e4159e");

    //client_payload_add(m_client,"{\"message\":\"1234\"}",18);

    client_getreq(m_client, "/junk", "192.168.100.100",80, app_headers_rx, app_payload_rx);
    //client_getreq(m_client, "/posts/1", "jsonplaceholder.typicode.com",80, app_headers_rx, app_payload_rx);


    client_postreq(m_client, "/", "192.168.100.100",80, app_headers_rx, app_payload_rx);


    client_deletereq(m_client, "/", "192.168.100.100",80, app_headers_rx, app_payload_rx);

    DBG("%s: Started\n",NAME);
	return;
}

void app_end()
{
    client_end(m_client);
    DBG("%s: Ended\n",NAME);
	return;
}

void app_mainloop()
{
	return;
}

// EOF
