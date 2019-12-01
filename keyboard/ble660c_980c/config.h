/*
Copyright 2017 Jun Wako <wakojun@gmail.com>

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
#define PRODUCT_ID      0x19C1
#define DEVICE_VER      0x0001
#define MANUFACTURER    TMK.
#define PRODUCT         BLE660C/BLE980C Keyboard
#define DESCRIPTION     t.m.k. keyboard firmware for FC660C&FC980C BLE Controller


#define MATRIX_ROWS 8
#define MATRIX_COLS 16


#define LEDMAPU_ENABLE

/* period of tapping(ms) */


#define SUSPEND_ACTION
#define TAPPING_TERM 180
#define TAPPING_TOGGLE  2




/* key combination for command */
#define IS_COMMAND() ( \
    keyboard_report->mods == (MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT)) \
)

/*
 * Hardware Serial(UART)
 */
#ifdef __AVR_ATmega32U4__
    /* iom32u4.h has no definition of UCSR1D. copy from iom32u2.h */
    #define UCSR1D _SFR_MEM8(0xCB)
    #define RTSEN 0
    #define CTSEN 1

    #define SERIAL_UART_BAUD        76800
    #define SERIAL_UART_DATA        UDR1
    #define SERIAL_UART_UBRR        ((F_CPU/(8.0*SERIAL_UART_BAUD)-1+0.5))
    #define SERIAL_UART_RXD_VECT    USART1_RX_vect
    #define SERIAL_UART_TXD_READY   (UCSR1A&(1<<UDRE1))
    #define SERIAL_UART_INIT()      do { \
        cli(); \
        UBRR1L = (uint8_t) SERIAL_UART_UBRR;       /* baud rate */ \
        UBRR1H = ((uint16_t)SERIAL_UART_UBRR>>8);  /* baud rate */ \
        UCSR1B |= (1<<RXCIE1) | (1<<RXEN1); /* RX interrupt, RX: enable */ \
        UCSR1B |= (0<<TXCIE1) | (1<<TXEN1); /* TX interrupt, TX: enable */ \
        UCSR1C |= (0<<UPM11) | (0<<UPM10);  /* parity: none(00), even(01), odd(11) */ \
        UCSR1A |= (1<<U2X1); /* 2x speed */ \
        sei(); \
    } while(0)
#else
    #error "USART configuration is needed."
#endif
/* BT Power Control */
#define BT_POWERED    !(PORTE&(1<<PE6))
#define bt_power_init()    do { DDRE |= (1<<PE6); PORTD &= ~(1<<PD5);} while(0)
#define turn_off_bt()    do { PORTE |= (1<<PE6); } while(0)
#define turn_on_bt()    do { PORTE &= ~(1<<PE6); } while(0)

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
#define NO_DEBUG

/* disable print */
#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
//#define NO_ACTION_MACRO
//#define NO_ACTION_FUNCTION

#endif
