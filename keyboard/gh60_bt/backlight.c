/*
Copyright 2013,2014 Kai Ryu <kai1103@gmail.com>

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
#ifdef SOFTPWM_LED_ENABLE
#include "softpwm_led.h"
#else
#include "breathing_led.h"
#endif
#include "hook.h"
#include "timer.h"
extern uint32_t kb_idle_timer; 
#ifdef BACKLIGHT_ENABLE

extern backlight_config_t backlight_config;

void backlight_enable(void);
void backlight_disable(void);
inline void backlight_set_raw(uint8_t raw);

static const uint8_t backlight_table[] PROGMEM = {
    0, 16, 128, 255
};


void backlight_set(uint8_t level)
{
#ifdef SOFTPWM_LED_ENABLE
    softpwm_enable();
    switch (level) {
        case 1:
        case 2:
            softpwm_led_enable_all();
#ifdef FADING_LED_ENABLE
            fading_led_disable_all();
#endif
            breathing_led_disable_all();
            softpwm_led_set_all(pgm_read_byte(&backlight_table[level]));
            break;
        case 3:
#ifdef FADING_LED_ENABLE
            fading_led_disable_all();
#endif
            breathing_led_disable_all();
            softpwm_led_disable_all();
            PORTF &= ~(1<<PF5);
            break;
        case 4:
        case 5:
        case 6:
            softpwm_led_enable_all();
#ifdef FADING_LED_ENABLE
            fading_led_disable_all();
#endif
            breathing_led_enable_all();
            breathing_led_set_duration(6 - level);
            break;
#ifdef FADING_LED_ENABLE
        case 7:
            softpwm_led_enable_all();
            fading_led_enable_all();
            breathing_led_disable_all();
            fading_led_set_direction_all(FADING_LED_FADE_IN);
            fading_led_set_duration(1);
            break;
        case 8:
            softpwm_led_enable_all();
            fading_led_enable_all();
            breathing_led_disable_all();
            fading_led_set_direction_all(FADING_LED_FADE_OUT);
            fading_led_set_duration(1);
            break;
#endif
        case 0:
        default:
#ifdef FADING_LED_ENABLE
            fading_led_disable_all();
#endif
            breathing_led_disable_all();
            softpwm_led_set_all(0);
            softpwm_led_disable_all();
            break;
    }
#endif
}

#ifdef SOFTPWM_LED_ENABLE
void softpwm_led_init(void)
{
    DDRF |= (1<<PF5);
}

void softpwm_led_on(uint8_t index)
{
    PORTF &= ~(1<<PF5);
}

void softpwm_led_off(uint8_t index)
{
    PORTF |= (1<<PF5);
}

#endif

#ifdef SOFTPWM_LED_ENABLE
#ifdef FADING_LED_ENABLE
void hook_matrix_change(keyevent_t event)
{
    kb_idle_timer = timer_read32();
    if (backlight_config.enable) {
        if (backlight_config.level == 7) {
            if (event.pressed) {
                fading_led_set_delay_all(32);
                softpwm_led_decrease_all(16);
            }
        }
        if (backlight_config.level == 8) {
            if (event.pressed) {
                fading_led_set_delay_all(32);
                softpwm_led_increase_all(16);
            }
        }
    }
} 

#endif


#endif

#endif
