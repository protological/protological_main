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

#include <Arduino.h>
#include <SPI.h>

#include "FLASHSPI.h"

#include "lib.h"

#define CMD_WRITE   0x02
#define CMD_READ    0x03
#define CMD_WREN    0x06


FLASHSPI::FLASHSPI()
{
    speed = 200;
    mode = SPI_MODE0;
    sspin = 10;

    bytes_per_page=128;
    pages=512;

    return;
}
FLASHSPI::~FLASHSPI()
{
    return;
}
void FLASHSPI::settings(uint32_t newspeed, uint8_t newmode, uint8_t newsspin)
{
    switch(newmode){
    default:
    case 0: mode = SPI_MODE0; break;
    case 1: mode = SPI_MODE1; break;
    case 2: mode = SPI_MODE2; break;
    case 3: mode = SPI_MODE3; break;
    }
    speed = newspeed;
    sspin = newsspin;

    return;
}
uint8_t FLASHSPI::read(uint8_t bytes,uint8_t * buf)
{
    uint16_t addr;
    addr = serial_getnumber("Enter address (DEC): ");
    return read_addr(addr,bytes,buf);
}

uint8_t FLASHSPI::write(uint8_t bytes,uint8_t * buf)
{
    uint16_t addr;
    addr = serial_getnumber("Enter address (DEC): ");
#if 1
    uint8_t v=0;
    uint8_t x;

    wren();
    digitalWrite(sspin, HIGH);

    SPI.beginTransaction(SPISettings((speed*1000), MSBFIRST, mode));
    digitalWrite(sspin, LOW);
    // Write command
    SPI.transfer(CMD_WRITE);
    // Address
    SPI.transfer(addr>>8);
    SPI.transfer(addr&0xFF);
    for(x=0;x<bytes;x++)
    {
        if(buf) v = buf[x];
        SPI.transfer(v);
    }
    digitalWrite(sspin, HIGH);
    SPI.endTransaction();
#else
    Serial.write("FLASHSPI write\n");
#endif
    return bytes;
}
void FLASHSPI::cmd_list()
{
    Serial.write("dpage - Dump FLASH page\n");
    Serial.write("fparam- Set FLASH params\n");
    Serial.write("finfo - FLASH info\n");
    Serial.write("dmem  - Dump ALL mem\n");
}
bool FLASHSPI::cmd(char * cmd)
{
    if(cmp("dpage",cmd)){
        uint8_t x;
        uint8_t bytes;
        uint16_t addr;
        addr = serial_getnumber("Enter Page (DEC): ");
        Serial.write('\n');

        addr = (bytes_per_page * addr);
        for(x=0;x<bytes_per_page;x+=16)
        {
            bytes = read_addr(addr+(16*x),16,(uint8_t*)m_rx_buffer);
            sprintf(m_printbuffer,"x%04X: ",addr+(16*x));
            Serial.write(m_printbuffer);
            pbuf((uint8_t*)m_rx_buffer,16);
        }
    }else if(cmp("dmem",cmd)){
            uint8_t x;
            uint8_t bytes;
            uint16_t addr=0;
            uint16_t page;

            for(page=0;page<pages;page++)
            {
                addr = (bytes_per_page * page);
                for(x=0;x<bytes_per_page;x+=16)
                {
                    addr+=16;
                    bytes = read_addr(addr,16,(uint8_t*)m_rx_buffer);
                    sprintf(m_printbuffer,"x%04X: ",addr);
                    Serial.write(m_printbuffer);
                    pbuf((uint8_t*)m_rx_buffer,16);
                }
            }

    }else if(cmp("fparam",cmd)){
        uint16_t v;
        v = serial_getnumber("Bytes/page (DEC): ");
        Serial.write('\n');
        bytes_per_page = v;
        v = serial_getnumber("Pages (DEC): ");
        Serial.write('\n');
        pages = v;
    }else if(cmp("finfo",cmd)){
        sprintf(m_printbuffer,"Bytes/Page: %d\n",bytes_per_page);
        Serial.write(m_printbuffer);
        sprintf(m_printbuffer,"Pages: %d\n",pages);
        Serial.write(m_printbuffer);
    }else{
        return false;
    }
    return true;
}

uint8_t FLASHSPI::read_addr(uint16_t addr, uint8_t bytes,uint8_t * buf)
{
#if 1
    uint8_t v;
    uint8_t x;
    SPI.beginTransaction(SPISettings((speed*1000), MSBFIRST, mode));
    digitalWrite (sspin, LOW);
    // Write command
    SPI.transfer(CMD_READ);
    // Address
    SPI.transfer(addr>>8);
    SPI.transfer(addr&0xFF);
    for(x=0;x<bytes;x++)
    {
        v = SPI.transfer(0);
        if(buf) buf[x] = v;
    }
    digitalWrite(sspin, HIGH);
    SPI.endTransaction();
#else
    Serial.write("FLASHSPI read\n");
#endif
    return bytes;
}

void FLASHSPI::wren()
{
    SPI.beginTransaction(SPISettings((speed*1000), MSBFIRST, mode));
    digitalWrite(sspin, LOW);
    // WREN
    SPI.transfer(CMD_WREN);
    digitalWrite(sspin, HIGH);
    SPI.endTransaction();
}

// eof
