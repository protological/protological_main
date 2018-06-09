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
 *
 */
#include "defs.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "socket_driver.h"
#include "platform_socket.h"
#include "httpclient.h"
#include "app.h"
#include "app2.h"

bool g_running = false;

// This handles a ctrl-c & SIGTERM and exits
void main_term(int signum)
{
	printf(" ** Client received exit SIG, exiting...\n");
    g_running=false;
    return;
}
int main()
{
	client_t client;
	struct sigaction action;

	// Handle a ctl-c
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = main_term;
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGINT, &action, NULL);

	g_running = true;

    printf("Running\n");
    
    sock_initlib();
    client_initlib();

    app_init();
    //app2_init();

    app_test();

#if 0
    app_test();
    driver_process();
#else
    while(g_running)
    {
        //app2_mainloop();
    	app_mainloop();
    	driver_process();
    	client_process();
    	usleep(0.1*USEC_PER_SEC);
    	//g_running = false;
    }
#endif
    //app2_end();
    app_end();

    printf("End\n");

    return 0;
}
