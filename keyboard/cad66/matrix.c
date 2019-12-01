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

/*
 * scan matrix
 */
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"

#include "timer.h"
#include "74hc595.h"

#ifndef DEBOUNCE
#define DEBOUNCE 5
#endif
extern debug_config_t debug_config;

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];


#define DEBOUNCE_MASK ((1 << DEBOUNCE) - 1)
uint8_t matrix_current_row = 0;
static uint16_t matrix_row_timestamp;
static uint8_t matrix_debouncing[MATRIX_ROWS][MATRIX_COLS];

static uint16_t matrix_scan_timestamp;


inline
uint8_t matrix_rows(void)
{
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void)
{
    return MATRIX_COLS;
}

void matrix_init(void)
{
    // disable JTAG
    MCUCR |= (1<<JTD);
    MCUCR |= (1<<JTD);

    TWIInit();
    DDRB  |= (1<<PB0);
    PORTB |= (1<<PB0);
    IS31FL3731_init(0x74);
    
    rgblight_init();
    
    timer_init();
    _delay_ms(1);

    matrix_scan_timestamp = timer_read();
    init_cols();
    for (uint8_t i = 0; i < matrix_rows(); i++) {
        matrix[i] = 0;
        matrix_current_row = 0;
        matrix_row_timestamp = timer_read();
        for (uint8_t j = 0; j < matrix_cols(); j++) {
            matrix_debouncing[i][j] = 0;
        }
    }
}

uint8_t matrix_scan(void)
{
    static uint16_t elapsed;
    if (matrix_current_row == 0) {
        elapsed = timer_elapsed(matrix_row_timestamp);
    }
    if (elapsed >= 1) {
        if (matrix_current_row == 0) {
            matrix_row_timestamp = timer_read();
        }
        select_row(matrix_current_row);
        _delay_us(5); // // without this wait read unstable value. tmk default value is 30. 10 is OK.
        for (uint8_t i = 0; i < matrix_cols(); i++) {
            uint8_t *debounce = &matrix_debouncing[matrix_current_row][i];
            uint8_t col = get_col(i);
            uint8_t count = elapsed;
            do {
                *debounce <<= 1;
                *debounce |= col;
            } while (--count);
            matrix_row_t *row = &matrix[matrix_current_row];
            matrix_row_t mask = ((matrix_row_t)1 << i);
            switch (*debounce & DEBOUNCE_MASK) {
                case DEBOUNCE_MASK:
#if DEBOUNCE > 1
                case (DEBOUNCE_MASK >> 1):
#if DEBOUNCE > 2
                case (DEBOUNCE_MASK >> 2):
#if DEBOUNCE > 3
                case (DEBOUNCE_MASK >> 3):
#if DEBOUNCE > 4
                case (DEBOUNCE_MASK >> 4):
#if DEBOUNCE > 5
                case (DEBOUNCE_MASK >> 5):
#if DEBOUNCE > 6
                case (DEBOUNCE_MASK >> 6):
#if DEBOUNCE > 7
                case (DEBOUNCE_MASK >> 7):
#if DEBOUNCE > 8
                case (DEBOUNCE_MASK >> 8):
#if DEBOUNCE > 9
                case (DEBOUNCE_MASK >> 9):
#if DEBOUNCE > 10
                case (DEBOUNCE_MASK >> 10):
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
                    if ((*row & mask) == 0) {
                        *row |= mask;
                    }
                    break;
#if DEBOUNCE > 1
                case ((DEBOUNCE_MASK << 1) & DEBOUNCE_MASK):
#if DEBOUNCE > 2
                case ((DEBOUNCE_MASK << 2) & DEBOUNCE_MASK):
#if DEBOUNCE > 3
                case ((DEBOUNCE_MASK << 3) & DEBOUNCE_MASK):
#if DEBOUNCE > 4
                case ((DEBOUNCE_MASK << 4) & DEBOUNCE_MASK):
#if DEBOUNCE > 5
                case ((DEBOUNCE_MASK << 5) & DEBOUNCE_MASK):
#if DEBOUNCE > 6
                case ((DEBOUNCE_MASK << 6) & DEBOUNCE_MASK):
#if DEBOUNCE > 7
                case ((DEBOUNCE_MASK << 7) & DEBOUNCE_MASK):
#if DEBOUNCE > 8
                case ((DEBOUNCE_MASK << 8) & DEBOUNCE_MASK):
#if DEBOUNCE > 9
                case ((DEBOUNCE_MASK << 9) & DEBOUNCE_MASK):
#if DEBOUNCE > 10
                case ((DEBOUNCE_MASK << 10) & DEBOUNCE_MASK):
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
                    break;
                case 0:
                    if ((*row & mask) != 0) {
                        *row &= ~mask;
                    }
                    break;
                default:
                    print("bounce!: ");
                    pbin(*debounce & DEBOUNCE_MASK);
                    print("\n");
                    break;
            }
        }
        matrix_current_row++;
        if (matrix_current_row >= matrix_rows()) {
            matrix_current_row = 0;
        }
    }
    return 1;
}


inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)1<<col));
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 01234567\n");
    for (uint8_t row = 0; row < matrix_rows(); row++) {
        phex(row); print(": ");
        print_bin_reverse8(matrix_get_row(row));
        print("\n");
    }
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop32(matrix[i]);
    }
    return count;
}
