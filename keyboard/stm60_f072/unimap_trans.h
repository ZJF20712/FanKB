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
#include "unimap.h"


/* Mapping to Universal keyboard layout
 *
 * Universal keyboard layout
 *         ,-----------------------------------------------.
 * |Esc|   |F1 |F2 |F3 |F4 |F5 |F6 |F7 |F8 |F9 |F10|F11|F12|     |PrS|ScL|Pau|     |VDn|VUp|Mut|
 * `---'   `-----------------------------------------------'     `-----------'     `-----------'
 * ,-----------------------------------------------------------. ,-----------. ,---------------.
 * |  `|  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|JPY|Bsp| |Ins|Hom|PgU| |NmL|  /|  *|  -|
 * |-----------------------------------------------------------| |-----------| |---------------|
 * |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|  \  | |Del|End|PgD| |  7|  8|  9|  +|
 * |-----------------------------------------------------------| `-----------' |---------------|
 * |CapsL |  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '|  #|Retn|               |  4|  5|  6|KP,|
 * |-----------------------------------------------------------|     ,---.     |---------------|
 * |Shft|  <|  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /| RO|Shift |     |Up |     |  1|  2|  3|KP=|
 * |-----------------------------------------------------------| ,-----------. |---------------|
 * |Ctl|Gui|Alt|MHEN|     Space      |HENK|KANA|Alt|Gui|App|Ctl| |Lef|Dow|Rig| |  0    |  .|Ent|
 * `-----------------------------------------------------------' `-----------' `---------------'
 */
const uint8_t PROGMEM unimap_trans[MATRIX_ROWS][MATRIX_COLS] = {
    { UNIMAP_ESC, UNIMAP_1,   UNIMAP_2,   UNIMAP_3,    UNIMAP_4,   UNIMAP_5,   UNIMAP_6,   UNIMAP_7,   UNIMAP_8,   UNIMAP_9,   UNIMAP_0,   UNIMAP_MINS,UNIMAP_EQL, UNIMAP_GRV, UNIMAP_PSCR},
    { UNIMAP_TAB, UNIMAP_Q,   UNIMAP_W,   UNIMAP_E,    UNIMAP_R,   UNIMAP_T,   UNIMAP_Y,   UNIMAP_U,   UNIMAP_I,   UNIMAP_O,   UNIMAP_P,   UNIMAP_LBRC,UNIMAP_RBRC,UNIMAP_BSLS,UNIMAP_HOME},
    { UNIMAP_CAPS,UNIMAP_A,   UNIMAP_S,   UNIMAP_D,    UNIMAP_F,   UNIMAP_G,   UNIMAP_H,   UNIMAP_J,   UNIMAP_K,   UNIMAP_L,   UNIMAP_SCLN,UNIMAP_QUOT,UNIMAP_NUHS,UNIMAP_ENT, UNIMAP_END },
    { UNIMAP_LSFT,UNIMAP_Z,   UNIMAP_X,   UNIMAP_C,    UNIMAP_V,   UNIMAP_B,   UNIMAP_N,   UNIMAP_M,   UNIMAP_COMM,UNIMAP_DOT, UNIMAP_SLSH,UNIMAP_RSFT,UNIMAP_UP,  UNIMAP_DEL, UNIMAP_PGUP},
    { UNIMAP_LCTL,UNIMAP_LGUI,UNIMAP_LALT,UNIMAP_NUBS, UNIMAP_APP, UNIMAP_SPC, UNIMAP_INS, UNIMAP_RGUI,UNIMAP_BSPC,UNIMAP_RALT,UNIMAP_RCTL,UNIMAP_LEFT,UNIMAP_DOWN,UNIMAP_RIGHT,UNIMAP_PGDN}
};

#endif
