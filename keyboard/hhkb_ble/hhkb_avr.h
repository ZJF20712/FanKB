#ifndef HHKB_AVR_H
#define HHKB_AVR_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


// Timer resolution check
#if (1000000/TIMER_RAW_FREQ > 20)
#   error "Timer resolution(>20us) is not enough for HHKB matrix scan tweak on V-USB."
#endif


extern bool is_ver_jp;

/*
 * HHKB Matrix I/O
 *
 * row:     HC4051[A,B,C]  selects scan row0-7
 * row-ext: [En0,En1] row extention for JP
 * col:     LS145[A,B,C,D] selects scan col0-7 and enable(D)
 * key:     on: 0/off: 1
 * prev:    hysteresis control: assert(1) when previous key state is on
 */

/*
 * For HHKB BLE Controller (atmega32u4)
 *
 * row:     PB0-2
 * col:     PB3-5,6
 * key:     PD7(pull-uped)
 * prev:    PB7
 * power:   PD4(L:off/H:on)
 * row-ext: PC6,7 for HHKB JP(active low)
 */
static inline void KEY_ENABLE(void) { (PORTB &= ~(1<<6)); }
static inline void KEY_UNABLE(void) { (PORTB |=  (1<<6)); }
static inline bool KEY_STATE(void) { return (PIND & (1<<7)); }
static inline void KEY_PREV_ON(void) { (PORTB |=  (1<<7)); }
static inline void KEY_PREV_OFF(void) { (PORTB &= ~(1<<7)); }
//static inline bool KEY_POWER_STATE(void) { return (PORTD & (1<<4)); }

static inline void KEY_POWER_ON(void) {
    DDRB = 0xFF; PORTB = 0x40;          // change pins output
    DDRD |= (1<<6); // |1<<4); 
    PORTD &= ~(1<<6);    // PMOS FET switch on
    //PORTD |= (1<<4);  // 5v is not used by HHKB BLE Controller

    if (is_ver_jp) {
        DDRC  |= (1<<6|1<<7);
        PORTC |= (1<<6|1<<7);
    }
    /* Without this wait you will miss or get false key events. */
    _delay_us(500);                       // wait for powering up, no DC so it may need less delay.
}

static inline void KEY_POWER_OFF(void) {
    /* input with pull-up consumes less than without it when pin is open. */
    DDRB = 0x00; PORTB = 0xFF;          // change pins input with pull-up
    DDRD |= (1<<6); // |1<<4); 
    PORTD |= (1<<6);   // PMOS FET switch off
    //PORTD &= ~(1<<4);  // 5v off

    if (is_ver_jp) {
        DDRC  &= ~(1<<6|1<<7);
        PORTC |=  (1<<6|1<<7);
    }
}

static inline void KEY_INIT(void)
{
    /* row,col,prev: output */
    DDRB  = 0xFF;
    PORTB = 0x40;   // unable
    /* key: input with pull-up */
    DDRD  &= ~(1<<7);
    PORTD |=  (1<<7);

    if (is_ver_jp) {
        /* row extention for HHKB JP */
        DDRC  |= (1<<6|1<<7);
        PORTC |= (1<<6|1<<7);
    } else {
        /* input with pull up to save power */
        DDRC  &= ~(1<<6|1<<7);
        PORTC |=  (1<<6|1<<7);
    }
    KEY_UNABLE();
    KEY_PREV_OFF();

    KEY_POWER_ON();
}
static inline void KEY_SELECT(uint8_t ROW, uint8_t COL)
{
    PORTB = (PORTB & 0xC0) | (((COL) & 0x07)<<3) | ((ROW) & 0x07);
    if (is_ver_jp) {
        if ((ROW) & 0x08) PORTC = (PORTC & ~(1<<6|1<<7)) | (1<<6);
        else              PORTC = (PORTC & ~(1<<6|1<<7)) | (1<<7);
    }
}

#endif
