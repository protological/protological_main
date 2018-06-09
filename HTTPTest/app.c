
#include "defs.h"

#include "app.h"
#include "debug.h"

#include "httpclient.h"

#define NAME	"APP"

typedef enum{
    STATE_INIT = 0,
    STATE_MAIN,
}app_states_e;


static app_states_e m_state;
static client_t * m_client;

void app_init()
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

    client_getreq(m_client, "/", "127.0.0.1",12345);

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

#if 0
void app_test()
{
    client_t client;
    client = client_new("127.0.0.1",80);
    client_header_add(&client,"Testing","1234");

    client_end(&client);
    return;
}
#endif

// EOF
