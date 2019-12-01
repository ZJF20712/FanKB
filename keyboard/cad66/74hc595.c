/*
Copyright 2017 Kai Ryu <kai1103@gmail.com>

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

#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "74hc595.h"
#include "ble51_task.h"
#include "debug.h"

#define DS_PL_INIT()    (DS_PL_DDR  |=  (1<<DS_PL_BIT))
#define DS_PL_HI()      (DS_PL_PORT |=  (1<<DS_PL_BIT))
#define DS_PL_LO()      (DS_PL_PORT &= ~(1<<DS_PL_BIT))

#define SHCP_CP_INIT()  (SHCP_CP_DDR  |=  (1<<SHCP_CP_BIT))
#define SHCP_CP_HI()    (SHCP_CP_PORT |=  (1<<SHCP_CP_BIT))
#define SHCP_CP_LO()    (SHCP_CP_PORT &= ~(1<<SHCP_CP_BIT))

#define KEY_STCP_INIT() (KEY_STCP_DDR  |=  (1<<KEY_STCP_BIT))
#define KEY_STCP_HI()   (KEY_STCP_PORT |=  (1<<KEY_STCP_BIT))
#define KEY_STCP_LO()   (KEY_STCP_PORT &= ~(1<<KEY_STCP_BIT))

#define PULSE_WIDTH 2

#define CLOCK_PULSE(w) \
    do { \
        SHCP_CP_HI(); \
        _delay_us(w); \
        SHCP_CP_LO(); \
        _delay_us(w); \
    } while(0)

#define COL_LOAD(w) \
    do { \
        DS_PL_HI(); \
        _delay_us(w); \
        DS_PL_LO(); \
        _delay_us(w); \
        DS_PL_HI(); \
        _delay_us(w); \
    } while(0)

#define KEY_LATCH(w) \
    do { \
        KEY_STCP_HI(); \
        _delay_us(w); \
        KEY_STCP_LO(); \
        _delay_us(w); \
    } while(0)

#define KEY_DISABLE()   KEY_STCP_HI()
#define KEY_ENABLE()    KEY_STCP_LO()
#define ROW_BANK 2

static uint8_t cols = 0;

static union {
    uint16_t u16;
    uint8_t u8[2];
} row_state;


static void shift_out(uint8_t data);

static bool debug_shift_out = false;

void init_cols(void)
{
    DS_PL_INIT();
    SHCP_CP_INIT();
    KEY_STCP_INIT();
    DS_PL_LO();
    SHCP_CP_LO();
    KEY_STCP_LO();
    DDRF  &= ~0b11110010;
    PORTF |= 0b11110010;
}

uint8_t get_col(uint8_t col)
{
    switch (col) {
        case 0: return PINF&(1<<PF1) ? 0 : 1;
        case 1: return PINF&(1<<PF4) ? 0 : 1;
        case 2: return PINF&(1<<PF5) ? 0 : 1;
        case 3: return PINF&(1<<PF6) ? 0 : 1;
        case 4: return PINF&(1<<PF7) ? 0 : 1;
        default: return 0;
    }
}

void select_row(uint8_t row)
{
  CLOCK_PULSE(PULSE_WIDTH);
    if (row == 0) {
        for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
            CLOCK_PULSE(PULSE_WIDTH);
        }
        DS_PL_LO();
        CLOCK_PULSE(PULSE_WIDTH);
        DS_PL_HI();
    } 
    KEY_LATCH(PULSE_WIDTH);
}

void select_all_rows()
{
    if (BLE51_PowerState == 10) {
        unselect_rows();
    } else {
        DS_PL_LO();
        for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
            CLOCK_PULSE(PULSE_WIDTH);
        }
        KEY_LATCH(PULSE_WIDTH);
    }
}

void unselect_rows()
{
    DS_PL_HI();
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        CLOCK_PULSE(PULSE_WIDTH);
    }
    KEY_LATCH(PULSE_WIDTH);
}

bool suspend_wakeup_condition(void)
{
    for (uint8_t i = 0; i < MATRIX_COLS; i++) {
        if (get_col(i)) return true;
    }
}
