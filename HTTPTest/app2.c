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

#include "app2.h"
#include "debug.h"

#include "httpclient.h"

#define NAME	"APP2"

typedef enum{
    STATE_INIT = 0,
    STATE_MAIN,
}app_states_e;


static app_states_e m_state;
static client_t * m_client;
static void app2_rx(uint8_t * buf, int size)
{
    DBG("%s: Got RX of %d bytes\n",NAME,size);
}
void app2_init()
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
    for(x=0;x<CLIENT_MAX_HEADERS;x++)
        client_header_add(m_client,"x-auth-key","adf4238a-882b-9ddc-4a9d-5b6758e4159e");

    client_getreq(m_client, "/", "127.0.0.1",12345, app2_rx);

    DBG("%s: Started\n",NAME);
	return;
}

void app2_end()
{
    client_end(m_client);
    DBG("%s: Ended\n",NAME);
	return;
}

void app2_mainloop()
{
	return;
}

#if 0
void app2_test()
{
    client_t client;
    client = client_new("127.0.0.1",80);
    client_header_add(&client,"Testing","1234");

    client_end(&client);
    return;
}
#endif

// EOF
