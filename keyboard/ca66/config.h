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

#ifndef CONFIG_H
#define CONFIG_H

/* USB Device descriptor parameter */
#define VENDOR_ID       0xFEED
#define PRODUCT_ID      0x6066
#define DEVICE_VER      0x0001
#define MANUFACTURER    YANG
#define PRODUCT         CA66 Keyboard
#define DESCRIPTION     t.m.k. keyboard firmware for CA66

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 15

/* define if matrix has ghost */
//#define MATRIX_HAS_GHOST

/* Set 0 if debouncing isn't needed */
#define DEBOUNCE    5
/* number of backlight levels */
#ifdef BREATHING_LED_ENABLE
#define BACKLIGHT_LEVELS 8
#else
#define BACKLIGHT_LEVELS 3
#endif
/* number of leds */

/* enable customized backlight logic */
#define SOFTPWM_LED_FREQ 80
#define CUSTOM_LED_ENABLE


/* ws2812 RGB LED */
#define ws2812_PORTREG  PORTB
#define ws2812_DDRREG   DDRB
#define ws2812_pin PB1
#define RGBLED_NUM 6     // Number of LEDs

/* key combination for command */
#define IS_COMMAND() ( \
    keyboard_report->mods == (MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT)) \
)

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
//#define NO_ACTION_MACRO
//#define NO_ACTION_FUNCTION

#endif
