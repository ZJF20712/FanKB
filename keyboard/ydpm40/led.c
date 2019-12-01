/*
Copyright 2011 Jun Wako <wakojun@gmail.com>

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
#include "stdint.h"
#include "led.h"
#include <avr/pgmspace.h>
#include "print.h"
#include "debug.h"


uint8_t indicator[] = {0,0,0,0,0,0,0,0};
/* LED pin configuration
 */
 
/* LED pin configration
 */
 
void ledmapu_init(void)
{
    DDRD  |=  (1<<PD0);
    PORTD &= ~(1<<PD0);
	for(uint8_t i=0;i<8;i++) {
		indicator[i] = pgm_read_word(0x6a00+i/2*0x100+i%2);
	}
} 
void ledmapu_led_on(uint8_t index)
{
	switch (index) {
		case 0:
			PORTD |= (1<<PD0);
            break;
		default:
			break;
	}
} 

void ledmapu_led_off(uint8_t index)
{
	switch (index) {
		case 0:
			PORTD &= ~(1<<PD0);
            break;
		default:
			break;
	}
} 

void led_set(uint8_t usb_led)
{
//in led.h USB_LED_NUM_LOCK=0 USB_LED_CAPS_LOCK=2 USB_LED_SCROLL_LOCK=3
	for(uint8_t i=0;i<8;i++) {
		if ((indicator[i]>15) && (indicator[i]<19)) {
    		if (usb_led & (1<<(indicator[i]-16))) {
	        	ledmapu_led_on(i);
		    } else {
		        ledmapu_led_off(i);
			}
		}
    }
}

void hook_layer_change(uint32_t state)
{
	for(uint8_t i=0;i<8;i++) {
		phex(indicator[i]);
		print(" ");
    }
    print("\n");
	for(uint8_t i=0;i<8;i++) {
	    if ((indicator[i]>0) && (indicator[i]<8)){
			if((1<<indicator[i] & state) != 0) {  
		        ledmapu_led_on(i);
		    } else {
		        ledmapu_led_off(i);
		    }
		}
	}
}

void hook_default_layer_change(uint32_t default_layer_state) {
	for(uint8_t i=0;i<8;i++) {
		phex(indicator[i]);
		print(" ");
	}
    print("\n");
	for(uint8_t i=0;i<8;i++) {
	    if ((indicator[i]>8) && (indicator[i]<16)){
			if((1<<(indicator[i]-8) & default_layer_state) != 0) { 
		        ledmapu_led_on(i);
		    } else {
		        ledmapu_led_off(i);
		    }
		}
	}
}

void keyboard_function_led_set(uint8_t function_led) {
	for(uint8_t i=0;i<8;i++) {
		if ((indicator[i]>20) && (indicator[i]<29)) {
    		if (function_led & (1<<(indicator[i]-21))) {
	        	ledmapu_led_on(i);
		    } else {
		        ledmapu_led_off(i);
			}
		}
    }
}
