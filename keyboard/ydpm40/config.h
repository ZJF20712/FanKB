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


#define VENDOR_ID       0xFEED
#define PRODUCT_ID      0x0240
#define DEVICE_VER      0x0001
#define MANUFACTURER    YANG
#define PRODUCT         YDP40/YDM40 keyboard
#define DESCRIPTION     t.m.k. keyboard firmware for YDP40

#define NO_UART
#define SUSPEND_ACTION
#define BREATHING_LED_TIMER1

/* matrix size */
#define MATRIX_ROWS 4  
#define MATRIX_COLS 12   

#define BACKLIGHT_LEVELS 6

#define DEBOUNCE 5
#define LEDMAPU_ENABLE
#define TAPPING_TERM 180
#define TAPPING_TOGGLE  2
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

#define NO_SYSTEMKEY
#endif
