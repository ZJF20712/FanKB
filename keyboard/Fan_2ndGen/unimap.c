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
#include <avr/pgmspace.h>
#include "unimap_trans.h"
#include "bootloader.h"
#include "wait.h"
#include "keycode.h"
#include "action.h"
#include "action_util.h"
#include "keymap.h"
#include "rgblight.h"


enum function_id {
    MAGIC_RSHIFT,
    TRICKY_ESC_LSHIFT,
    JMP_BL,
    RGBLED_ACTION = 0x0A,
    HOST_SWITCH,
    TRICKY_ALT_F4,
};
#ifdef KeyB_40Key
enum function_opt {
    ESC_GRV,
    ENT_QUOT,
    DOT_SLSH,
    UP_SLSH,
    L_SCLN,
    BSPC_BSLS,
};
#endif
enum macro_id {
    HOME_PGUP,
    END_PGDN,
};
enum RGB_opt{
	Toggle = 0,
	Mode_M = 1,
	Mode_P = 2,
	Hue_M  = 3,
	Hue_P  = 4,
	Sat_M  = 5,
	Sat_P  = 6,
	Val_M  = 7,
	Val_P  = 8,
};
//Layer ctl
#define AC_LM1 ACTION_LAYER_MOMENTARY(1)
#define AC_LM2 ACTION_LAYER_MOMENTARY(2)
#define AC_LM3 ACTION_LAYER_MOMENTARY(3)
#define AC_LM4 ACTION_LAYER_MOMENTARY(4)
#define AC_LM5 ACTION_LAYER_MOMENTARY(5)
#define AC_LM6 ACTION_LAYER_MOMENTARY(6)
#define AC_LM7 ACTION_LAYER_MOMENTARY(7)
#define AC_LM8 ACTION_LAYER_MOMENTARY(8)
//ACTION_FUNCTION_OPT(id, opt)
//ACTION_FUNCTION(id)
#define AC_BL  ACTION_FUNCTION(JMP_BL)
#define AC_HB  ACTION_FUNCTION(HOST_SWITCH)
#define AC_LESC  ACTION_FUNCTION(TRICKY_ESC_LSHIFT)
#define AC_4_F4  ACTION_FUNCTION(TRICKY_ALT_F4)
#define AC_RGB_TG   ACTION_FUNCTION_OPT(RGBLED_ACTION, Toggle)
#define AC_RGB_MM   ACTION_FUNCTION_OPT(RGBLED_ACTION, Mode_M)
#define AC_RGB_MP   ACTION_FUNCTION_OPT(RGBLED_ACTION, Mode_P)
#define AC_RGB_HM   ACTION_FUNCTION_OPT(RGBLED_ACTION, Hue_M)
#define AC_RGB_HP   ACTION_FUNCTION_OPT(RGBLED_ACTION, Hue_P)
#define AC_RGB_SM   ACTION_FUNCTION_OPT(RGBLED_ACTION, Sat_M)
#define AC_RGB_SP   ACTION_FUNCTION_OPT(RGBLED_ACTION, Sat_P)
#define AC_RGB_VM   ACTION_FUNCTION_OPT(RGBLED_ACTION, Val_M)
#define AC_RGB_VP   ACTION_FUNCTION_OPT(RGBLED_ACTION, Val_P)
/*	0 toggle
1 mode-	2 mode+
3 hue- 	4 hue+
5 sat- 	6 sat+
7 val- 	8 val+
*/
#ifdef KEYMAP_SECTION_ENABLE
const action_t actionmaps[][UNIMAP_ROWS][UNIMAP_COLS] __attribute__ ((section (".keymap.keymaps"))) = {
#else
const action_t actionmaps[][UNIMAP_ROWS][UNIMAP_COLS] PROGMEM = {
#endif
	//Layer 0
    UNIMAP(
        BTN1,BTN2,BTN3,BTN4,BTN5,WH_U,WH_D, F20, F21, F22, F23, F24,
        LESC,      F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12,           PSCR,SLCK,PAUS,         VOLD,VOLU,MUTE,
        GRV, 1,   2,   3,   4_F4,   5,   6,   7,   8,   9,   0,   MINS,EQL, JYEN,BSPC,     INS, HOME,PGUP,    NLCK,PSLS,PAST,PMNS,
        TAB, Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,   LBRC,RBRC,     BSLS,     DEL, END, PGDN,    P7,  P8,  P9,  PPLS,
        CAPS,A,   S,   D,   F,   G,   H,   J,   K,   L,   SCLN,QUOT,     NUHS,ENT,                         P4,  P5,  P6,  PCMM,
        LSFT,NUBS,Z,   X,   C,   V,   B,   N,   M,   COMM,DOT, SLSH,     RO,  RSFT,          UP,           P1,  P2,  P3,  PENT,
        LCTL,LGUI,LALT,MHEN,          SPC,           HENK,KANA,RALT,LM1, LM2, RCTL,     LEFT,DOWN,RGHT,    P0,       PDOT,PEQL
        ),
     //Layer 1
    UNIMAP(
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        GRV,      TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,          TRNS,TRNS,TRNS,         TRNS,TRNS,TRNS,
        ESC, F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12, INS, DEL,      TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,UP,  TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,INS,  PGUP,   PGDN,     TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS,TRNS,
        TRNS,LEFT,DOWN,RGHT,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     TRNS,TRNS,                        TRNS,TRNS,TRNS,TRNS,
        TRNS,RGB_TG,RGB_MM,RGB_MP,RGB_HM,RGB_HP,RGB_SM,RGB_SP,RGB_VM,RGB_VP,TRNS,TRNS,TRNS,TRNS,TRNS,      TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,          TRNS,          TRNS,TRNS,TRNS,TRNS,END,TRNS,     MUTE,VOLD,VOLU,     TRNS,     TRNS,TRNS
        ),
     //Layer2
    UNIMAP(
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        GRV,      TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,          TRNS,TRNS,TRNS,         TRNS,TRNS,TRNS,
        ESC, F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12, INS, DEL,      TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,  TRNS,   TRNS,     TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,    TRNS,TRNS,                         TRNS,TRNS,TRNS,TRNS,
        BL  ,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     TRNS,TRNS,          TRNS,         TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,          TRNS,          TRNS,TRNS,TRNS,HOME,TRNS,TRNS,     MUTE,VOLD,VOLU,    TRNS,     TRNS,TRNS
        ),
};

void action_function(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    static uint8_t mod_keys_registered;
    static uint8_t key_mod;
    static uint8_t key_orig;
    switch (id) {
        case TRICKY_ESC_LSHIFT:
            if (record->event.pressed) {
                if ((get_mods() & MOD_BIT(KC_LSHIFT))&& (!(get_mods() & MOD_BIT(KC_LCTRL)))) {
                    mod_keys_registered = KC_GRV;
                }
                else {
                    mod_keys_registered = KC_ESC;
                }
                register_code(mod_keys_registered);
                send_keyboard_report();
            }
            else {
                unregister_code(mod_keys_registered);
                send_keyboard_report();
            }
            break;
		case TRICKY_ALT_F4:
			if (record->event.pressed) {
                if ((get_mods() & MOD_BIT(KC_LALT))) {
                    mod_keys_registered = KC_F4;
                }
                else {
                    mod_keys_registered = KC_4;
                }
                register_code(mod_keys_registered);
                send_keyboard_report();
            }
            else {
                unregister_code(mod_keys_registered);
                send_keyboard_report();
            }
            break;	
#ifdef KeyB_40Key
//For only for keyboard with less than 60 keys
        case MAGIC_RSHIFT:
            switch (opt){
                case ESC_GRV:
                    if (get_mods() & MOD_BIT(KC_LALT)){
                        key_orig = KC_F4;
                    }else{
                        key_orig = KC_ESC;
                    }
                    key_mod = KC_GRV;
                    break;
                case ENT_QUOT:
                    key_mod = KC_QUOT;
                    key_orig = KC_ENT;
                    break;
                case DOT_SLSH:
                    key_mod = KC_SLSH;
                    key_orig = KC_DOT;
                    break;
                case UP_SLSH:
                    key_mod = KC_SLSH;
                    key_orig = KC_UP;
                    break;
                case L_SCLN:
                    key_mod = KC_SCLN;
                    key_orig = KC_L;
                    break;
                case BSPC_BSLS:
                    key_mod = KC_BSLS;
                    key_orig = KC_BSPC;
                    break;
            }
            if (record->event.pressed) {
                if (get_mods() & MOD_BIT(KC_RSHIFT)) {
                    mod_keys_registered = key_mod;
                } else {
                    mod_keys_registered = key_orig;
                }
                register_code(mod_keys_registered);
                send_keyboard_report();
            } else {
                unregister_code(mod_keys_registered);
                send_keyboard_report();
            }
            break;
#endif
    }
    if (record->event.pressed) {
        switch (id) {
            case JMP_BL:
                if (get_mods() & MOD_BIT(KC_LCTRL)) {
                    command_extra(KC_B);
                }
                register_mods(0b11111111);
                unregister_mods(0b11111111);
                clear_keyboard(); 
                break;
            case RGBLED_ACTION:
                rgblight_action(opt);
				    /*  0 toggle
 					   1 mode-    2 mode+
 					   3 hue-     4 hue+
 					   5 sat-     6 sat+
 					   7 val-     8 val+
 				   */
                break;
            case HOST_SWITCH:
                command_extra(KC_U);
                break;
        }
    }
}

const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    static uint16_t key_timer;
    switch(id) {
        case HOME_PGUP: 
            if (record->event.pressed) {
                key_timer = timer_read(); 
            }
            else {
                if (timer_elapsed(key_timer) > 200) { 
                    return MACRO( T(HOME), END );
                }
                else {
                    return MACRO( T(PGUP), END );
                }
            }
            break;
        case END_PGDN: 
            if (record->event.pressed) {
                key_timer = timer_read(); 
            }
            else { 
                if (timer_elapsed(key_timer) > 200) {
                    return MACRO( T(END), END );
                }
                else {
                    return MACRO( T(PGDN), END );
                }
            }
            break;
    }
    return MACRO_NONE;
};
