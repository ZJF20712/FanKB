#ifndef BLE660C_980C_H
#define BLE660C_980C_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timer.h"
#include "unimap.h"


// Timer resolution check
#if (1000000/TIMER_RAW_FREQ > 20)
#   error "Timer resolution(>20us) is not enough for HHKB matrix scan tweak on V-USB."
#endif

extern bool is_980c;

/*
 * Pin configuration for ATMega32U4
 *
 * Row:     PD4-6, 7(~EN)
 * Col:     PB0-2, 3(Z5 ~EN), 4(Z4 ~EN)
 * Key:     PC6(pull-uped)
 * Hys:     PC7
 * power:   PF7(L:on/H:off)
*/
static inline void KEY_ENABLE(void) { is_980c? (PORTD &= ~(1<<4)) : (PORTD &= ~(1<<7)); }
static inline void KEY_UNABLE(void) { is_980c? (PORTD |= (1<<4)) : (PORTD |=  (1<<7)); }
static inline bool KEY_STATE(void) { return (PINC & (1<<6)); }
static inline void KEY_HYS_ON(void) { (PORTC |=  (1<<7)); }
static inline void KEY_HYS_OFF(void) { (PORTC &= ~(1<<7)); }
static inline bool KEY_POWER_STATE(void) { return (!(PORTF & (1<<7))); }

static inline void KEY_POWER_ON(void) {

    DDRB |= 0b00011111;  // change col pins output
    DDRC |= (1<<7);
    if (is_980c) {
        DDRD |= 0b01110000;  
    } else {
        DDRD |= 0b11110000;  // change row pins output
    }
    DDRF |= (1<<7); PORTF &= ~(1<<7);    // PMOS switch on
    /* Without this wait you will miss or get false key events. */
    _delay_us(500);                       // wait for powering up, no DC so it may need less delay.
}

static inline void KEY_POWER_OFF(void) {
    /* input with pull-up consumes less than without it when pin is open. */
    DDRB  &= ~0b00011111;
    PORTB |=  0b00011111;
    DDRC  &= ~(1<<7);
    PORTC |=  (1<<7);
    if (is_980c) {
        DDRD  &= ~0b01100000;
        PORTD |=  0b01100000;
    } else {
        DDRD  &= ~0b11110000;
        PORTD |=  0b11110000;
    }
    
    DDRF |= (1<<7); PORTF |= (1<<7);    // PMOS switch off

}

static inline void KEY_INIT(void)
{
    KEY_POWER_ON();
    /* Key: input with pull-up */
    DDRC  &= ~(1<<6);
    PORTC |=  (1<<6);


    KEY_UNABLE();
    KEY_HYS_OFF();
}
static inline void SET_ROW(uint8_t ROW)
{
    if (is_980c) {
        (ROW & (1<<0)) ? (PORTD |= (1<<5)) : (PORTD &= ~(1<<5));
        (ROW & (1<<1)) ? (PORTD |= (1<<6)) : (PORTD &= ~(1<<6));
        (ROW & (1<<2)) ? (PORTB |= (1<<0)) : (PORTB &= ~(1<<0));
    } else {
        PORTD = (PORTD & 0x0F) | (1<<7) | ((ROW & 0x07) << 4);
    }
}
static inline void SET_COL(uint8_t COL)
{
    if (is_980c) {
        PORTB = (PORTB & 0b11100001) | ((COL<<1) & 0b00011110);
    } else {
        PORTB = (PORTB & 0xE0) | ((COL & 0x08) ? 1<<4 : 1<<3) | (COL & 0x07);
    }
}

#endif
