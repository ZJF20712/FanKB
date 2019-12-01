#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "progmem.h"
#include "timer.h"
#include "rgblight.h"
#include "debug.h"
#include "is31fl3733.h"
#include "ble51.h"
#include "keycode.h"
#include "hook.h"
#include "keymap.h"
#include "TWIlib.h"

#define SRGBLED_NUM 47
#define rgblight_timer_init() do { DDRB |= (1<<PB7); DDRD |= (1<<PD4);} while(0)
#define rgblight_timer_enable() do { PORTB |= (1<<PB7); PORTD &= ~(1<<PD4);} while(0)
#define rgblight_timer_disable() do { PORTB &= ~(1<<PB7);  PORTD |= (1<<PD4);} while(0)
#define rgblight_timer_enabled (PORTB&(1<<PB7))

const uint8_t RGBLED_BREATHING_TABLE[128] PROGMEM= {0,0,0,0,1,1,1,2,2,3,4,5,5,6,7,9,10,11,12,14,15,17,18,20,21,23,25,27,29,31,33,35,37,40,42,44,47,49,52,54,57,59,62,65,67,70,73,76,79,82,85,88,90,93,97,100,103,106,109,112,115,118,121,124,127,131,134,137,140,143,146,149,152,155,158,162,165,167,170,173,176,179,182,185,188,190,193,196,198,201,203,206,208,211,213,215,218,220,222,224,226,228,230,232,234,235,237,238,240,241,243,244,245,246,248,249,250,250,251,252,253,253,254,254,254,255,255,255};
const uint8_t SLED_MATRIX_MASK[4][12] = 
{
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11},
    {12,13,14,15,16,17,18,19,20,21,22,23},
    {24,25,26,27,28,29,30,31,32,33,34,35},
    {36,37,38,39,40,41,47,42,43,44,45,46},
};

static uint8_t fading_single_key[SRGBLED_NUM+1] = {0};
static uint8_t hue_single_key[SRGBLED_NUM+1] = {0};
static uint8_t fading_single_underglow[RGBLED_NUM] = {0};
static uint8_t hue_single_underglow[RGBLED_NUM] = {0};
extern uint32_t kb_idle_timer; 

rgblight_config_t rgblight_config;
rgblight_config_t inmem_config;
struct cRGB led[RGBLED_NUM];
struct cRGB sled[SRGBLED_NUM+1];

static int8_t wave_col_l[4] = {0};
static int8_t wave_col_r[4] = {0};
static uint8_t wave_num = 0;
static uint8_t horse_race_remain = 0;
//rgblight_layer
bool rgb_layer_update = false;
extern uint32_t layer_state; 

void sethsv(uint16_t hue, uint8_t saturation, uint8_t brightness, struct cRGB *led1)
{
    /*
    original code: https://blog.adafruit.com/2012/03/14/constant-brightness-hsb-to-rgb-algorithm/
    */
    uint8_t r, g, b;
    uint8_t temp[5];
    uint8_t n = (hue >> 8) % 3;
    uint8_t x = ((((hue & 255) * saturation) >> 8) * brightness) >> 8;
    uint8_t s = ((256 - saturation) * brightness) >> 8;
    temp[0] = temp[3] = s;
    temp[1] = temp[4] = x + s;
    temp[2] = brightness - x;
    r = temp[n + 2];
    g = temp[n + 1];
    b = temp[n];
    setrgb(r,g,b, led1);
}

void sethsv_s255(uint8_t hue, uint8_t brightness, struct cRGB *led1)
{
    uint8_t temp[5];
    uint8_t n = (hue*3) >> 8;
    temp[0] = temp[3] = 0;
    temp[1] = temp[4] = ((uint8_t)(hue * 3 -1) * brightness) >> 8;
    temp[2] = brightness - temp[1];
    (*led1).r = temp[n + 2];
    (*led1).g = temp[n + 1];
    (*led1).b = temp[n];
}

void setrgb(uint8_t r, uint8_t g, uint8_t b, struct cRGB *led1)
{
    (*led1).r = r;
    (*led1).g = g;
    (*led1).b = b;
}


uint32_t eeconfig_read_rgblight(void)
{
    return eeprom_read_dword(EECONFIG_RGBLIGHT);
}

void eeconfig_write_rgblight(uint32_t val)
{
    eeprom_update_dword(EECONFIG_RGBLIGHT, val);
}

void eeconfig_write_rgblight_default(void)
{
    dprintf("eeconfig_write_rgblight_default\n");
    rgblight_config.enable = 1;
    rgblight_config.mode = 12;
    rgblight_config.hue = 640;
    rgblight_config.sat = 255;
    rgblight_config.val = 255;
    eeconfig_write_rgblight(rgblight_config.raw);
}

void eeconfig_debug_rgblight(void) {
    dprintf("rgblight_config eeprom\n");
    dprintf("enable = %d\n", rgblight_config.enable);
    dprintf("mode = %d\n", rgblight_config.mode);
    dprintf("hue = %d\n", rgblight_config.hue);
    dprintf("sat = %d\n", rgblight_config.sat);
    dprintf("val = %d\n", rgblight_config.val);
}

void rgblight_init(void)
{
    dprintf("rgblight_init start!\n");
    if (!eeconfig_is_enabled()) {
        dprintf("rgblight_init eeconfig is not enabled.\n");
        eeconfig_init();
        eeconfig_write_rgblight_default();
    }
    rgblight_config.raw = eeconfig_read_rgblight();

    if (rgblight_config.mode == 0b11111) {  
        dprintf("rgblight_init rgblight_config.mode = 0. Write default values to EEPROM.\n");
        eeconfig_write_rgblight_default();
        rgblight_config.raw = eeconfig_read_rgblight();
    }
    eeconfig_debug_rgblight(); 

    rgblight_timer_init(); 

    if (rgblight_config.enable) {
        rgblight_mode(rgblight_config.mode);
    } else {
        rgblight_set();
        rgblight_timer_disable();
    }
}


void rgblight_mode(uint8_t mode)
{
    if (!rgblight_config.enable) {
        return;
    }
    if (mode < 1) {
        rgblight_config.mode = RGBLIGHT_MODES;
    } else if (mode > RGBLIGHT_MODES) {
        rgblight_config.mode = 1;
    } else {
        rgblight_config.mode = mode;
    }
    eeconfig_write_rgblight(rgblight_config.raw);
    dprintf("rgblight mode: %u\n", rgblight_config.mode);
    if (rgblight_config.mode > 0) {
        rgblight_timer_enable();
    }

    if (rgblight_config.mode == 1) {
        rgblight_sethsv(rgblight_config.hue, rgblight_config.sat, rgblight_config.val);
        rgblight_set();
    } else if (mode == 2) {
        memset(&fading_single_key[0], 0, SRGBLED_NUM);
        rgblight_clear();
    } else if (mode == 4) {
        hook_layer_change(layer_state);
    } else if (mode >= 9 && mode <= 10) {
        horse_race_remain = 0;
    }
}

inline
void rgblight_toggle(void)
{
    rgblight_config.enable ^= 1;
    eeconfig_write_rgblight(rgblight_config.raw);
    dprintf("rgblight toggle: rgblight_config.enable = %u\n", rgblight_config.enable);
    if (rgblight_config.enable) {
        rgblight_mode(rgblight_config.mode);
    } else {
        rgblight_clear();
        rgblight_set();
        if (USB_DeviceState != DEVICE_STATE_Configured) {
            rgblight_timer_disable();
        }
    }
}


void rgblight_action(uint8_t action)
{
    /*  0 toggle
    1 mode-    2 mode+
    3 hue-     4 hue+
    5 sat-     6 sat+
    7 val-     8 val+
    */
    uint16_t hue = rgblight_config.hue;
    int16_t sat = rgblight_config.sat;
    int16_t val = rgblight_config.val;
    int8_t increament = 1;
    if (action % 2) increament = -1;
    if (get_mods() & MOD_BIT(KC_LSHIFT)) {
        increament *= -1;
    } 
    switch (action) {
        case 0: 
            rgblight_toggle();
            break;
        case 1:
        case 2:
            rgblight_mode(rgblight_config.mode + increament);
            break;
        case 3:
        case 4:
            hue = (rgblight_config.hue + 768 + RGBLIGHT_HUE_STEP * increament) % 768;
            break;
        case 5:
        case 6:
            sat = rgblight_config.sat + RGBLIGHT_SAT_STEP * increament;
            if (sat > 255) sat = 255;
            if (sat < 0) sat = 0;
            break;
        case 7:
        case 8:
            val = rgblight_config.val + RGBLIGHT_VAL_STEP * increament;
            if (val > 255) val = 255;
            if (val < 0) val = 0;
            break;
        default:
            break;
    }
    if (action >= 3) rgblight_sethsv(hue, sat, val);
}

void rgblight_sethsv_noeeprom(uint16_t hue, uint8_t sat, uint8_t val)
{
    if (rgblight_config.enable) {
        sethsv(hue, sat, val, &sled[SRGBLED_NUM]);
        for (uint8_t i=0; i< SRGBLED_NUM; i++) {
            if (i<RGBLED_NUM) led[i] = sled[SRGBLED_NUM];
            sled[i] = sled[SRGBLED_NUM];
        }
        rgblight_set();
    }
}

void rgblight_sethsv(uint16_t hue, uint8_t sat, uint8_t val)
{
    if (rgblight_config.enable) {
        if (rgblight_config.mode == 1) {
            rgblight_sethsv_noeeprom(hue, sat, val);
        } 
        rgblight_config.hue = hue;
        rgblight_config.sat = sat;
        rgblight_config.val = val;
        eeconfig_write_rgblight(rgblight_config.raw);
        dprintf("rgblight set hsv [EEPROM]: %u,%u,%u\n", rgblight_config.hue, rgblight_config.sat, rgblight_config.val);
    }
}

void rgblight_setrgb(uint8_t r, uint8_t g, uint8_t b)
{
    for (uint8_t i=0;i<RGBLED_NUM;i++) {
        led[i].r = r;
        led[i].g = g;
        led[i].b = b;
    }
    for ( uint8_t i=0; i<SRGBLED_NUM; i++) {
        sled[i].r = r;
        sled[i].g = g;
        sled[i].b = b;
    }
    rgblight_set();
}

void rgblight_clear(void)
{
    memset(&sled[0], 0, 3 * SRGBLED_NUM);
    memset(&led[0], 0, 3 * RGBLED_NUM);
}

void rgblight_set(void)
{
    if (!rgblight_config.enable) {
        rgblight_clear();
    } 
    for ( uint8_t i=0; i<SRGBLED_NUM; i++) {
        IS31FL3733_set_color(i, sled[i].r, sled[i].g, sled[i].b);
    }
    ws2812_setleds(led, RGBLED_NUM);
}

inline
void rgblight_task(void)
{
    if (rgblight_config.enable && rgblight_timer_enabled ) {
        switch (rgblight_config.mode) {
            case 2:
                rgblight_effect_click_fading();
                break;
            case 3:
                rgblight_effect_click_wave();
                break;
            case 4:
                rgblight_effect_layer();
                break;
            case 5 ... 6:
                rgblight_effect_stars(rgblight_config.mode-4); // 1 fast, 2 normal
                break;
            case 7 ... 8:
                rgblight_effect_raindrop(rgblight_config.mode-6); // 1 fast, 2 normal
                break;
            case 9:
                rgblight_effect_horse_race(); 
                break;
            case 10:
                rgblight_effect_hit(); 
                break;
            case 11 ... 12:
                rgblight_effect_breathing(rgblight_config.mode-9);  // 2 slow, 3 fast
                break;
            case 13 ... 14:
                rgblight_effect_rainbow_mood(rgblight_config.mode-11); // 2 slow, 3 fast
                break;
            case 15 ... 18:
                rgblight_effect_rainbow_swirl(rgblight_config.mode-13); // 2 fast,
                break;
            case 19 ... 22:
                rgblight_effect_snake(rgblight_config.mode-19); //0 fast
                break;
            case 23 ... 24:
                rgblight_effect_knight(rgblight_config.mode-23);
                break;
        }
    }
}


inline
void rgblight_matrix(uint8_t row, uint8_t col)
{
    if (!rgblight_config.enable) return;
    if (rgblight_config.mode == 2) {
        if (!rgblight_timer_enabled) rgblight_timer_enable();
        fading_single_key[SLED_MATRIX_MASK[row][col]] = 126;
        uint8_t hue = rand()%256;
        hue_single_key[SLED_MATRIX_MASK[row][col]] = hue;
    }
    if (rgblight_config.mode == 3) {
        if (!rgblight_timer_enabled) rgblight_timer_enable();
        if (wave_col_l[(wave_num + 1)%RGBLIGHT_EFFECT_MAX_WAVES] < -2 && wave_col_r[(wave_num + 1)%RGBLIGHT_EFFECT_MAX_WAVES] >= MATRIX_COLS+2) {
            if (++wave_num >= RGBLIGHT_EFFECT_MAX_WAVES) wave_num = 0;
        }
        wave_col_l[wave_num] = wave_col_r[wave_num] = col;
        uint8_t hue = rand()%256;
        hue_single_key[wave_num] = hue;
    }
}

void rgblight_effect_breathing(uint8_t interval)
{
    static uint8_t pos = 0;
    static int8_t increament = 1;
    rgblight_sethsv_noeeprom(rgblight_config.hue, rgblight_config.sat, pgm_read_byte(&RGBLED_BREATHING_TABLE[pos]));
    pos = pos + interval*increament;
    if (pos < interval || pos+interval > 126) {
        increament = ~increament + 1;
    }
}

void rgblight_effect_click_fading(void)
{
    uint8_t i;
    bool key_fading = 0;
    for (i = 0; i < SRGBLED_NUM; i++) {
        if (fading_single_key[i] > 1) {
            sethsv_s255(hue_single_key[i], pgm_read_byte(&RGBLED_BREATHING_TABLE[fading_single_key[i]]), &sled[i]);
            fading_single_key[i] -= 2;
            key_fading = 1;
        }
    }
    for (i = 0; i < RGBLED_NUM; i++) {
        if (fading_single_underglow[i] > 1) {
            sethsv_s255(hue_single_underglow[i], pgm_read_byte(&RGBLED_BREATHING_TABLE[fading_single_underglow[i]]), &led[i]);
            fading_single_underglow[i] -= 2;
            key_fading = 1;
        }
    }
    if (!key_fading && rgblight_config.mode == 2 && USB_DeviceState != DEVICE_STATE_Configured) { 
        rgblight_timer_disable();
    }
    rgblight_set();	
}

void rgblight_effect_click_wave(void)
{
    static uint8_t step = 0;
    uint8_t i;
    if (++step > 1) {
        step = 0;
        rgblight_clear();
        for (uint8_t j=0; j<RGBLIGHT_EFFECT_MAX_WAVES; j++) {
            sethsv_s255(hue_single_key[j], 255, &sled[SRGBLED_NUM]);
            if (wave_col_l[j] >= -2) {
                for (i=0; i<MATRIX_ROWS; i++) {
                    if (wave_col_l[j] >= 0) {
                        sled[SLED_MATRIX_MASK[i][wave_col_l[j]]] = sled[SRGBLED_NUM];
                    } 
                }
                wave_col_l[j]--;
            } 
            if (wave_col_r[j] < (MATRIX_COLS + 2)) {
                for (i=0; i<MATRIX_ROWS; i++) {
                    if (wave_col_r[j] < MATRIX_COLS) {
                        sled[SLED_MATRIX_MASK[i][wave_col_r[j]]] = sled[SRGBLED_NUM];
                    }
                }
                wave_col_r[j]++;
            } 
        }
        if ((wave_col_l[wave_num] < -2 && wave_col_r[wave_num] >= (MATRIX_COLS + 2)) && USB_DeviceState != DEVICE_STATE_Configured) {
            rgblight_timer_disable();
        } else {
            rgblight_set();
        }
    }
}

void rgblight_effect_layer(void)
{
    rgblight_clear();
    if (rgb_layer_update) {
        rgb_layer_update = false;
        uint8_t top_layer = 0;
        for (uint8_t i=7; i>0; i--) {
            if (layer_state>>i > 0) {
                top_layer = i;
                break;
            }
        }
        xprintf("top layer: %d\n",top_layer);
        if (top_layer > 0) {
            if (top_layer == 7) setrgb(182, 182, 182, &sled[SRGBLED_NUM]);  // white for layer 7
            else sethsv((top_layer-1)*128, 255, 255, &sled[SRGBLED_NUM]); 
            keypos_t key_check;
            for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
                key_check.row = i;
                for (uint8_t j = 0; j < MATRIX_COLS; j++) {
                    key_check.col = j;
                    uint16_t key_check_code = action_for_key(top_layer, key_check).code;
                    if (key_check_code >= 2) {  //key is not NO or TRANS
                        sled[SLED_MATRIX_MASK[i][j]] = sled[SRGBLED_NUM];
                    }
                }
            }
            for (uint8_t i = 0; i < RGBLED_NUM; i++) {
                led[i] = sled[SRGBLED_NUM];
            }
            rgblight_set(); 
        } else {
            rgblight_set(); 
            if (USB_DeviceState != DEVICE_STATE_Configured) {
                rgblight_timer_disable();
            }
        }
    }
}

void rgblight_effect_stars(uint8_t interval)
{
    static uint8_t timer_step = 0;
    if (++timer_step > interval * 2) { //1 fast, 2 normal
        timer_step = 0;
        uint8_t random_sled_row, random_sled_col;
        random_sled_row = rand() % MATRIX_ROWS;
        random_sled_col = rand() % MATRIX_COLS;
        uint8_t pressed_sled = SLED_MATRIX_MASK[random_sled_row][random_sled_col];
        if (fading_single_key[pressed_sled] <= 1) {
            fading_single_key[pressed_sled] = 126;
            uint8_t hue = rand()%256;
            hue_single_key[pressed_sled] = hue;
        } 
    }
    rgblight_effect_click_fading();
}

void rgblight_effect_raindrop(uint8_t interval)
{
    static uint8_t step = 0;
    uint8_t i,j;
    if (++step > interval) {  //0 fast , 1 normal
        rgblight_clear();
        uint8_t random_sled_col;
        random_sled_col = rand() % MATRIX_COLS;
        if (fading_single_key[random_sled_col] == 0) {
            fading_single_key[random_sled_col] = MATRIX_ROWS + 2 - 1;
            hue_single_key[random_sled_col] = rand() % 256; 
            step = 0;
        }
        for (j=0; j<MATRIX_COLS; j++) {
            if (fading_single_key[j]) {
                sethsv_s255(hue_single_key[j], 255, &sled[SRGBLED_NUM]);
                for (i=MATRIX_ROWS; i<MATRIX_ROWS + 2; i++) {
                    uint8_t tmp_row = i-fading_single_key[j];
                    if (tmp_row >= 0 && tmp_row < MATRIX_ROWS) {
                        sled[SLED_MATRIX_MASK[tmp_row][j]] = sled[SRGBLED_NUM];
                    }
                }
                fading_single_key[j]--;
            } 
        }
        rgblight_set();
    }
} 

void rgblight_effect_horse_race(void)
{
    static uint8_t step = 0;
    static uint8_t start_wait = 0;
    uint8_t i,j;
    if (++step > 0) { 
        memset(&sled[0], 0, 3 * SRGBLED_NUM);
        if (horse_race_remain == 0) {
            horse_race_remain = MATRIX_ROWS;
            start_wait = 20; 
            for (i=0; i<MATRIX_ROWS; i++) {
                fading_single_key[i] = MATRIX_COLS + RGBLIGHT_EFFECT_HORSE_LENGTH - 1;
                hue_single_key[i] = rand()%256; 
                memset(&led[0], 0, 3 * RGBLED_NUM);
            }
        }
        if (start_wait) { 
            start_wait--;
            return;
        }
        for (j=0; j<MATRIX_ROWS; j++) {
            if (fading_single_key[j] >= MATRIX_COLS + RGBLIGHT_EFFECT_HORSE_LENGTH) {
                horse_race_remain = 0;
                return;
            } 
            if (fading_single_key[j]) {
                sethsv_s255(hue_single_key[j], 255, &sled[SRGBLED_NUM]);
                for (i=MATRIX_COLS; i<MATRIX_COLS+RGBLIGHT_EFFECT_HORSE_LENGTH; i++) {
                    uint8_t tmp_col = i-fading_single_key[j];
                    if (tmp_col >= 0 && tmp_col < MATRIX_COLS) {
                        sled[SLED_MATRIX_MASK[j][tmp_col]] = sled[SRGBLED_NUM];
                    } 
                }
                uint8_t random_delay1, random_delay2;
                random_delay1 = rand() % MATRIX_ROWS;
                random_delay2 = rand() % MATRIX_ROWS;
                if (j != random_delay1 && j != random_delay2) {
                    if (--fading_single_key[j] == 0) { 
                        if (horse_race_remain > 0) horse_race_remain--;
                    }
                }
            } 
        }
        rgblight_set();
    }
}

void rgblight_effect_hit(void)
{
    static uint8_t step = 0;
    int8_t i,j;
    uint16_t hue;
    if (++step > 0) { 
        if (horse_race_remain == 0) {
            horse_race_remain = 4;
            for (i=0; i<MATRIX_ROWS; i++) {
                wave_col_l[i] = 0;
                wave_col_r[i] = 11;
            }
        }
        uint8_t random_l, random_l2, random_r, random_r2;
        random_l = rand() % MATRIX_ROWS;
        random_r = rand() % MATRIX_ROWS;
        random_l2 = rand() % MATRIX_ROWS;
        random_r2 = rand() % MATRIX_ROWS;
        for (j=0; j<MATRIX_ROWS; j++) {
            if (wave_col_l[j] > wave_col_r[j]) {
                for (i=1; i<MATRIX_COLS-1; i++) {
                        memset(&sled[SLED_MATRIX_MASK[j][i]], 0, 3);
                }
                wave_col_l[j] = 0;
                wave_col_r[j] = 11;
                continue;
            }
            hue = (rgblight_config.hue+j*128)%768;
            if ((j != random_l && j != random_l2) && wave_col_l[j] >= 0) {
                sethsv(hue, rgblight_config.sat, 255, &sled[SLED_MATRIX_MASK[j][wave_col_l[j]]]);
                if (++wave_col_l[j] > 14) {
                    horse_race_remain = 0;
                    rgblight_clear();
                }
            }
            hue = (hue+300)%768;
            if ((j != random_r && j != random_r2) && wave_col_r[j] > 0) {
                sethsv(hue, rgblight_config.sat, 255, &sled[SLED_MATRIX_MASK[j][wave_col_r[j]]]);
                if ( --wave_col_r[j] == 0) {
                    horse_race_remain = 0;
                    rgblight_clear();
                }
            }
        }
        rgblight_set();
    }
}

void rgblight_effect_rainbow_mood(uint8_t interval)
{
    static uint16_t current_hue = 0;
    rgblight_sethsv_noeeprom(current_hue, rgblight_config.sat, rgblight_config.val);
    current_hue = (current_hue + interval * 3) % 768;
}

void rgblight_effect_rainbow_swirl(uint8_t interval)
{
    static uint16_t current_hue=0;
    uint16_t hue;
    int8_t i,j;
    uint8_t interval2 = interval/2;
    for (i=0; i<MATRIX_COLS; i++) {
        hue = (768/16*i+current_hue)%768;
        sethsv(hue, rgblight_config.sat, rgblight_config.val, &sled[SRGBLED_NUM]);
        for (j=0; j<4; j++) {
            sled[SLED_MATRIX_MASK[j][i]] = sled[SRGBLED_NUM];
        }
        if (i == 0) {
            for (j=0; j<3; j++) {
                led[j] = sled[SRGBLED_NUM];
            }
        } else if (i == 11) {
            for (j=3; j<6; j++) {
                led[j] = sled[SRGBLED_NUM];
            }
        }
    }
    rgblight_set();
    if (interval % 2) {
        current_hue = (current_hue + interval2*16) % 768;
    } else {
        current_hue = (current_hue + 768 - interval2*16) % 768;
    }
}

void rgblight_effect_snake(uint8_t interval)
{
    static int8_t pos1 = 0 - RGBLIGHT_EFFECT_SNAKE_LENGTH;
    static uint8_t sled_step = 0;
    uint8_t i;
    int8_t increament = 1;
    uint8_t interval2 = interval/2;
    if (++sled_step > interval2) {
        sled_step = 0;
        rgblight_clear();
        if (interval%2) increament = -1;
        for (i=0; i<RGBLIGHT_EFFECT_SNAKE_LENGTH; i++) {
            uint8_t *tmp_sled = &SLED_MATRIX_MASK[0][0]+(pos1+i*increament+MATRIX_COLS*MATRIX_ROWS)%(MATRIX_COLS*MATRIX_ROWS);
            sethsv((rgblight_config.hue+i*50)%768, rgblight_config.sat, rgblight_config.val, &sled[*tmp_sled]);
        }
        pos1 += increament;
        if (pos1 > MATRIX_COLS*MATRIX_ROWS) pos1 = 0;
        else if (pos1 < 0) pos1 = MATRIX_COLS*MATRIX_ROWS;
        rgblight_set();
    }
}

void rgblight_effect_knight(uint8_t interval)
{
    static int8_t pos = MATRIX_COLS - 1;
    static uint8_t sled_step = 0;
    static uint16_t current_hue=0;
    uint8_t i,j;
    static int8_t increament = 1;
    if (++sled_step > interval) {
        bool need_update = 0;
        sled_step = 0;
        rgblight_clear();
        for (i=0; i<RGBLIGHT_EFFECT_KNIGHT_LENGTH; i++) {
            if (pos+i < MATRIX_COLS && pos+i >= 0){
                need_update = 1;
                uint8_t tmp_col = pos+i;
                for (j=0; j<4; j++) {
                    sled[SLED_MATRIX_MASK[j][tmp_col]] = sled[SRGBLED_NUM];
                }
                if (tmp_col == 0) {
                    for (j=0; j<3; j++) {
                        led[j] = sled[SRGBLED_NUM];
                    }
                } else if (tmp_col == 11) {
                    for (j=3; j<6; j++) {
                        led[j] = sled[SRGBLED_NUM];
                    }
                }
            }
        }
        if (need_update) rgblight_set();
        pos += increament;
        if (pos <= 0 - RGBLIGHT_EFFECT_KNIGHT_LENGTH || pos >= MATRIX_COLS ) {
            increament = ~increament+1;
            current_hue = (current_hue + 40) % 768;
            sethsv(current_hue, rgblight_config.sat, rgblight_config.val, &sled[SRGBLED_NUM]);
        }
    }
}

#ifdef SUSPEND_ACTION
void suspend_power_down_action(void)
{
    rgblight_timer_disable();
}

void suspend_wakeup_init_action(void)
{
    TWIInfo.mode = 0;
    rgblight_init();
}
#endif

void hook_matrix_change(keyevent_t event)
{
    if (event.pressed) {
        rgblight_matrix(event.key.row, event.key.col);
    }
    kb_idle_timer = timer_read32();
}

void hook_keyboard_loop()
{
    static uint8_t rgb_step = 0;
    if (rgblight_timer_enabled && ++rgb_step > 80) {
        rgb_step = 0;
        rgblight_task();
        IS31FL3733_update_pwm_buffers_all();
    }
}
