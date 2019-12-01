#include <avr/pgmspace.h>
#include "unimap_trans.h"
#include "bootloader.h"
#include "wait.h"
#include "keycode.h"
#include "action.h"
#include "action_util.h"
#include "keymap.h"

extern bool is_980c;;

enum function_id {
    TRICKY_ESC,
    LALT_F4,
    RESET,
    RGBLED_ACTION = 0x0A,
    HOST_SWITCH,
    SLEEP_EN,
};


enum macro_id {
    HOME_PGUP,
    END_PGDN,
};
#define AC_FN0 ACTION_LAYER_MOMENTARY(1)
#define AC_FN1 ACTION_FUNCTION(RESET)
#ifdef KEYMAP_SECTION_ENABLE
const action_t actionmaps[][UNIMAP_ROWS][UNIMAP_COLS] __attribute__ ((section (".keymap.keymaps"))) = {
#else
const action_t actionmaps[][UNIMAP_ROWS][UNIMAP_COLS] PROGMEM = {
#endif
    UNIMAP(
        BTN1,BTN2,BTN3,BTN4,BTN5,WH_U,WH_D, F20, F21, F22, F23, F24,
        ESC,      F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12,           PSCR,SLCK,PAUS,         VOLD,VOLU,MUTE,
        GRV, 1,   2,   3,   4,   5,   6,   7,   8,   9,   0,   MINS,EQL, JYEN,BSPC,     INS, HOME,PGUP,    NLCK,PSLS,PAST,PMNS,
        TAB, Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,   LBRC,RBRC,     BSLS,     DEL, END, PGDN,    P7,  P8,  P9,  PPLS,
        CAPS,A,   S,   D,   F,   G,   H,   J,   K,   L,   SCLN,QUOT,     NUHS,ENT,                         P4,  P5,  P6,  PCMM,
        LSFT,NUBS,Z,   X,   C,   V,   B,   N,   M,   COMM,DOT, SLSH,     RO,  RSFT,          UP,           P1,  P2,  P3,  PENT,
        LCTL,LGUI,LALT,MHEN,          SPC,           HENK,KANA,RALT,RGUI,APP, RCTL,     LEFT,DOWN,RGHT,    P0,       PDOT,PEQL
        ),
    UNIMAP(
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
        GRV,      TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,          TRNS,TRNS,TRNS,         TRNS,TRNS,TRNS,
        ESC, F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12, INS, DEL,      TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,  TRNS,   TRNS,     TRNS,TRNS,TRNS,    TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,CALC,TRNS,TRNS,HOME,PGUP,LEFT,RGHT,     TRNS,TRNS,                        TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,FN1,TRNS,TRNS,END, PGDN,DOWN,     TRNS,TRNS,          TRNS,         TRNS,TRNS,TRNS,TRNS,
        TRNS,TRNS,TRNS,TRNS,          TRNS,          TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     MUTE,VOLD,VOLU,    TRNS,     TRNS,TRNS
        ),
};

keypos_t unimap_translate(keypos_t key)
{
    uint8_t unimap_pos; 
    if (is_980c) unimap_pos = pgm_read_byte(&unimap_trans_980c[key.row][key.col]);
    else unimap_pos = pgm_read_byte(&unimap_trans_660c[key.row][key.col]);
    return (keypos_t) {
        .row = ((unimap_pos & 0xf0) >> 4),
        .col = (unimap_pos & 0x0f)
    };
}

#define MODS_SHIFT_MASK (MOD_BIT(KC_LSHIFT)|MOD_BIT(KC_RSHIFT))
void action_function(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    static uint8_t mod_keys_registered;
    switch (id) {
        case TRICKY_ESC:
            if (record->event.pressed) {
                if ((get_mods() & MODS_SHIFT_MASK) && (!(get_mods() & MOD_BIT(KC_LCTRL)))) {
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
        case LALT_F4:
            if (record->event.pressed) {
                if (get_mods() & MOD_BIT(KC_LALT)) { 
                    mod_keys_registered = KC_F4;
                }
                else {
                    mod_keys_registered = KC_4;
                }
                register_code(mod_keys_registered);
                send_keyboard_report();
            } else {
                unregister_code(mod_keys_registered);
                send_keyboard_report();
            }
            break;
    }
    if (record->event.pressed) {
        switch (id) {
            case RESET:
                if (get_mods() & MOD_BIT(KC_LCTRL)) {
                    uint16_t *const bootKeyPtr = (uint16_t *)0x0800;
                    *bootKeyPtr = 0xFC2B;
                    command_extra(KC_B);
                }
                register_mods(0b11111111);
                unregister_mods(0b11111111);
                clear_keyboard(); 
                break;
            case HOST_SWITCH:
                command_extra(KC_U);
                break;
            case SLEEP_EN:
                command_extra(KC_P);
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
