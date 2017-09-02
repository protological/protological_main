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

#include "RTCM41.h"

#include "lib.h"

RTCM41::RTCM41()
{
    speed = 200;
    mode = SPI_MODE0;
    sspin = 10;

    return;
}
RTCM41::~RTCM41()
{
    return;
}
void RTCM41::settings(uint32_t newspeed, uint8_t newmode, uint8_t newsspin)
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
uint8_t RTCM41::read(uint8_t bytes,uint8_t * buf)
{
    return bytes;
}

uint8_t RTCM41::write(uint8_t bytes,uint8_t * buf)
{
    return bytes;
}
void RTCM41::cmd_list()
{
    Serial.write("time  - Get time\n");
}
bool RTCM41::cmd(char * cmd)
{
    if(cmp("time",cmd)){
        // Get the current time

    }else{
        return false;
    }
    return true;
}


// eof
