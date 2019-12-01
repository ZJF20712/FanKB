/*
Copyright 2011,2012,2013 Jun Wako <wakojun@gmail.com>

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
#ifndef KEYMAP_COMMON_H
#define KEYMAP_COMMON_H

#include <stdint.h>
#include "action.h"

extern const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS];
extern const uint16_t fn_actions[];
/* All keys */
#define KEYMAP( \
    K0A, K0B, K0C, K0D, K0E, K0F, K0G, K0H, K0J, K0K, K0L, K0M, \
    K1A, K1B, K1C, K1D, K1E, K1F, K1G, K1H, K1J, K1K, K1L, K1M, \
    K2A, K2B, K2C, K2D, K2E, K2F, K2G, K2H, K2J, K2K, K2L, K2M, \
    K3A, K3B, K3C, K3D, K3E, K3F, K3G, K3H, K3J, K3K, K3L, K3M  \
) { \
    { KC_##K0A, KC_##K0B, KC_##K0C, KC_##K0D, KC_##K0E, KC_##K0F, KC_##K0G, KC_##K0H, KC_##K0J, KC_##K0K, KC_##K0L, KC_##K0M, },  \
    { KC_##K1A, KC_##K1B, KC_##K1C, KC_##K1D, KC_##K1E, KC_##K1F, KC_##K1G, KC_##K1H, KC_##K1J, KC_##K1K, KC_##K1L, KC_##K1M, },  \
    { KC_##K2A, KC_##K2B, KC_##K2C, KC_##K2D, KC_##K2E, KC_##K2F, KC_##K2G, KC_##K2H, KC_##K2J, KC_##K2K, KC_##K2L, KC_##K2M, },  \
    { KC_##K3A, KC_##K3B, KC_##K3C, KC_##K3D, KC_##K3E, KC_##K3F, KC_##K3G, KC_##K3H, KC_##K3J, KC_##K3K, KC_##K3L, KC_##K3M  }  \
}

#endif
