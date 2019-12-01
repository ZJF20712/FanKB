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
#include "print.h"
#include "debug.h"
#define LEDMAP_0 LEDMAP_CAPSLOCK
#define LEDMAP_2 LEDMAP_L1
#define LEDMAP_4 LEDMAP_L2

uint8_t indicator[8] = {LEDMAP_0,LEDMAP_1,LEDMAP_2,LEDMAP_3,LEDMAP_4,LEDMAP_5,LEDMAP_6,LEDMAP_7};
uint8_t indicator_state = 0;

/* LED pin configuration
 */
void ledmapu_init(void)
{
    DDRF  |=  (1<<PF4 |1<<PF1 | 1<<PF0);
    PORTF &= ~(1<<PF4 |1<<PF1 | 1<<PF0);
} 

void ledmapu_led_on(uint8_t index)
{
    switch (index) {
        case 0:
            PORTF |= (1<<PF4);
            break;
        case 2:
            PORTF |= (1<<PF1);
            break;
        case 4:
            PORTF |= (1<<PF0);
            break;
        default:
            break;
    }
    indicator_state |= (1<<index); 
} 
void ledmapu_led_off(uint8_t index)
{
    switch (index) {
        case 0:
            PORTF &= ~(1<<PF4);
            break;
        case 2:
            PORTF &= ~(1<<PF1);
            break;
        case 4:
            PORTF &= ~(1<<PF0);
            break;
        default:
            break;
    }
    indicator_state &= ~(1<<index); 
} 

void ledmapu_state_restore(void) {
    for (uint8_t i=0; i<8; i++) {
        if (indicator_state & (1<<i)) ledmapu_led_on(i);
        else ledmapu_led_off(i);
    }
}

void led_set(uint8_t usb_led)
{
//in led.h USB_LED_NUM_LOCK=0 USB_LED_CAPS_LOCK=2 USB_LED_SCROLL_LOCK=3
    for(uint8_t i=0; i<8; i++) {
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
    for(uint8_t i=0; i<8; i++) {
        if ((indicator[i]>0) && (indicator[i]<8)){
            if((1<<indicator[i] & state) != 0) {  
                ledmapu_led_on(i);
            } else {
                ledmapu_led_off(i);
            }
        }
    }
}

void hook_default_layer_change(uint32_t default_layer_state)
{
    for(uint8_t i=0; i<8; i++) {
        if ((indicator[i]>8) && (indicator[i]<16)){
            if((1<<(indicator[i]-8) & default_layer_state) != 0) { 
                ledmapu_led_on(i);
            } else {
                ledmapu_led_off(i);
            }
        }
    }
}

void keyboard_function_led_set(uint8_t function_led)
{
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
