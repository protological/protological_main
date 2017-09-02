/*****************************************************************************
*
* 2017 protological.com/CLI Systems LLC http://clisystems.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include <SPI.h>

#include "lib.h"
#include "GeneralSPI.h"
#include "FLASHSPI.h"
#include "RTCM41.h"

#define FIRMWARE_VERSION    "1.0"

#define TIMER_PERIOD_MS     100

#define LED_PIN             7

#define MAX_RX_SIZE         50
#define MAX_READ_SIZE       32
#define MAX_SEND_SIZE       32

char m_rx_buffer[MAX_RX_SIZE];
uint8_t m_rx_buffer_index=0;

uint8_t m_read_buffer[MAX_READ_SIZE];
uint8_t m_write_buffer[MAX_SEND_SIZE];

char m_printbuffer[MAX_RX_SIZE];

GeneralSPI  spi_general;
FLASHSPI    spi_flash;
RTCM41      spi_rtc;

// SPI settings
struct spidata{
    uint8_t mode;
    uint8_t sspin;
    uint8_t protocol;
    uint8_t reserved;
    uint32_t speed;
}spidata_s;


void setup()
{

    memset(m_rx_buffer,0,sizeof(m_rx_buffer));
    memset(m_read_buffer,0,sizeof(m_read_buffer));
    memset(m_write_buffer,0,sizeof(m_write_buffer));
    memset(m_printbuffer,0,sizeof(m_printbuffer));

    pinMode(LED_PIN,OUTPUT);

    timer1_setup(TIMER_PERIOD_MS);
    timer1_start();

    spidata_s.mode=0;
    spidata_s.sspin=10;
    spidata_s.protocol=0;
    spidata_s.speed=200;

    SPI.begin();

    Serial.begin(57600);

    info();
    help_menu();
    Serial.write(">");

    return;
}

void loop()
{
    if(rx())
    {
        process(m_rx_buffer);
    }
    return;
}

bool rx()
{
    char c;
    if(Serial.available()==false) return false;
    while(Serial.available())
    {
        c = Serial.read();
        if(c=='\n'){
            m_rx_buffer[m_rx_buffer_index]=0;
            m_rx_buffer_index=0;
            return true;
        }else if(c=='\r'){
            // Ignore Winblows?
        }else if(c>-1 && m_rx_buffer_index<MAX_RX_SIZE){
            m_rx_buffer[m_rx_buffer_index]=c;
            m_rx_buffer_index++;
        }else if(m_rx_buffer_index>=MAX_RX_SIZE){
            m_rx_buffer[sizeof(m_rx_buffer)-1]=0;
            m_rx_buffer_index=0;
            return true;
        }
    }
    return false;
}
void process(char * cmd)
{
#if 0
    sprintf(m_printbuffer,"Got: '%s'\n",m_rx_buffer);
    Serial.write(m_printbuffer);
#endif
    Serial.write(m_rx_buffer);
    Serial.write('\n');
    if(cmp("show",cmd)){
        show_settings();
#if 0
    }else if(cmp("test",cmd)){
        uint32_t v;
        Serial.write("Running test\n");
        v = serial_getnumber("Enter number: ");
        Serial.write("Number was ");
        Serial.println(v,DEC);
#endif
    }else if(cmp("read",cmd)){
        uint8_t bytes=0;
        //Serial.write("Read\n");
        bytes = serial_getnumber("Bytes to read: ");
        Serial.write('\n');
        if(bytes>MAX_READ_SIZE) bytes=MAX_READ_SIZE;
        switch(spidata_s.protocol){
        case 0:
            spi_general.settings(spidata_s.speed, spidata_s.mode, spidata_s.sspin);
            bytes = spi_general.read(bytes,m_read_buffer);
            break;
        case 1:
            spi_flash.settings(spidata_s.speed, spidata_s.mode, spidata_s.sspin);
            bytes = spi_flash.read(bytes,m_read_buffer);
            break;
        case 2:
            spi_rtc.settings(spidata_s.speed, spidata_s.mode, spidata_s.sspin);
            bytes = spi_rtc.read(bytes,m_read_buffer);
            break;
        }
        if(bytes)
            pbuf(m_read_buffer,bytes);

    }else if(cmp("speed",cmd)){
        set_speed();

    }else if(cmp("write",cmd)){
        uint8_t bytes=0;
        bytes = serial_getnumber("Bytes to write: ");
        Serial.write('\n');
        switch(spidata_s.protocol){
        case 0:
            spi_general.settings(spidata_s.speed, spidata_s.mode, spidata_s.sspin);
            bytes = spi_general.write(bytes,m_write_buffer);
            break;
        case 1:
            spi_flash.settings(spidata_s.speed, spidata_s.mode, spidata_s.sspin);
            bytes = spi_flash.write(bytes,m_write_buffer);
            break;
        case 2:
            spi_rtc.settings(spidata_s.speed, spidata_s.mode, spidata_s.sspin);
            bytes = spi_rtc.write(bytes,m_write_buffer);
            break;
        }

    }else if(cmp("proto",cmd)){
        select_protocol();

    }else if(cmp("mode",cmd)){
        select_mode();
    }else if(cmp("ss",cmd)){
        select_sspin();

    }else if(cmp("fill",cmd)){
        fill();
    }else if(cmp("dump",cmd)){
        dump();

// Help and default
    }else if(cmp("help",cmd)){
        help_menu();
    }else{
        bool result;
        switch(spidata_s.protocol){
        case 0:
            result =spi_general.cmd(cmd);
            break;
        case 1:
            result =spi_flash.cmd(cmd);
            break;
        case 2:
            result =spi_rtc.cmd(cmd);
            break;
        }
        if(!result){
            sprintf(m_printbuffer,"Unknown '%s'\n",cmd);
            Serial.write(m_printbuffer);
        }
    }
    Serial.write(">");
    return;
}

void info()
{
#if 0
    sprintf(m_printbuffer,"sizeof(char) = %d\n",sizeof(char));
    Serial.write(m_printbuffer);
    sprintf(m_printbuffer,"sizeof(int) = %d\n",sizeof(int));
    Serial.write(m_printbuffer);
    sprintf(m_printbuffer,"sizeof(u int) = %d\n",sizeof(unsigned int));
    Serial.write(m_printbuffer);
    sprintf(m_printbuffer,"sizeof(u l i) = %d\n",sizeof(unsigned long int));
    Serial.write(m_printbuffer);
    sprintf(m_printbuffer,"sizeof(uint8_t) = %d\n",sizeof(uint8_t));
    Serial.write(m_printbuffer);
    sprintf(m_printbuffer,"sizeof(uint16_t) = %d\n",sizeof(uint16_t));
    Serial.write(m_printbuffer);
    sprintf(m_printbuffer,"sizeof(uint32_t) = %d\n",sizeof(uint32_t));
    Serial.write(m_printbuffer);
#endif
    return;
}
void help_menu()
{
    sprintf(m_printbuffer,"\n\nSPI Analyzer v%s %s\n",FIRMWARE_VERSION,__DATE__);
    Serial.write(m_printbuffer);
    Serial.write("help  - This menu\n");
    Serial.write("speed - Set clock speed\n");
    Serial.write("mode  - Set mode (0-3)\n");
    Serial.write("proto - Set prototcol\n");
    Serial.write("ss    - Set SS pin\n");
    Serial.write("read  - Read bytes\n");
    Serial.write("write - Write bytes\n");
    Serial.write("show - Show settings\n");
    Serial.write("fill - Mod write buffer\n");
    Serial.write("dump - Dump write buffer\n");
    Serial.write("------------------------\n");
    switch(spidata_s.protocol){
    case 0:
        spi_general.cmd_list();
        break;
    case 1:
        spi_flash.cmd_list();
        break;
    case 2:
        spi_rtc.cmd_list();
        break;
    }

    serial_flush();
    return;
}

void show_settings()
{
    sprintf(m_printbuffer,"Mode: %d\n",spidata_s.mode);
    Serial.write(m_printbuffer);
    sprintf(m_printbuffer,"Speed: %u KHz\n",spidata_s.speed);
    Serial.write(m_printbuffer);
    sprintf(m_printbuffer,"SS Pin: %d\n",spidata_s.sspin);
    Serial.write(m_printbuffer);
    sprintf(m_printbuffer,"Protocol: %d\n",spidata_s.protocol);
    Serial.write(m_printbuffer);
}

void select_protocol()
{
    uint8_t v;
    Serial.write("\nSelect Protocol\n");
    Serial.write(" 0 - General SPI\n");
    Serial.write(" 1 - FLASH SPI\n");
    Serial.write(" 2 - M41 RTC\n");
    v = serial_getnumber("Protocol number: ");
    if(v>2) v=2;
    sprintf(m_printbuffer,"\nProtocol is: %d\n",v);
    Serial.write(m_printbuffer);
    spidata_s.protocol = v;
    return;
}
void select_mode()
{
    uint8_t v;
    Serial.write("\nSelect Mode\n");
    Serial.write(" 0 - CPOL=0,CPHA=0)\n");
    Serial.write(" 1 - CPOL=0,CPHA=1\n");
    Serial.write(" 2 - CPOL=1,CPHA=0\n");
    Serial.write(" 3 - CPOL=1,CPHA=1\n");
    v = serial_getnumber("Enter mode number: ");
    if(v>3) v=3;
    sprintf(m_printbuffer,"\nMode is now %d\n",v);
    Serial.write(m_printbuffer);
    spidata_s.mode = v;
    return;
}
void select_sspin()
{
    uint8_t v;
    sprintf(m_printbuffer,"\nSS Pin is: %d\n",spidata_s.sspin);
    Serial.write(m_printbuffer);
    v = serial_getnumber("New SS Pin: ");
    if(v>20) v=20;
    sprintf(m_printbuffer,"\nSS Pin: %d\n",v);
    Serial.write(m_printbuffer);
    spidata_s.sspin = v;
    return;
}
void set_speed()
{
    uint32_t v;
    sprintf(m_printbuffer,"\nCurrent speed: %d\n",spidata_s.speed);
    Serial.write(m_printbuffer);
    v = serial_getnumber("Enter new speed KHz: ");
    if(v<=0){
        Serial.write("\nInvalid speed\n");
    }else{
        sprintf(m_printbuffer,"\nSpeed is now %u\n",v);
        Serial.write(m_printbuffer);
        spidata_s.speed = v;
    }
    return;
}

void fill()
{
    bool abort=false;
    uint8_t count=0;
    uint8_t v=0;
    char c;
    uint8_t st=0;
    Serial.write("Enter hex values, enter to stop\n");
    serial_flush();

    do{
        st = 0;
        abort=false;
        do{
           c = Serial.read();
           if(c>-1)
           {
               if(!ishexdigit(c))
               {
                   v=0;
                   abort=true;
                   break;
               }
               if(st){
                   // Second digit
                   v<<=4;
                   v+= hextodec(c);
                   break;
               }else{
                   // First digit
                   st=1;
                   v=hextodec(c);
               }
           }
        }while(1);

        // We now have a uint8_t in v. Send it, unless we got a non hex value
        if(abort) break;

        m_write_buffer[count] = v;
        count++;

    }while(count<MAX_SEND_SIZE);

    Serial.write("Total bytes: ");
    Serial.println(count,DEC);

    return;
}
void dump()
{
    uint8_t x;
    for(x=0;x<MAX_SEND_SIZE;x+=8)
    {
        sprintf(m_printbuffer,"x%04X: ",(x*8));
        Serial.write(m_printbuffer);
        pbuf(&(m_write_buffer[x*8]),8);
        //Serial.write('\n');
    }
    return;
}

uint8_t hextodec(char c)
{
    if(c>='0' && c<='9') return c-'0';
    if(c>='A'&& c<='F') return ((c-'A')+10);
    if(c>='a'&& c<='f') return ((c-'a')+10);
    return 0;
}
bool ishexdigit(char c)
{
    if(c>='0' && c<='9') return true;
    if(c>='A'&& c<='F') return true;
    if(c>='a'&& c<='f') return true;
    return false;
}
// eof
