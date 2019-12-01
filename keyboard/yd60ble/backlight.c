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

#include "breathing_led.h"
#include "action.h"
#include "hook.h"
#include "timer.h"
#include "debug.h"

extern uint32_t kb_idle_timer; 
#ifdef BACKLIGHT_ENABLE

void backlight_enable(void);
void backlight_disable(void);
inline void backlight_set_raw(uint8_t raw);

static const uint8_t backlight_table[] PROGMEM = {
    255, 239, 128, 0
};

static bool fading_led_on = false;
extern bool breathing_led_on;
extern backlight_config_t backlight_config;
extern const uint8_t breathing_table[256] PROGMEM;
static uint8_t index = 0;

/* Backlight pin configuration
 * Backlight: PC6 - OCR3A
 */
#ifndef SOFTPWM_LED_ENABLE
void backlight_enable(void)
{
    // Turn on PWM
    cli();
    DDRC  |= (1<<PC6);
    TCCR3A |= ((1<<WGM30) | (1<<COM3A1));
    TCCR3B |= ((1<<CS31) | (1<<CS30));
    sei();
}
#endif

#ifndef SOFTPWM_LED_ENABLE
void backlight_disable(void)
{
    // Turn off PWM
    cli();
    DDRC &= ~ (1<<PC6);
    TCCR3A &= ~((1<<WGM30) | (1<<COM3A1));
    TCCR3B &= ~((1<<CS31) | (1<<CS30));
    OCR3A= 0;
    sei();
}
#endif

void backlight_set(uint8_t level)
{
    breathing_led_enable();
    switch (level) {
        case 1:
        case 2:
        case 3:
            breathing_led_on = false;
            fading_led_on = false;
            backlight_enable();
            //breathing_led_disable();
            backlight_set_raw(pgm_read_byte(&backlight_table[level]));
            break;
        case 4:
        case 5:
        case 6:
            breathing_led_on = true;
            fading_led_on = false;
            backlight_enable();
            breathing_led_set_duration(7 - level);
            break;
        case 7:
        case 8:
            backlight_enable();
            breathing_led_on = false;
            fading_led_on = true;
            break;
        case 0:
        default:
            //breathing_led_disable();
            backlight_disable();
            break;
    }
}

#ifdef BREATHING_LED_ENABLE
void breathing_led_set_raw(uint8_t raw)
{
    backlight_set_raw(raw);
}
#endif

inline void backlight_set_raw(uint8_t raw)
{
    OCR3A = raw;
}


void hook_matrix_change(keyevent_t event)
{
    kb_idle_timer = timer_read32();
    if (backlight_config.enable) {
        if (backlight_config.level == 7) {
            if (event.pressed) {
                index-=10;
                if (index <10) index = 10;
            }
        }
        if (backlight_config.level == 8) {
            if (event.pressed) {
                index+=10;
                if (index >118 ) index = 118;
            }
        }
    }
}


#ifdef PWM_CUSTOM_LED_ENABLE
void pwm_led_custom(void)
{
    if (fading_led_on) { 
        static uint8_t step = 0;
        step++;
        if (step > 5) {
            step = 0;
            breathing_led_set_raw(pgm_read_byte(&breathing_table[index]));
            if (backlight_config.level == 7) {   //fading in
                index++;
                if (index > 128) index = 128;
            }
            if (backlight_config.level == 8) {   //fading out
                index--;
                if (index < 0) index = 0;
            }
        }
    }
}
#endif

#endif
