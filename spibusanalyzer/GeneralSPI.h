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
#ifndef __GENERALSPI_H__
#define __GENERALSPI_H__

#include <stdint.h>

class GeneralSPI {
    public:
        GeneralSPI();
        ~GeneralSPI();
        void settings(uint32_t newspeed, uint8_t newmode, uint8_t newsspin);
        uint8_t read(uint8_t bytes,uint8_t * buf);
        uint8_t write(uint8_t bytes,uint8_t * buf);

        void cmd_list();
        bool cmd(char * cmd);
    private:
        uint32_t speed;
        uint8_t mode;
        uint8_t sspin;
};

#endif

//eof
