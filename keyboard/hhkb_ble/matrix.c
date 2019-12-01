/*
Copyright 2011 Jun Wako <wakojun@gmail.com>

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
#include <util/delay.h>
#include "print.h"
#include "debug.h"
#include "util.h"
#include "timer.h"
#include "matrix.h"
#include "hhkb_avr.h"
#include "suspend.h"
#include "lufa.h"
#include "ble51_task.h"

bool is_ver_jp = 0;

// matrix state buffer(1:on, 0:off)
static matrix_row_t *matrix;
static matrix_row_t *matrix_prev;
static matrix_row_t _matrix0[MATRIX_ROWS];
static matrix_row_t _matrix1[MATRIX_ROWS];

extern uint32_t kb_idle_timer; 


static uint8_t hhkb_matrix_rows;
static uint8_t hhkb_matrix_col_start;
static uint8_t hhkb_matrix_col_end;

void matrix_init(void)
{
    DDRE &= ~(1<<PE2);
    PORTE |= (1<<PE2);
    _delay_ms(2);
    is_ver_jp = (PINE&(1<<PE2))? 0 : 1;
    KEY_INIT();

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) _matrix0[i] = 0x00;
    for (uint8_t i=0; i < MATRIX_ROWS; i++) _matrix1[i] = 0x00;
    matrix = _matrix0;
    matrix_prev = _matrix1;
    if (is_ver_jp) {
        hhkb_matrix_rows = 16;
        hhkb_matrix_col_start = 2;
        hhkb_matrix_col_end = 7;
    } else {
        hhkb_matrix_rows = 8;
        hhkb_matrix_col_start = 0;
        hhkb_matrix_col_end = 8;
    }
}

uint8_t matrix_scan(void)
{
    uint8_t *tmp;

    tmp = matrix_prev;
    matrix_prev = matrix;
    matrix = tmp;

    for (uint8_t row = 0; row < hhkb_matrix_rows; row++) {
        for (uint8_t col = hhkb_matrix_col_start; col < hhkb_matrix_col_end; col++) {
            KEY_SELECT(row, col);
            _delay_us(5);

            // Not sure this is needed. This just emulates HHKB controller's behaviour.
            if (matrix_prev[row] & (1<<col)) {
                KEY_PREV_ON();
            }
            _delay_us(10);

            // NOTE: KEY_STATE is valid only in 20us after KEY_ENABLE.
            // If V-USB interrupts in this section we could lose 40us or so
            // and would read invalid value from KEY_STATE.
            uint8_t last = TIMER_RAW;

            KEY_ENABLE();

            // Wait for KEY_STATE outputs its value.
            // 1us was ok on one HHKB, but not worked on another.
            // no   wait doesn't work on Teensy++ with pro(1us works)
            // no   wait does    work on tmk PCB(8MHz) with pro2
            // 1us  wait does    work on both of above
            // 1us  wait doesn't work on tmk(16MHz)
            // 5us  wait does    work on tmk(16MHz)
            // 5us  wait does    work on tmk(16MHz/2)
            // 5us  wait does    work on tmk(8MHz)
            // 10us wait does    work on Teensy++ with pro
            // 10us wait does    work on 328p+iwrap with pro
            // 10us wait doesn't work on tmk PCB(8MHz) with pro2(very lagged scan)
            _delay_us(5);

            if (KEY_STATE()) {
                matrix[row] &= ~(1<<col);
            } else {
                matrix[row] |= (1<<col);
            }

            // Ignore if this code region execution time elapses more than 20us.
            // MEMO: 20[us] * (TIMER_RAW_FREQ / 1000000)[count per us]
            // MEMO: then change above using this rule: a/(b/c) = a*1/(b/c) = a*(c/b)
            if (TIMER_DIFF_RAW(TIMER_RAW, last) > 20/(1000000/TIMER_RAW_FREQ)) {
                matrix[row] = matrix_prev[row];
            }

            _delay_us(5);
            KEY_PREV_OFF();
            KEY_UNABLE();

            // NOTE: KEY_STATE keep its state in 20us after KEY_ENABLE.
            // This takes 25us or more to make sure KEY_STATE returns to idle state.

            // Looks like JP needs faster scan due to its twice larger matrix
            // or it can drop keys in fast key typing
            if (is_ver_jp) _delay_us(30);
            else _delay_us(75);

            if (BLE51_PowerState > 1) {
                if (matrix[row] > 0) return 100;
            }

            if (matrix[row] > 0) kb_idle_timer = timer_read32();
        }
    }
    return 1;
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void select_all_rows(void)
{
}

void unselect_rows(void)
{
}

bool suspend_wakeup_condition(void)
{
    static sleep_timer = 0;
    if (BLE51_PowerState >= 4) {
        if (++sleep_timer < 80) return false;
        else sleep_timer = 0;
    }

    //if (!KEY_POWER_STATE()) KEY_POWER_ON();
    KEY_POWER_ON();
    matrix_scan();
    if (matrix_scan() == 100) {
            if (BLE51_PowerState >=4) {
                PORTF |= (1<<PF4 |1<<PF1 | 1<<PF0);
                _delay_ms(400);
            }
            return true;
    }

    KEY_POWER_OFF();
    return false;
}

void suspend_power_down_action(void)
{
    KEY_POWER_OFF();
    PORTF &= ~(1<<PF4 |1<<PF1 | 1<<PF0); // turn off all leds.
}

void suspend_wakeup_init_action(void)
{
    ledmapu_state_restore();
}

