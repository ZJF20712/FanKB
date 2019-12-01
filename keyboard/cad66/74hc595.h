/*
Copyright 2017 Kai Ryu <kai1103@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef EK2000_H
#define EK2000_H

#include <stdint.h>
#include "matrix.h"

#define DS_PL_DDR       DDRC
#define DS_PL_PORT      PORTC
#define DS_PL_BIT       6


#define SHCP_CP_DDR     DDRC
#define SHCP_CP_PORT    PORTC
#define SHCP_CP_BIT     7

#define KEY_STCP_DDR    DDRE
#define KEY_STCP_PORT   PORTE
#define KEY_STCP_BIT    2


void read_cols(void);
uint8_t get_col(uint8_t col);
void init_cols(void);
void unselect_rows(void);
void select_row(uint8_t row);
void select_all_row();
void unselect_all_row();
#endif
