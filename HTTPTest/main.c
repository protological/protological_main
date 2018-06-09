
#include "defs.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "socket_driver.h"
#include "platform_socket.h"
#include "httpclient.h"
#include "app.h"

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

#if 0
    app_test();
    driver_process();
#else
    while(g_running)
    {
    	app_mainloop();
    	driver_process();
    	client_process();
    	usleep(0.1*USEC_PER_SEC);
    	//g_running = false;
    }
#endif

    app_end();

    printf("End\n");

    return 0;
}
