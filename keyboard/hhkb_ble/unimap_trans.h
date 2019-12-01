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


/* Mapping to Universal keyboard layout
 *
 * Universal keyboard layout
 *         ,-----------------------------------------------.
 *         |F13|F14|F15|F16|F17|F18|F19|F20|F21|F22|F23|F24|
 * ,---.   |-----------------------------------------------|     ,-----------.     ,-----------.
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

/* -----------------------------------------------------------------
 * HHKB
 * ,-----------------------------------------------------------.
 * |Esc|  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|  \|  `|
 * |-----------------------------------------------------------|
 * |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|  BS |
 * |-----------------------------------------------------------|
 * |CapsL |  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '|Enter   |
 * |-----------------------------------------------------------|
 * |Shift   |  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /|Shift |Del|
 * `-----------------------------------------------------------'
 *       |Alt |Win  |         Space        |RWin |RAlt|
 *       `--------------------------------------------'
 *
 * Matrix - row:8 x col:8
 * ,-----------------------------------------------------------.
 * |31 |30 |00 |10 |11 |20 |21 |40 |41 |60 |61 |70 |71 |50 |51 |
 * |-----------------------------------------------------------|
 * |32   |01 |02 |13 |12 |23 |22 |42 |43 |62 |63 |73 |72 |52   |
 * |-----------------------------------------------------------|
 * |33    |04 |03 |14 |15 |24 |25 |45 |44 |65 |64 |74 |     53 |
 * |-----------------------------------------------------------|
 * |34      |05 |06 |07 |16 |17 |26 |46 |66 |76 |75 |55    |54 |
 * `-----------------------------------------------------------'
 *       |35 |36   |          37           |57   |56 |
 *       `-------------------------------------------'
 */

const uint8_t PROGMEM unimap_trans[8][8] = {
    { UM_2,   UM_Q,   UM_W,   UM_S,   UM_A,   UM_Z,   UM_X,   UM_C,   },
    { UM_3,   UM_4,   UM_R,   UM_E,   UM_D,   UM_F,   UM_V,   UM_B,   },
    { UM_5,   UM_6,   UM_Y,   UM_T,   UM_G,   UM_H,   UM_N,   UM_NO,  },
    { UM_1,   UM_ESC, UM_TAB, UM_LCTL,UM_LSFT,UM_LALT,UM_LGUI,UM_SPC, },
    { UM_7,   UM_8,   UM_U,   UM_I,   UM_K,   UM_J,   UM_M,   UM_NO,  },
    { UM_BSLS,UM_GRV, UM_BSPC,UM_ENT, UM_DEL, UM_RSFT,UM_RALT,UM_RGUI,},
    { UM_9,   UM_0,   UM_O,   UM_P,   UM_SCLN,UM_L,   UM_COMM,UM_NO,  },
    { UM_MINS,UM_EQL, UM_RBRC,UM_LBRC,UM_QUOT,UM_SLSH,UM_DOT, UM_NO,  }
};

/*  matrix of US
["@\n2","Q","W","S","A","Z","X","C"],
["#\n3","$\n4","R","E","D","F","V","B"],
["%\n5","^\n6","Y","T","G","H","N",{a:7},""],
[{a:4},"!\n1","Esc","Tab","Ctrl","Shift","Alt","Win","Space"],
["&\n7","*\n8","U","I","K","J","M",{a:7},""],
[{a:4},"|\n\\","~\n`","Backspace","Enter","Del","RShift","RAlt","RWin"],
["(\n9",")\n0","O","P",":\n;","L","<\n,",{a:7},""],
[{a:4},"_\n-","+\n=","}\n]","{\n[","\"\n'","?\n/",">\n.",{a:7},""]
*/

/* ---------------------------------------------------------------------
 * HHKB JP
 * ,-----------------------------------------------------------.
 * |`  |1  |2  |3  |4  |5  |6  |7  |8  |9  |0  |-  |=  |JPY|BS |
 * |-----------------------------------------------------------|
 * |Tab  |Q  |W  |E  |R  |T  |Y  |U  |I  |O  |P  |[  |]  |     |
 * |------------------------------------------------------`    |
 * |CapsL |A  |S  |D  |F  |G  |H  |J  |K  |L  |;  |'  |#  |Entr|
 * |-----------------------------------------------------------|
 * |Shift  |Z  |X  |C  |V  |B  |N  |M  |,  |.  |/  |RO |Up |Shi|
 * |-----------------------------------------------------------|
 * |Ctl||Esc|Gui|Alt|MHE|         |HEN|KAN|Alt|Ctl||Lef|Dow|Rig|
 * `-----------------------------------------------------------'
 *
 * Matrix -  row:16 x col:8
 * ,-----------------------------------------------------------.
 * |02 |32 |62 |22 |12 |52 |72 |A2 |92 |82 |B2 |E2 |F2 |D2 |C2 |
 * |-----------------------------------------------------------|
 * |03   |63 |23 |13 |53 |73 |A3 |93 |83 |B3 |E3 |F3 |D3 |     |
 * |------------------------------------------------------`    |
 * |06    |66 |26 |16 |56 |76 |A6 |96 |86 |B6 |E6 |F6 |D6 | C6 |
 * |-----------------------------------------------------------|
 * |05     |65 |25 |15 |55 |75 |A5 |95 |85 |B5 |E5 |F5 |D5 |C5 |
 * |-----------------------------------------------------------|
 * |04 ||34 |64 |24 |14 |     74  |94 |84 |B4 |E4 ||F4 |D4 |C4 |
 * `-----------------------------------------------------------'
 */
const uint8_t PROGMEM unimap_trans_jp[16][8] = {
    { UM_NO,  UM_NO,  UM_ESC, UM_TAB, UM_HOME,UM_LSFT,UM_CAPS,UM_NO,  },
    { UM_NO,  UM_NO,  UM_4,   UM_E,   UM_APP, UM_C,   UM_D,   UM_NO,  },
    { UM_NO,  UM_NO,  UM_3,   UM_W,   UM_LALT,UM_X,   UM_S,   UM_NO,  },
    { UM_NO,  UM_NO,  UM_1,   UM_NO,  UM_LCTL,UM_NO,  UM_NO,  UM_NO,  },
    { UM_NO,  UM_NO,  UM_NO,  UM_NO,  UM_NO,  UM_NO,  UM_NO,  UM_NO,  },
    { UM_NO,  UM_NO,  UM_5,   UM_R,   UM_NO,  UM_V,   UM_F,   UM_NO,  },
    { UM_NO,  UM_NO,  UM_2,   UM_Q,   UM_LGUI,UM_Z,   UM_A,   UM_NO,  },
    { UM_NO,  UM_NO,  UM_6,   UM_T,   UM_SPC, UM_B,   UM_G,   UM_NO,  },
    { UM_NO,  UM_NO,  UM_9,   UM_I,   UM_RALT,UM_COMM,UM_K,   UM_NO,  },
    { UM_NO,  UM_NO,  UM_8,   UM_U,   UM_END, UM_M,   UM_J,   UM_NO,  },
    { UM_NO,  UM_NO,  UM_7,   UM_Y,   UM_NO,  UM_N,   UM_H,   UM_NO,  },
    { UM_NO,  UM_NO,  UM_0,   UM_O,   UM_RGUI,UM_DOT, UM_L,   UM_NO,  },
    { UM_NO,  UM_NO,  UM_BSPC,UM_NO,  UM_RGHT,UM_RSFT,UM_ENT, UM_NO,  },
    { UM_NO,  UM_NO,  UM_GRV, UM_RBRC,UM_DOWN,UM_UP,  UM_BSLS,UM_NO,  },
    { UM_NO,  UM_NO,  UM_MINS,UM_P,   UM_RCTL,UM_SLSH,UM_SCLN,UM_NO,  },
    { UM_NO,  UM_NO,  UM_EQL, UM_LBRC,UM_LEFT,UM_DEL, UM_QUOT,UM_NO,  },
};

/*
[{a:7},"","",{a:4},"Esc","Tab","Home","Shift","Caps Lock",{a:7},""],
["","",{a:4},"$\n4","E","App","C","D",{a:7},""],
["","",{a:4},"#\n3","W","Alt","X","S",{a:7},""],
["","",{a:4},"!\n1",{a:7},"",{a:4},"Ctrl",{a:7},"","",""],
["","","","","","","",""],
["","",{a:4},"%\n5","R",{a:7},"",{a:4},"V","F",{a:7},""],
["","",{a:4},"@\n2","Q","Win","Z","A",{a:7},""],
["","",{a:4},"^\n6","T","Space","B","G",{a:7},""],
["","",{a:4},"(\n9","I","RAlt","<\n,","K",{a:7},""],
["","",{a:4},"*\n8","U","End","M","J",{a:7},""],
["","",{a:4},"&\n7","Y",{a:7},"",{a:4},"N","H",{a:7},""],
["","",{a:4},")\n0","O","RWin",">\n.","L",{a:7},""],
["","",{a:4},"Backspace",{a:7},"",{a:4},"→","RShift","Enter",{a:7},""],
["","",{a:4},"~\n`","}\n]","↓","↑","|\n\\",{a:7},""],
["","",{a:4},"_\n-","P","RCtrl","?\n/",":\n;",{a:7},""],
["","",{a:4},"+\n=","{\n[","←","Del","\"\n'",{a:7},""]
*/
#endif
