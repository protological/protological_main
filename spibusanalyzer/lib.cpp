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
#include <stdint.h>

#include "lib.h"

#define ENABLE_TIMER_FUNCTIONS  0

void pbuf(uint8_t * buf, uint8_t size)
{
    char out[5];
    uint8_t x;
    for(x=0;x<size;x++)
    {
        sprintf(out,"x%02X,",buf[x]);
        Serial.write(out);
    }
    Serial.write('\n');
    return;
}


// *******************************************************************************
// Serial functions
// *******************************************************************************
void serial_flush()
{
    char c;
    Serial.flush();
    delay(1);
    while(Serial.available())
    {
        delay(1);
        c = Serial.read();
    }
    return;
}
uint32_t serial_getnumber(char * msg)
{
    uint32_t v=0;
    char c;
    Serial.write(msg);
    serial_flush();
    do{
        c = Serial.read();
        if(c>-1)
        {
            if(c<'0' || c>'9') return v;
            v = v*10;
            v = v+(c-'0');
            Serial.write(c);
        }
    }while(1);
    return v;
}


// *******************************************************************************
// timer 1 functions
// *******************************************************************************
// Setup timer 1 but don't start it
// _clockTimeMS is milliseconds for the clock
void timer1_setup(float _clockTimeMS)
{
#if ENABLE_TIMER_FUNCTIONS
    //cli();

    // Clear timer 1
    TCCR1A = 0;
    TCCR1B = 0;

    // set Compare Match value:
    // ATmega32U crystal is 16MHz
    // timer resolution = 1/( 16E6 /64) = 4E-6 seconds for 64 prescaling
    // target time = timer resolution * (# timer counts + 1)
    // so timer counts = (target time)/(timer resolution) -1
    // For 1 ms interrupt, timer counts = 1E-3/4E-6 - 1 = 249
    OCR1A = (uint32_t)(_clockTimeMS * 249);

    // Turn on CTC mode:
    TCCR1B |= (1 << WGM12);

    // Enable timer compare interrupt:
    TIMSK1 |= (1 << OCIE1A);

    // Interrupt enable
    //sei();
#endif
    return;
}

void timer1_start()
{
#if ENABLE_TIMER_FUNCTIONS
    //cli();

    // Start the timer 1 counting, with prescaler 64
    TCCR1B |= (1 << CS11) | (1 << CS10);
    // Start the timer counting, with prescaler 1024
    //TCCR1B |= (1 << CS12) | (1 << CS10);

    //sei();
#endif
}

void timer1_stop()
{
#if ENABLE_TIMER_FUNCTIONS
    //cli();
    //Stop the timer counting
    TCCR1B &= 0B11111000;
    //sei();
#endif
}

void timer1_reset()
{
#if ENABLE_TIMER_FUNCTIONS
    //cli();
    TCCR1A = 0;  // set all bits of Timer/Counter 1 Control Register to 0
    TCCR1B = 0;
    // Reset the Counter value to 0:
    TCNT1 = 0;
    OCR1A = 0;

    //sei();
#endif
}

#if ENABLE_TIMER_FUNCTIONS
// This code is triggered every time the global clock ticks
ISR(TIMER1_COMPA_vect)
{
    return;
}
#endif

// EOF
