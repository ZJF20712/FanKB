/*
Copyright 2016 Jun Wako <wakojun@gmail.com>

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
#ifndef UNIMAP_COMMON_H
#define UNIMAP_COMMON_H

#include <stdint.h>

#include <avr/pgmspace.h>
#include "unimap.h"

const uint8_t PROGMEM unimap_trans_660c[5][MATRIX_COLS] = {
    { UM_Q   ,UM_W   ,UM_E   ,UM_TAB ,UM_R   ,UM_U   ,UM_T   ,UM_Y   ,UM_O   ,UM_P   ,UM_LBRC,UM_I   ,UM_RBRC,UM_NO  ,UM_BSLS,UM_DEL   },
    { UM_1   ,UM_2   ,UM_3   ,UM_ESC ,UM_4   ,UM_7,   UM_5   ,UM_6   ,UM_9   ,UM_0   ,UM_MINS,UM_8   ,UM_EQL ,UM_NO  ,UM_BSPC,UM_INS   },
    { UM_NO  ,UM_LGUI,UM_LALT,UM_LCTL,UM_NO  ,UM_NO,  UM_NO  ,UM_SPC ,UM_RALT,UM_NO  ,UM_RCTL,UM_NO  ,UM_RGUI,UM_DOWN,UM_LEFT,UM_RGHT  },
    { UM_NO  ,UM_Z   ,UM_X   ,UM_LSFT,UM_C   ,UM_N,   UM_V   ,UM_B   ,UM_COMM,UM_DOT ,UM_SLSH,UM_M   ,UM_RSFT,UM_UP  ,UM_NO  ,UM_NO    },
    { UM_A   ,UM_S   ,UM_D   ,UM_CAPS,UM_F   ,UM_J,   UM_G   ,UM_H   ,UM_L   ,UM_SCLN,UM_QUOT,UM_K   ,UM_NO  ,UM_NO  ,UM_ENT ,UM_NO    }
};

const uint8_t PROGMEM unimap_trans_980c[MATRIX_ROWS][MATRIX_COLS] = {
    { UM_LEFT,UM_RCTL,UM_RALT,UM_NO  ,UM_DOWN,UM_PDOT,UM_RGHT,UM_P0  ,UM_X   ,UM_LGUI,UM_GRV ,UM_V   ,UM_NO  ,UM_ESC ,UM_M   ,UM_SPC   },
    { UM_RGUI,UM_DOT ,UM_NO  ,UM_NO  ,UM_P1  ,UM_PENT,UM_P2  ,UM_P3  ,UM_Z   ,UM_LALT,UM_LCTL,UM_C   ,UM_K   ,UM_NO  ,UM_N   ,UM_B     },
    { UM_QUOT,UM_SLSH,UM_COMM,UM_NO  ,UM_P4  ,UM_PPLS,UM_P5  ,UM_P6  ,UM_D   ,UM_A   ,UM_LSFT,UM_F   ,UM_J   ,UM_F1  ,UM_H   ,UM_G     },
    { UM_RSFT,UM_SCLN,UM_L   ,UM_RBRC,UM_UP  ,UM_NO  ,UM_NO  ,UM_NO  ,UM_S   ,UM_Q   ,UM_CAPS,UM_R   ,UM_I   ,UM_F3  ,UM_U   ,UM_T     },
    { UM_NO  ,UM_NO  ,UM_NO  ,UM_NO  ,UM_NO  ,UM_NO  ,UM_NO  ,UM_NO  ,UM_NO  ,UM_NO  ,UM_NO  ,UM_NO  ,UM_NO  ,UM_NO  ,UM_NO  ,UM_NO    },
    { UM_EQL ,UM_MINS,UM_0   ,UM_BSLS,UM_NLCK,UM_BSPC,UM_PSLS,UM_PAST,UM_3   ,UM_2   ,UM_NO  ,UM_4   ,UM_9   ,UM_F2  ,UM_7   ,UM_6     },
    { UM_LBRC,UM_P   ,UM_O   ,UM_ENT ,UM_P7  ,UM_PMNS,UM_P8  ,UM_P9  ,UM_W   ,UM_1   ,UM_TAB ,UM_E   ,UM_8   ,UM_F4  ,UM_Y   ,UM_5     },
    { UM_F11 ,UM_F10 ,UM_F9  ,UM_F12 ,UM_DEL ,UM_PGDN,UM_INS ,UM_PGUP,UM_NO  ,UM_NO  ,UM_NO  ,UM_NO  ,UM_F8  ,UM_F5  ,UM_F7  ,UM_F6    }
};
#endif
