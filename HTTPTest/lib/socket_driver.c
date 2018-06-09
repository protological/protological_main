
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#include "socket_driver.h"
#define NAME	"DRIVER"
#ifdef DEBUG
#include "debug.h"
#else
#define DBG(...)
#endif


// Types and defines
// ---------------------------------------------------------------------------

// Local prototypes
// ---------------------------------------------------------------------------
static uint32_t parseIPV4string(char* ipAddress);

// Variables
// ---------------------------------------------------------------------------
static int active_sockets[NUM_SOCKETS];
static uint8_t m_rx_buffer[100];

// Public functions
// ---------------------------------------------------------------------------

void driver_sock_init()
{
    memset(active_sockets,0,sizeof(active_sockets));
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
#if 1
    return send(sock,data,size, 0);
#else
    data[size]=0;
    printf("TX: --%s--",(char*)data);
    return size;
#endif
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

// Private functions
// ---------------------------------------------------------------------------

// https://stackoverflow.com/questions/10283703/conversion-of-ip-address-to-integer
#if 0
static uint32_t parseIPV4string(char* ipAddress) {
  char ipbytes[4];
  sscanf(ipAddress, "%uhh.%uhh.%uhh.%uhh", &ipbytes[3], &ipbytes[2], &ipbytes[1], &ipbytes[0]);
  return ipbytes[0] | ipbytes[1] << 8 | ipbytes[2] << 16 | ipbytes[3] << 24;
}
#endif

// EOF
