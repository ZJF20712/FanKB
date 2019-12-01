/*
Copyright 2018 yangdigi <drk@live.com>

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
#include "action.h"
#include "hook.h"
#include "timer.h"
#include "IS31FL3731.h"
#include "ble51_task.h"

#ifdef BACKLIGHT_ENABLE

#define backlight_enabled (backlight_config.enable)  
#define R_MATRIX_ROWS 5
#define R_MATRIX_COLS 15
#define BACKLIGHT_EFFECT_HORSE_LENGTH 3

extern uint32_t kb_idle_timer; 

void backlight_enable(void);
void backlight_disable(void);
void backlight_loop_task(void);

inline void backlight_set_raw(uint8_t raw);

static const uint8_t backlight_table[] PROGMEM = {
    0, 20, 100, 200
};

static int8_t wave_col_l[4] = {0};
static int8_t wave_col_r[4] = {0};
static uint8_t wave_num = 0;
const uint8_t SLED_MATRIX_MASK[5][15] = 
{
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14 },
    {15,16,17,18,19,20,21,22,23,24,25,26,27,28,29 },
    {30,31,32,33,34,35,36,37,38,39,40,41,42,43,44 },
    {45,46,47,48,49,50,51,52,53,54,55,71,56,57,58 },
    {59,60,61,62,63,71,64,71,65,66,67,71,68,69,70 },
};

extern const uint8_t RGBLED_BREATHING_TABLE[128] PROGMEM;
extern const uint8_t g_pwm_control_register[SLED_NUM] PROGMEM;

uint8_t fading_single_key[SLED_NUM+1] = {0};

extern backlight_config_t backlight_config;
extern uint8_t g_pwm_buffer[144];
extern bool g_pwm_buffer_update_required;

bool backlight_layer_update = false;
extern uint32_t layer_state; 
static uint8_t horse_race_remain = 0;
bool backlight_update_once = false;

/* Backlight pin configuration
 * ISSI_SDB: PB0
 */
void backlight_enable(void)
{
    DDRB  |= (1<<PB0);
    PORTB |= (1<<PB0);
}
void backlight_disable(void)
{
    backlight_clear();
    backlight_update_once = 1;
    if (PORTF&(1<<PF0)) {  
        PORTB &= ~ (1<<PB0);
    }

}

void backlight_set(uint8_t level)
{
    switch (level) {
        case 1 ... 3:
            backlight_enable();
            backlight_set_raw(pgm_read_byte(&backlight_table[level]));
            break;
        case 4 ... 11:
            backlight_enable();
            backlight_clear();
            g_pwm_buffer_update_required = 1;
            break;
        case 12 ... 13:
            backlight_enable();
            break;
        case 0:
        default:
            backlight_disable();
            break;
    }
}

void backlight_task(void) 
{
    if (backlight_enabled && backlight_config.level >= 4) {
        // Mode = 1, static light, do nothing here
        switch (backlight_config.level) {
            case 4:
                backlight_effect_click_fading();
                break;
            case 5:
                backlight_effect_click_wave();
                break;
            case 6:
                backlight_effect_layer();
                break;
            case 7 ... 8:
                backlight_effect_stars(backlight_config.level-6);
                break;
            case 9 ... 10:
                backlight_effect_raindrop(backlight_config.level-8);
                break;
            case 11:
                backlight_effect_horse_race();
                break; 
            case 12 ... 13:
                backlight_effect_breathing(backlight_config.level-10);
                break;
            default:
                break;
        }
    }
}


inline void backlight_set_raw(uint8_t raw)
{
    IS31FL3731_set_pwm_all(raw);
}

inline void backlight_clear(void)
{
    for (uint8_t i=0; i<SLED_NUM; i++) {
        g_pwm_buffer[ pgm_read_byte(&g_pwm_control_register[i])-0x24 ] = 0;
    }
}

void hook_matrix_change(keyevent_t event)
{
    kb_idle_timer = timer_read32();
    if (backlight_enabled && (backlight_config.level == 4 || backlight_config.level == 5)) {
        if (event.pressed == 1) {
            backlight_matrix(event.key.col, event.key.row);
        }
    }
} 

inline
void backlight_matrix(uint8_t row, uint8_t col)
{
    if (backlight_config.level == 4) {
        fading_single_key[ SLED_MATRIX_MASK[row][col] ] = 126;
    } else if (backlight_config.level == 5) {
        if (wave_col_l[(wave_num + 1)%4] < -2 && wave_col_r[(wave_num + 1)%4] >= 17) {
            if (++wave_num >= 4) wave_num = 0;
        }
        wave_col_l[wave_num] = wave_col_r[wave_num] = col;
    }
}

// effects
void backlight_effect_breathing(uint8_t interval)
{
    static uint8_t pos = 0;
    static int8_t increament = 1;
    backlight_set_raw(pgm_read_byte(&RGBLED_BREATHING_TABLE[pos]));
    pos = pos + interval*increament;
    if (pos < interval) increament = 1;
    if (pos+interval > 126) increament = -1;
}


void backlight_effect_click_fading(void) {
    uint8_t i;
    bool key_fading = 0;
    for (i = 0; i < SLED_NUM; i++) {
        if (fading_single_key[i] > 1) {
            g_pwm_buffer[ pgm_read_byte(&g_pwm_control_register[i])-0x24 ] = pgm_read_byte(&RGBLED_BREATHING_TABLE[fading_single_key[i]]);
            fading_single_key[i] -= 2;
            key_fading = 1;
            g_pwm_buffer_update_required = true;
        }
    }
}

void backlight_effect_click_wave(void) {
    uint8_t i;
    static uint8_t step = 0;
    if (++step > 2) {
        step = 0;
        backlight_clear();
        for (uint8_t j=0; j<4; j++) {
            if (wave_col_l[j] >= -2) {
                if (wave_col_l[j] >= 0) {
                    for (i=0; i<5; i++) {
                        g_pwm_buffer[ pgm_read_byte(&g_pwm_control_register[ SLED_MATRIX_MASK[i][wave_col_l[j]] ])-0x24 ] = 255;
                    }
                }
                wave_col_l[j]--;
            } 
            if (wave_col_r[j] < (R_MATRIX_COLS + 2)) {
                if (wave_col_r[j] < R_MATRIX_COLS) {
                    for (i=0; i<5; i++) {
                        g_pwm_buffer[ pgm_read_byte(&g_pwm_control_register[ SLED_MATRIX_MASK[i][wave_col_r[j]] ])-0x24 ] =  255;
                    }
                }
                wave_col_r[j]++;
            } 
        }
        if (wave_col_l[wave_num] < -2 && wave_col_r[wave_num] >= (R_MATRIX_COLS + 2)) {
            wave_col_l[wave_num] = -3;
        } else {
            g_pwm_buffer_update_required = true;
        }
    }   
}

void backlight_effect_layer(void) {
    if (backlight_layer_update) {
        backlight_clear();
        backlight_layer_update = false;
        uint8_t top_layer = 0;
        for (uint8_t i=7; i>0; i--) {
            if (layer_state>>i > 0) {
                top_layer = i;
                break;
            }
        }
        if (top_layer > 0) {
            keypos_t key_check;
            for (uint8_t i = 0; i < R_MATRIX_ROWS; i++) {
                key_check.col = i;
                for (uint8_t j = 0; j < R_MATRIX_COLS; j++) {
                    key_check.row = j;
                    uint16_t key_check_code = action_for_key(top_layer, key_check).code;
                    if (key_check_code >= 2) {
                        g_pwm_buffer[ pgm_read_byte(&g_pwm_control_register[ SLED_MATRIX_MASK[i][j] ])-0x24 ] = 200; 
                    } 
                }
            }
        } 
        g_pwm_buffer_update_required = true;
    }
}

void backlight_effect_stars(uint8_t interval)
{
    static uint8_t timer_step = 0;
    if (++timer_step > interval ) { //1 fast, 2 normal
        uint8_t random_sled_row, random_sled_col;
        random_sled_row = rand() % R_MATRIX_ROWS;
        random_sled_col = rand() % R_MATRIX_COLS;
        uint8_t pressed_sled = pgm_read_byte(&g_pwm_control_register[ SLED_MATRIX_MASK[random_sled_row][random_sled_col] ]) - 0x24;
        if (fading_single_key[pressed_sled] <= 1) {
            fading_single_key[pressed_sled] = 126;
            timer_step = 0;
        } 
    }
    backlight_effect_click_fading();
}

void backlight_effect_raindrop(uint8_t interval)
{
    static uint8_t step = 0;
    uint8_t i,j;
    if (++step > interval) {  
        backlight_clear();
        uint8_t random_sled_col = rand() % R_MATRIX_COLS;
        if (fading_single_key[random_sled_col] == 0) {
            fading_single_key[random_sled_col] = R_MATRIX_ROWS + 2 - 1;
            step = 0;
        }
        for (j=0; j<R_MATRIX_COLS; j++) {
            if (fading_single_key[j]) {
                for (i=R_MATRIX_ROWS; i<R_MATRIX_ROWS+2; i++) {
                    uint8_t tmp_row = i-fading_single_key[j];
                    if (tmp_row >= 0 && tmp_row < 5) {
                        g_pwm_buffer[ pgm_read_byte(&g_pwm_control_register[ SLED_MATRIX_MASK[tmp_row][j] ])-0x24 ] = 255;
                    }
                }
                fading_single_key[j]--;
            } 
        }
        g_pwm_buffer_update_required = 1;
    }
} 

void backlight_effect_horse_race(void)
{
    static uint8_t step = 0;
    static uint8_t start_wait = 0;
    uint8_t i,j;
    if (++step > 1) { 
        backlight_clear();
        if (horse_race_remain == 0) {
            horse_race_remain = 5;
            start_wait = 20;
            for (i=0; i<5; i++) {
                fading_single_key[i] = R_MATRIX_COLS + BACKLIGHT_EFFECT_HORSE_LENGTH - 1;
            }
        }
        if (start_wait) { 
            start_wait--;
            return;
        }
        for (j=0; j<R_MATRIX_ROWS; j++) {
            if (fading_single_key[j] >= R_MATRIX_COLS + BACKLIGHT_EFFECT_HORSE_LENGTH) {
                horse_race_remain = 0;
                return;
            } 
            if (fading_single_key[j]) {
                for (i=R_MATRIX_COLS; i<R_MATRIX_COLS+BACKLIGHT_EFFECT_HORSE_LENGTH; i++) {
                    uint8_t tmp_col = i-fading_single_key[j];
                    if (tmp_col >= 0 && tmp_col < 15) {
                        g_pwm_buffer[ pgm_read_byte(&g_pwm_control_register[ SLED_MATRIX_MASK[j][tmp_col] ])-0x24 ] = 255;
                    } 
                }
                uint8_t random_delay1, random_delay2;
                random_delay1 = rand() % R_MATRIX_ROWS;
                random_delay2 = rand() % R_MATRIX_ROWS;
                if (j != random_delay1 && j != random_delay2) {
                    if (--fading_single_key[j] == 0) { 
                        if (horse_race_remain)  horse_race_remain--;
                    }
                }
            } 
        }
        g_pwm_buffer_update_required = 1;
    }
}

void backlight_loop_task(void)
{
    static uint8_t backlight_step = 0;
    static uint8_t IS31_LED_control_register_step = 0;
    if (backlight_enabled && ++backlight_step > 60) {
        backlight_step = 0;
        backlight_task();
        IS31FL3731_update_pwm_buffers(0x74);
        if (++IS31_LED_control_register_step > 80)
        {
            IS31_LED_control_register_step = 0;
            IS31FL3731_turn_on_LED_control_register(0x74);
        }
    }
    if (!backlight_enabled && backlight_update_once) {
        backlight_update_once = 0;
        g_pwm_buffer_update_required = true;
        IS31FL3731_update_pwm_buffers(0x74);
    }
}

#endif
