/*
Copyright 2016 Jun Wako <wakojun@gmail.com>
*/
#ifndef _UNIMAP_H_
#define _UNIMAP_H_

#include <stdint.h>
#include <stdbool.h>
#include "action.h"
#include "action_code.h"
#include "actionmap.h"


// Universal map table: 8x16=128key
#define UNIMAP_ROWS 8
#define UNIMAP_COLS 16

/* Universal 128-key keyboard layout(8x16)
        ,-----------------------------------------------.
        |F13|F14|F15|F16|F17|F18|F19|F20|F21|F22|F23|F24|
,---.   |-----------------------------------------------|     ,-----------.     ,-----------.
|Esc|   |F1 |F2 |F3 |F4 |F5 |F6 |F7 |F8 |F9 |F10|F11|F12|     |PrS|ScL|Pau|     |VDn|VUp|Mut|
`---'   `-----------------------------------------------'     `-----------'     `-----------'
,-----------------------------------------------------------. ,-----------. ,---------------.
|  `|  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|JPY|Bsp| |Ins|Hom|PgU| |NmL|  /|  *|  -|
|-----------------------------------------------------------| |-----------| |---------------|
|Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|  \  | |Del|End|PgD| |  7|  8|  9|  +|
|-----------------------------------------------------------| `-----------' |---------------|
|CapsL |  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '|  #|Entr|               |  4|  5|  6|KP,|
|-----------------------------------------------------------|     ,---.     |---------------|
|Shft|  <|  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /| RO|Shift |     |Up |     |  1|  2|  3|Ent|
|-----------------------------------------------------------| ,-----------. |---------------|
|Ctl|Gui|Alt|MHEN|     Space      |HENK|KANA|Alt|Gui|App|Ctl| |Lef|Dow|Rig| |  0    |  .|KP=|
`-----------------------------------------------------------' `-----------' `---------------'
App:         Windows Menu key
Gui:         Windows key, Mac ⌘ key or Meta key
VDn Vup Mut: Volume control
< #:         ISO keys(in UK legend)
KP=:         Keypad = for Mac
KP,:         Brazilian Keypad Comma
JPY:         Japanese Yen(￥)
RO:          Japanese ろ(Ro) or Brazilian /(Slash)
MHEN:        Japanese 無変換(Non Conversion) or Korean Hanja
HENK:        Japanese 変換(Conversion) or Korean Hangul/English
KANA:        Japanese かな(Hiragana/Katakana)
https://en.wikipedia.org/wiki/Keyboard_layout#Japanese
https://en.wikipedia.org/wiki/Keyboard_layout#Hangul_.28for_Korean.29
*/
#define UNIMAP( \
            K68,K69,K6A,K6B,K6C,K6D,K6E,K6F,K70,K71,K72,K73,                                     \
    K29,    K3A,K3B,K3C,K3D,K3E,K3F,K40,K41,K42,K43,K44,K45,      K46,K47,K48,      K01,K02,K03, \
    K35,K1E,K1F,K20,K21,K22,K23,K24,K25,K26,K27,K2D,K2E,K74,K2A,  K49,K4A,K4B,  K53,K54,K55,K56, \
    K2B,K14,K1A,K08,K15,K17,K1C,K18,K0C,K12,K13,K2F,K30,    K31,  K4C,K4D,K4E,  K5F,K60,K61,K57, \
    K39,K04,K16,K07,K09,K0A,K0B,K0D,K0E,K0F,K33,K34,    K32,K28,                K5C,K5D,K5E,K66, \
    K79,K64,K1D,K1B,K06,K19,K05,K11,K10,K36,K37,K38,    K75,K7D,      K52,      K59,K5A,K5B,K58, \
    K78,K7B,K7A,K77,        K2C,        K76,K00,K7E,K7F,K65,K7C,  K50,K51,K4F,      K62,K63,K67  \
) { \
    { AC_##K00, AC_##K01, AC_##K02, AC_##K03, AC_##K04, AC_##K05, AC_##K06, AC_##K07,   /* 00-07 */ \
      AC_##K08, AC_##K09, AC_##K0A, AC_##K0B, AC_##K0C, AC_##K0D, AC_##K0E, AC_##K0F }, /* 08-0F */ \
    { AC_##K10, AC_##K11, AC_##K12, AC_##K13, AC_##K14, AC_##K15, AC_##K16, AC_##K17,   /* 10-17 */ \
      AC_##K18, AC_##K19, AC_##K1A, AC_##K1B, AC_##K1C, AC_##K1D, AC_##K1E, AC_##K1F }, /* 18-1F */ \
    { AC_##K20, AC_##K21, AC_##K22, AC_##K23, AC_##K24, AC_##K25, AC_##K26, AC_##K27,   /* 20-27 */ \
      AC_##K28, AC_##K29, AC_##K2A, AC_##K2B, AC_##K2C, AC_##K2D, AC_##K2E, AC_##K2F }, /* 28-2F */ \
    { AC_##K30, AC_##K31, AC_##K32, AC_##K33, AC_##K34, AC_##K35, AC_##K36, AC_##K37,   /* 30-37 */ \
      AC_##K38, AC_##K39, AC_##K3A, AC_##K3B, AC_##K3C, AC_##K3D, AC_##K3E, AC_##K3F }, /* 38-3F */ \
    { AC_##K40, AC_##K41, AC_##K42, AC_##K43, AC_##K44, AC_##K45, AC_##K46, AC_##K47,   /* 40-47 */ \
      AC_##K48, AC_##K49, AC_##K4A, AC_##K4B, AC_##K4C, AC_##K4D, AC_##K4E, AC_##K4F }, /* 48-4F */ \
    { AC_##K50, AC_##K51, AC_##K52, AC_##K53, AC_##K54, AC_##K55, AC_##K56, AC_##K57,   /* 50-57 */ \
      AC_##K58, AC_##K59, AC_##K5A, AC_##K5B, AC_##K5C, AC_##K5D, AC_##K5E, AC_##K5F }, /* 58-5F */ \
    { AC_##K60, AC_##K61, AC_##K62, AC_##K63, AC_##K64, AC_##K65, AC_##K66, AC_##K67,   /* 60-67 */ \
      AC_##K68, AC_##K69, AC_##K6A, AC_##K6B, AC_##K6C, AC_##K6D, AC_##K6E, AC_##K6F }, /* 68-6F */ \
    { AC_##K70, AC_##K71, AC_##K72, AC_##K73, AC_##K74, AC_##K75, AC_##K76, AC_##K77,   /* 70-77 */ \
      AC_##K78, AC_##K79, AC_##K7A, AC_##K7B, AC_##K7C, AC_##K7D, AC_##K7E, AC_##K7F }  /* 78-7F */ \
}


/* Matrix poision for Universal 128-key keyboard
        ,-----------------------------------------------.
        | 68| 69| 6A| 6B| 6C| 6D| 6E| 6F| 70| 71| 72| 73|
,---.   |-----------------------------------------------|     ,-----------.     ,-----------.
| 29|   | 3A| 3B| 3C| 3D| 3E| 3F| 40| 41| 42| 43| 44| 45|     | 46| 47| 48|     | 01| 02| 03|
`---'   `-----------------------------------------------'     `-----------'     `-----------'
,-----------------------------------------------------------. ,-----------. ,---------------.
| 35| 1E| 1F| 20| 21| 22| 23| 24| 25| 26| 27| 2D| 2E| 74| 2A| | 49| 4A| 4B| | 53| 54| 55| 56|
|-----------------------------------------------------------| |-----------| |---------------|
|   2B| 14| 1A| 08| 15| 17| 1C| 18| 0C| 12| 13| 2F| 30|   31| | 4C| 4D| 4E| | 5F| 60| 61| 57|
|-----------------------------------------------------------| `-----------' |---------------|
|    39| 04| 16| 07| 09| 0A| 0B| 0D| 0E| 0F| 33| 34| 32|  28|               | 5C| 5D| 5E| 66|
|-----------------------------------------------------------|     ,---.     |---------------|
|  79| 64| 1D| 1B| 06| 19| 05| 11| 10| 36| 37| 38| 75|    7D|     | 52|     | 59| 5A| 5B| 58|
|-----------------------------------------------------------| ,-----------. |---------------|
| 78| 7B| 7A|  77|              2C|  76|  00| 7E| 7F| 65| 7C| | 50| 51| 4F| |     62| 63| 67|
`-----------------------------------------------------------' `-----------' `---------------'
*/
// Universal map position codes
enum unimap_position_codes {
//  logical name            position(row << 4 | col)
//  ------------------------------------------------
    UNIMAP_KANA,            // 0x00
    UNIMAP_VOLUME_DOWN,     // 0x01
    UNIMAP_VOLUME_UP,       // 0x02
    UNIMAP_VOLUME_MUTE,     // 0x03
    UNIMAP_A,               // 0x04
    UNIMAP_B,               // 0x05
    UNIMAP_C,               // 0x06
    UNIMAP_D,               // 0x07
    UNIMAP_E,               // 0x08
    UNIMAP_F,               // 0x09
    UNIMAP_G,               // 0x0A
    UNIMAP_H,               // 0x0B
    UNIMAP_I,               // 0x0C
    UNIMAP_J,               // 0x0D
    UNIMAP_K,               // 0x0E
    UNIMAP_L,               // 0x0F
    UNIMAP_M,               // 0x10
    UNIMAP_N,               // 0x11
    UNIMAP_O,               // 0x12
    UNIMAP_P,               // 0x13
    UNIMAP_Q,               // 0x14
    UNIMAP_R,               // 0x15
    UNIMAP_S,               // 0x16
    UNIMAP_T,               // 0x17
    UNIMAP_U,               // 0x18
    UNIMAP_V,               // 0x19
    UNIMAP_W,               // 0x1A
    UNIMAP_X,               // 0x1B
    UNIMAP_Y,               // 0x1C
    UNIMAP_Z,               // 0x1D
    UNIMAP_1,               // 0x1E
    UNIMAP_2,               // 0x1F
    UNIMAP_3,               // 0x20
    UNIMAP_4,               // 0x21
    UNIMAP_5,               // 0x22
    UNIMAP_6,               // 0x23
    UNIMAP_7,               // 0x24
    UNIMAP_8,               // 0x25
    UNIMAP_9,               // 0x26
    UNIMAP_0,               // 0x27
    UNIMAP_ENTER,           // 0x28
    UNIMAP_ESCAPE,          // 0x29
    UNIMAP_BSPACE,          // 0x2A
    UNIMAP_TAB,             // 0x2B
    UNIMAP_SPACE,           // 0x2C
    UNIMAP_MINUS,           // 0x2D
    UNIMAP_EQUAL,           // 0x2E
    UNIMAP_LBRACKET,        // 0x2F
    UNIMAP_RBRACKET,        // 0x30
    UNIMAP_BSLASH,          // 0x31
    UNIMAP_NONUS_HASH,      // 0x32 ISO UK hash
    UNIMAP_SCOLON,          // 0x33
    UNIMAP_QUOTE,           // 0x34
    UNIMAP_GRAVE,           // 0x35
    UNIMAP_COMMA,           // 0x36
    UNIMAP_DOT,             // 0x37
    UNIMAP_SLASH,           // 0x38
    UNIMAP_CAPSLOCK,        // 0x39
    UNIMAP_F1,              // 0x3A
    UNIMAP_F2,              // 0x3B
    UNIMAP_F3,              // 0x3C
    UNIMAP_F4,              // 0x3D
    UNIMAP_F5,              // 0x3E
    UNIMAP_F6,              // 0x3F
    UNIMAP_F7,              // 0x40
    UNIMAP_F8,              // 0x41
    UNIMAP_F9,              // 0x42
    UNIMAP_F10,             // 0x43
    UNIMAP_F11,             // 0x44
    UNIMAP_F12,             // 0x45
    UNIMAP_PSCREEN,         // 0x46
    UNIMAP_SCROLLLOCK,      // 0x47
    UNIMAP_PAUSE,           // 0x48
    UNIMAP_INSERT,          // 0x49
    UNIMAP_HOME,            // 0x4A
    UNIMAP_PGUP,            // 0x4B
    UNIMAP_DELETE,          // 0x4C
    UNIMAP_END,             // 0x4D
    UNIMAP_PGDOWN,          // 0x4E
    UNIMAP_RIGHT,           // 0x4F
    UNIMAP_LEFT,            // 0x50
    UNIMAP_DOWN,            // 0x51
    UNIMAP_UP,              // 0x52
    UNIMAP_NUMLOCK,         // 0x53
    UNIMAP_KP_SLASH,        // 0x54
    UNIMAP_KP_ASTERISK,     // 0x55
    UNIMAP_KP_MINUS,        // 0x56
    UNIMAP_KP_PLUS,         // 0x57
    UNIMAP_KP_ENTER,        // 0x58
    UNIMAP_KP_1,            // 0x59
    UNIMAP_KP_2,            // 0x5A
    UNIMAP_KP_3,            // 0x5B
    UNIMAP_KP_4,            // 0x5C
    UNIMAP_KP_5,            // 0x5D
    UNIMAP_KP_6,            // 0x5E
    UNIMAP_KP_7,            // 0x5F
    UNIMAP_KP_8,            // 0x60
    UNIMAP_KP_9,            // 0x61
    UNIMAP_KP_0,            // 0x62
    UNIMAP_KP_DOT,          // 0x63
    UNIMAP_NONUS_BSLASH,    // 0x64 ISO UK backslash
    UNIMAP_APPLICATION,     // 0x65
    UNIMAP_KP_COMMA,        // 0x66
    UNIMAP_KP_EQUAL,        // 0x67
    UNIMAP_F13,             // 0x68
    UNIMAP_F14,             // 0x69
    UNIMAP_F15,             // 0x6A
    UNIMAP_F16,             // 0x6B
    UNIMAP_F17,             // 0x6C
    UNIMAP_F18,             // 0x6D
    UNIMAP_F19,             // 0x6E
    UNIMAP_F20,             // 0x6F
    UNIMAP_F21,             // 0x70
    UNIMAP_F22,             // 0x71
    UNIMAP_F23,             // 0x72
    UNIMAP_F24,             // 0x73
    UNIMAP_JYEN,            // 0x74
    UNIMAP_RO,              // 0x75
    UNIMAP_HENK,            // 0x76
    UNIMAP_MHEN,            // 0x77
    UNIMAP_LCTRL,           // 0x78
    UNIMAP_LSHIFT,          // 0x79
    UNIMAP_LALT,            // 0x7A
    UNIMAP_LGUI,            // 0x7B
    UNIMAP_RCTRL,           // 0x7C
    UNIMAP_RSHIFT,          // 0x7D
    UNIMAP_RALT,            // 0x7E
    UNIMAP_RGUI,            // 0x7F
    UNIMAP_NO,              // 0x80
};

/*
 * Short names
 */
#define UNIMAP_LCTL UNIMAP_LCTRL
#define UNIMAP_RCTL UNIMAP_RCTRL
#define UNIMAP_LSFT UNIMAP_LSHIFT
#define UNIMAP_RSFT UNIMAP_RSHIFT
#define UNIMAP_ESC  UNIMAP_ESCAPE
#define UNIMAP_BSPC UNIMAP_BSPACE
#define UNIMAP_ENT  UNIMAP_ENTER
#define UNIMAP_DEL  UNIMAP_DELETE
#define UNIMAP_INS  UNIMAP_INSERT
#define UNIMAP_CAPS UNIMAP_CAPSLOCK
#define UNIMAP_CLCK UNIMAP_CAPSLOCK
#define UNIMAP_RGHT UNIMAP_RIGHT
#define UNIMAP_PGDN UNIMAP_PGDOWN
#define UNIMAP_PSCR UNIMAP_PSCREEN
#define UNIMAP_SLCK UNIMAP_SCROLLLOCK
#define UNIMAP_PAUS UNIMAP_PAUSE
#define UNIMAP_BRK  UNIMAP_PAUSE
#define UNIMAP_NLCK UNIMAP_NUMLOCK
#define UNIMAP_SPC  UNIMAP_SPACE
#define UNIMAP_MINS UNIMAP_MINUS
#define UNIMAP_EQL  UNIMAP_EQUAL
#define UNIMAP_GRV  UNIMAP_GRAVE
#define UNIMAP_RBRC UNIMAP_RBRACKET
#define UNIMAP_LBRC UNIMAP_LBRACKET
#define UNIMAP_COMM UNIMAP_COMMA
#define UNIMAP_BSLS UNIMAP_BSLASH
#define UNIMAP_SLSH UNIMAP_SLASH
#define UNIMAP_SCLN UNIMAP_SCOLON
#define UNIMAP_QUOT UNIMAP_QUOTE
#define UNIMAP_APP  UNIMAP_APPLICATION
#define UNIMAP_NUHS UNIMAP_NONUS_HASH
#define UNIMAP_NUBS UNIMAP_NONUS_BSLASH
/* Japanese specific */
#define UNIMAP_ZKHK UNIMAP_GRAVE
/* Keypad */
#define UNIMAP_P1   UNIMAP_KP_1
#define UNIMAP_P2   UNIMAP_KP_2
#define UNIMAP_P3   UNIMAP_KP_3
#define UNIMAP_P4   UNIMAP_KP_4
#define UNIMAP_P5   UNIMAP_KP_5
#define UNIMAP_P6   UNIMAP_KP_6
#define UNIMAP_P7   UNIMAP_KP_7
#define UNIMAP_P8   UNIMAP_KP_8
#define UNIMAP_P9   UNIMAP_KP_9
#define UNIMAP_P0   UNIMAP_KP_0
#define UNIMAP_PDOT UNIMAP_KP_DOT
#define UNIMAP_PCMM UNIMAP_KP_COMMA
#define UNIMAP_PSLS UNIMAP_KP_SLASH
#define UNIMAP_PAST UNIMAP_KP_ASTERISK
#define UNIMAP_PMNS UNIMAP_KP_MINUS
#define UNIMAP_PPLS UNIMAP_KP_PLUS
#define UNIMAP_PEQL UNIMAP_KP_EQUAL
#define UNIMAP_PENT UNIMAP_KP_ENTER
/* Consumer Page */
#define UNIMAP_MUTE UNIMAP_VOLUME_MUTE
#define UNIMAP_VOLU UNIMAP_VOLUME_UP
#define UNIMAP_VOLD UNIMAP_VOLUME_DOWN



/*more shrot*/
#define UM_KANA UNIMAP_KANA            // 0x00
#define UM_VOLUME_DOWN UNIMAP_VOLUME_DOWN     // 0x01
#define UM_VOLUME_UP UNIMAP_VOLUME_UP       // 0x02
#define UM_VOLUME_MUTE UNIMAP_VOLUME_MUTE     // 0x03
#define UM_A UNIMAP_A               // 0x04
#define UM_B UNIMAP_B               // 0x05
#define UM_C UNIMAP_C               // 0x06
#define UM_D UNIMAP_D               // 0x07
#define UM_E UNIMAP_E               // 0x08
#define UM_F UNIMAP_F               // 0x09
#define UM_G UNIMAP_G               // 0x0A
#define UM_H UNIMAP_H               // 0x0B
#define UM_I UNIMAP_I               // 0x0C
#define UM_J UNIMAP_J               // 0x0D
#define UM_K UNIMAP_K               // 0x0E
#define UM_L UNIMAP_L               // 0x0F
#define UM_M UNIMAP_M               // 0x10
#define UM_N UNIMAP_N               // 0x11
#define UM_O UNIMAP_O               // 0x12
#define UM_P UNIMAP_P               // 0x13
#define UM_Q UNIMAP_Q               // 0x14
#define UM_R UNIMAP_R               // 0x15
#define UM_S UNIMAP_S               // 0x16
#define UM_T UNIMAP_T               // 0x17
#define UM_U UNIMAP_U               // 0x18
#define UM_V UNIMAP_V               // 0x19
#define UM_W UNIMAP_W               // 0x1A
#define UM_X UNIMAP_X               // 0x1B
#define UM_Y UNIMAP_Y               // 0x1C
#define UM_Z UNIMAP_Z               // 0x1D
#define UM_1 UNIMAP_1               // 0x1E
#define UM_2 UNIMAP_2               // 0x1F
#define UM_3 UNIMAP_3               // 0x20
#define UM_4 UNIMAP_4               // 0x21
#define UM_5 UNIMAP_5               // 0x22
#define UM_6 UNIMAP_6               // 0x23
#define UM_7 UNIMAP_7               // 0x24
#define UM_8 UNIMAP_8               // 0x25
#define UM_9 UNIMAP_9               // 0x26
#define UM_0 UNIMAP_0               // 0x27
#define UM_ENTER UNIMAP_ENTER           // 0x28
#define UM_ESCAPE UNIMAP_ESCAPE          // 0x29
#define UM_BSPACE UNIMAP_BSPACE          // 0x2A
#define UM_TAB UNIMAP_TAB             // 0x2B
#define UM_SPACE UNIMAP_SPACE           // 0x2C
#define UM_MINUS UNIMAP_MINUS           // 0x2D
#define UM_EQUAL UNIMAP_EQUAL           // 0x2E
#define UM_LBRACKET UNIMAP_LBRACKET        // 0x2F
#define UM_RBRACKET UNIMAP_RBRACKET        // 0x30
#define UM_BSLASH UNIMAP_BSLASH          // 0x31
#define UM_NONUS_HASH UNIMAP_NONUS_HASH      // 0x32 ISO UK hasu
#define UM_SCOLON UNIMAP_SCOLON          // 0x33
#define UM_QUOTE UNIMAP_QUOTE           // 0x34
#define UM_GRAVE UNIMAP_GRAVE           // 0x35
#define UM_COMMA UNIMAP_COMMA           // 0x36
#define UM_DOT UNIMAP_DOT             // 0x37
#define UM_SLASH UNIMAP_SLASH           // 0x38
#define UM_CAPSLOCK UNIMAP_CAPSLOCK        // 0x39
#define UM_F1 UNIMAP_F1              // 0x3A
#define UM_F2 UNIMAP_F2              // 0x3B
#define UM_F3 UNIMAP_F3              // 0x3C
#define UM_F4 UNIMAP_F4              // 0x3D
#define UM_F5 UNIMAP_F5              // 0x3E
#define UM_F6 UNIMAP_F6              // 0x3F
#define UM_F7 UNIMAP_F7              // 0x40
#define UM_F8 UNIMAP_F8              // 0x41
#define UM_F9 UNIMAP_F9              // 0x42
#define UM_F10 UNIMAP_F10             // 0x43
#define UM_F11 UNIMAP_F11             // 0x44
#define UM_F12 UNIMAP_F12             // 0x45
#define UM_PSCREEN UNIMAP_PSCREEN         // 0x46
#define UM_SCROLLLOCK UNIMAP_SCROLLLOCK      // 0x47
#define UM_PAUSE UNIMAP_PAUSE           // 0x48
#define UM_INSERT UNIMAP_INSERT          // 0x49
#define UM_HOME UNIMAP_HOME            // 0x4A
#define UM_PGUP UNIMAP_PGUP            // 0x4B
#define UM_DELETE UNIMAP_DELETE          // 0x4C
#define UM_END UNIMAP_END             // 0x4D
#define UM_PGDOWN UNIMAP_PGDOWN          // 0x4E
#define UM_RIGHT UNIMAP_RIGHT           // 0x4F
#define UM_LEFT UNIMAP_LEFT            // 0x50
#define UM_DOWN UNIMAP_DOWN            // 0x51
#define UM_UP UNIMAP_UP              // 0x52
#define UM_NUMLOCK UNIMAP_NUMLOCK         // 0x53
#define UM_KP_SLASH UNIMAP_KP_SLASH        // 0x54
#define UM_KP_ASTERISK UNIMAP_KP_ASTERISK     // 0x55
#define UM_KP_MINUS UNIMAP_KP_MINUS        // 0x56
#define UM_KP_PLUS UNIMAP_KP_PLUS         // 0x57
#define UM_KP_ENTER UNIMAP_KP_ENTER        // 0x58
#define UM_KP_1 UNIMAP_KP_1            // 0x59
#define UM_KP_2 UNIMAP_KP_2            // 0x5A
#define UM_KP_3 UNIMAP_KP_3            // 0x5B
#define UM_KP_4 UNIMAP_KP_4            // 0x5C
#define UM_KP_5 UNIMAP_KP_5            // 0x5D
#define UM_KP_6 UNIMAP_KP_6            // 0x5E
#define UM_KP_7 UNIMAP_KP_7            // 0x5F
#define UM_KP_8 UNIMAP_KP_8            // 0x60
#define UM_KP_9 UNIMAP_KP_9            // 0x61
#define UM_KP_0 UNIMAP_KP_0            // 0x62
#define UM_KP_DOT UNIMAP_KP_DOT          // 0x63
#define UM_NONUS_BSLASH UNIMAP_NONUS_BSLASH    // 0x64 ISO UK backslash
#define UM_APPLICATION UNIMAP_APPLICATION     // 0x65
#define UM_KP_COMMA UNIMAP_KP_COMMA        // 0x66
#define UM_KP_EQUAL UNIMAP_KP_EQUAL        // 0x67
#define UM_F13 UNIMAP_F13             // 0x68
#define UM_F14 UNIMAP_F14             // 0x69
#define UM_F15 UNIMAP_F15             // 0x6A
#define UM_F16 UNIMAP_F16             // 0x6B
#define UM_F17 UNIMAP_F17             // 0x6C
#define UM_F18 UNIMAP_F18             // 0x6D
#define UM_F19 UNIMAP_F19             // 0x6E
#define UM_F20 UNIMAP_F20             // 0x6F
#define UM_F21 UNIMAP_F21             // 0x70
#define UM_F22 UNIMAP_F22             // 0x71
#define UM_F23 UNIMAP_F23             // 0x72
#define UM_F24 UNIMAP_F24             // 0x73
#define UM_JYEN UNIMAP_JYEN            // 0x74
#define UM_RO UNIMAP_RO              // 0x75
#define UM_HENK UNIMAP_HENK            // 0x76
#define UM_MHEN UNIMAP_MHEN            // 0x77
#define UM_LCTRL UNIMAP_LCTRL           // 0x78
#define UM_LSHIFT UNIMAP_LSHIFT          // 0x79
#define UM_LALT UNIMAP_LALT            // 0x7A
#define UM_LGUI UNIMAP_LGUI            // 0x7B
#define UM_RCTRL UNIMAP_RCTRL           // 0x7C
#define UM_RSHIFT UNIMAP_RSHIFT          // 0x7D
#define UM_RALT UNIMAP_RALT            // 0x7E
#define UM_RGUI UNIMAP_RGUI            // 0x7F
#define UM_NO UNIMAP_NO              // 0x80
/*short*/
#define UM_LCTL UNIMAP_LCTRL
#define UM_RCTL UNIMAP_RCTRL
#define UM_LSFT UNIMAP_LSHIFT
#define UM_RSFT UNIMAP_RSHIFT
#define UM_ESC  UNIMAP_ESCAPE
#define UM_BSPC UNIMAP_BSPACE
#define UM_ENT  UNIMAP_ENTER
#define UM_DEL  UNIMAP_DELETE
#define UM_INS  UNIMAP_INSERT
#define UM_CAPS UNIMAP_CAPSLOCK
#define UM_CLCK UNIMAP_CAPSLOCK
#define UM_RGHT UNIMAP_RIGHT
#define UM_PGDN UNIMAP_PGDOWN
#define UM_PSCR UNIMAP_PSCREEN
#define UM_SLCK UNIMAP_SCROLLLOCK
#define UM_PAUS UNIMAP_PAUSE
#define UM_BRK  UNIMAP_PAUSE
#define UM_NLCK UNIMAP_NUMLOCK
#define UM_SPC  UNIMAP_SPACE
#define UM_MINS UNIMAP_MINUS
#define UM_EQL  UNIMAP_EQUAL
#define UM_GRV  UNIMAP_GRAVE
#define UM_RBRC UNIMAP_RBRACKET
#define UM_LBRC UNIMAP_LBRACKET
#define UM_COMM UNIMAP_COMMA
#define UM_BSLS UNIMAP_BSLASH
#define UM_SLSH UNIMAP_SLASH
#define UM_SCLN UNIMAP_SCOLON
#define UM_QUOT UNIMAP_QUOTE
#define UM_APP  UNIMAP_APPLICATION
#define UM_NUHS UNIMAP_NONUS_HASH
#define UM_NUBS UNIMAP_NONUS_BSLASH
/* Japanese specific */
#define UM_ZKHK UNIMAP_GRAVE
/* Keypad */
#define UM_P1   UNIMAP_KP_1
#define UM_P2   UNIMAP_KP_2
#define UM_P3   UNIMAP_KP_3
#define UM_P4   UNIMAP_KP_4
#define UM_P5   UNIMAP_KP_5
#define UM_P6   UNIMAP_KP_6
#define UM_P7   UNIMAP_KP_7
#define UM_P8   UNIMAP_KP_8
#define UM_P9   UNIMAP_KP_9
#define UM_P0   UNIMAP_KP_0
#define UM_PDOT UNIMAP_KP_DOT
#define UM_PCMM UNIMAP_KP_COMMA
#define UM_PSLS UNIMAP_KP_SLASH
#define UM_PAST UNIMAP_KP_ASTERISK
#define UM_PMNS UNIMAP_KP_MINUS
#define UM_PPLS UNIMAP_KP_PLUS
#define UM_PEQL UNIMAP_KP_EQUAL
#define UM_PENT UNIMAP_KP_ENTER
/* Consumer Page */
#define UM_MUTE UNIMAP_VOLUME_MUTE
#define UM_VOLU UNIMAP_VOLUME_UP
#define UM_VOLD UNIMAP_VOLUME_DOWN

#endif
