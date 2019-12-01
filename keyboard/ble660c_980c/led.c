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
extern bool is_980c;

/* LED pin configuration
 */
void ledmapu_init(void)
{
    DDRB  |= (1<<PB6 |1<<PB5);
    if (is_980c) {
        PORTB &= ~(1<<PB6 |1<<PB5);
        DDRD  |=  (1<<PD7);
        PORTD &= ~(1<<PD7);
    } else {
        PORTB |= (1<<PB6 |1<<PB5);
    }
}
 
void ledmapu_led_on(uint8_t index)
{
    switch (index) {
        case 0: //insert of 660c, num lock of 980c
            if (is_980c) PORTD |= (1<<PD7);
            else PORTB &= ~(1<<PB6);
            break;
        case 2: //caps lock of 660c and 980c
            if (is_980c) PORTB |= (1<<PB6);
            else PORTB &= ~(1<<PB5);
            break;
        case 4: //scroll lock of 980c
            if (is_980c) PORTB |= (1<<PB5);
            break;
        default:
            break;
    }
    indicator_state |= (1<<index); 
} 
void ledmapu_led_off(uint8_t index)
{
    switch (index) {
        case 0: //insert of 660c, num lock of 980c
            if (is_980c) PORTD &= ~(1<<PD7);
            else PORTB |= (1<<PB6);
            break;
        case 2: //caps lock of 660c and 980c
            if (is_980c) PORTB &= ~(1<<PB6);
            else PORTB |= (1<<PB5);
            break;
        case 4: //scroll lock of 980c
            if (is_980c) PORTB &= ~(1<<PB5);
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
