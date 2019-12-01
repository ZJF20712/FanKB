/*
Copyright 2012 Jun Wako <wakojun@gmail.com>

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
#include "rgblight.h"

extern rgblight_config_t rgblight_config;
extern bool rgb_layer_update;

void ledmapu_init(void){
} 

void ledmapu_led_on(uint8_t index){
} 

void ledmapu_led_off(uint8_t index) {
} 

void ledmapu_state_restore(void) {
}

void led_set(uint8_t usb_led) {
}

void hook_layer_change(uint32_t state)
{
    if (rgblight_config.enable && rgblight_config.mode == 4) {
        PORTD |= (1<<PD4);
        PORTB &= ~(1<<PB1);
        rgb_layer_update = true;
    }
}


void keyboard_function_led_set(uint8_t function_led) {
}
