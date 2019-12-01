/*
Copyright 2016 Kai Ryu <kai1103@gmail.com>

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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "backlight.h"
#include "action.h"

#ifdef BACKLIGHT_ENABLE

void backlight_enable(void);
void backlight_disable(void);

extern backlight_config_t backlight_config;

/* Backlight pin configuration
 * Backlight: PD1
 */
 
void backlight_enable(void)
{
    DDRD  |= (1<<PD1);
	PORTD &= ~(1<<PD1);
}

void backlight_disable(void)
{
    DDRD &= ~(1<<PD1);
	PORTD |= (1<<PD1);
}

void backlight_set(uint8_t level)
{
 if (level == 1) backlight_enable();
 else backlight_disable();
}


#endif
