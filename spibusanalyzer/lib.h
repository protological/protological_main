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
#ifndef __LIB_H__
#define __LIB_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wwrite-strings"

#define MAX_PRINT_BUFFER    50
extern char m_printbuffer[];
extern char m_rx_buffer[];

#define toggle(P)   do{if(digitalRead(P)){ digitalWrite((P),LOW); }else{ digitalWrite((P),HIGH);}}while(0)

#define cmp(S,B)    (strncmp((B),(S),sizeof(S)-1)==0)


void pbuf(uint8_t * buf, uint8_t size);

void serial_flush();

uint32_t serial_getnumber(char * msg);

void timer1_setup(float _clockTimeMS);

void timer1_start();

void timer1_stop();

void timer1_reset();

#endif

// eof
