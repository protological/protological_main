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

#include "GeneralSPI.h"

GeneralSPI::GeneralSPI()
{
    speed = 200;
    mode = SPI_MODE0;
    sspin = 10;
    return;
}
GeneralSPI::~GeneralSPI()
{
    return;
}
void GeneralSPI::settings(uint32_t newspeed, uint8_t newmode, uint8_t newsspin)
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
uint8_t GeneralSPI::read(uint8_t bytes,uint8_t * buf)
{
    uint8_t v;
    uint8_t x;
    //Serial.write("GeneralSPI read\n");
    SPI.beginTransaction(SPISettings((speed*1000), MSBFIRST, mode));
    digitalWrite (sspin, LOW);
    for(x=0;x<bytes;x++)
    {
        v = SPI.transfer(0);
        if(buf) buf[x] = v;
    }
    digitalWrite(sspin, HIGH);
    SPI.endTransaction();

    return bytes;
}

uint8_t GeneralSPI::write(uint8_t bytes,uint8_t * buf)
{
    uint8_t v=0;
    uint8_t x;
    SPI.beginTransaction(SPISettings((speed*1000), MSBFIRST, mode));
    digitalWrite(sspin, LOW);
    for(x=0;x<bytes;x++)
    {
        if(buf) v = buf[x];
        SPI.transfer(v);
    }
    digitalWrite(sspin, HIGH);
    SPI.endTransaction();
    return bytes;
}

void GeneralSPI::cmd_list()
{
    return;
}
bool GeneralSPI::cmd(char * cmd)
{
    return false;
}


// eof
