/*

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
#define PRODUCT_ID      0x1840
#define DEVICE_VER      0x0001
#define MANUFACTURER    YANG
#define PRODUCT         BLE40 Keyboard
#define DESCRIPTION     t.m.k. keyboard firmware for BLE40

/* key matrix size */
#define MATRIX_ROWS 4
#define MATRIX_COLS 11

/* define if matrix has ghost */
//#define MATRIX_HAS_GHOST

/* Set 0 if debouncing isn't needed */
#define DEBOUNCE    5
#define LEDMAPU_ENABLE
#define SUSPEND_ACTION
/* number of backlight levels */

#define TAPPING_TERM 180
#define TAPPING_TOGGLE  2


/* Locking resynchronize hack */

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
#define BT_POWERED    !(PORTD&(1<<PD5))
#define bt_power_init()    do { DDRD |= (1<<PD5); PORTD &= ~(1<<PD5); } while(0)
#define turn_off_bt()    do { PORTD |= (1<<PD5); } while(0)
#define turn_on_bt()    do { PORTD &= ~(1<<PD5); } while(0)

#define ws2812_PORTREG  PORTB
#define ws2812_DDRREG   DDRB
#define ws2812_pin PB3
#define RGBLED_NUM 6     // Number of LEDs


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
