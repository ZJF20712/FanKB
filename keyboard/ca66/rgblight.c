#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "progmem.h"
#include "timer.h"
#include "action.h"
#include "rgblight.h"
#include "debug.h"
#include "lufa.h"
#include "keycode.h"
#include "backlight.h"
#include "hook.h"
#define rgblight_timer_init() do { DDRE |= (1<<PE2);} while(0)
#define rgblight_timer_enable() do { PORTE &= ~(1<<PE2);} while(0)
#define rgblight_timer_disable() do { PORTE |= (1<<PE2);} while(0)
#define rgblight_timer_enabled !(PORTE&(1<<PE2))
const uint8_t RGBLED_BREATHING_TABLE[128] PROGMEM = {0,0,0,0,1,1,1,2,2,3,4,5,5,6,7,9,10,11,12,14,15,17,18,20,21,23,25,27,29,31,33,35,37,40,42,44,47,49,52,54,57,59,62,65,67,70,73,76,79,82,85,88,90,93,97,100,103,106,109,112,115,118,121,124,127,131,134,137,140,143,146,149,152,155,158,162,165,167,170,173,176,179,182,185,188,190,193,196,198,201,203,206,208,211,213,215,218,220,222,224,226,228,230,232,234,235,237,238,240,241,243,244,245,246,248,249,250,250,251,252,253,253,254,254,254,255,255,255};

rgblight_config_t rgblight_config;
rgblight_config_t inmem_config;
struct cRGB led[RGBLED_NUM+1];

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
    rgblight_config.mode = 8;
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

    if (!rgblight_config.mode || rgblight_config.mode == 0b11111) {  //flashing eep makes config.raw to 0xffffffff 
        dprintf("rgblight_init rgblight_config.mode = 0. Write default values to EEPROM.\n");
        eeconfig_write_rgblight_default();
        rgblight_config.raw = eeconfig_read_rgblight();
    }
    eeconfig_debug_rgblight(); // display current eeprom values

    rgblight_timer_init(); // setup the timer

    if (rgblight_config.enable) {
        rgblight_mode(rgblight_config.mode);
    } else {
        //rgblight_set();
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
  rgblight_sethsv(rgblight_config.hue, rgblight_config.sat, rgblight_config.val);
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
        //rgblight_set();
        //rgblight_timer_disable();
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
        sethsv(hue, sat, val, &led[RGBLED_NUM]);
        for (uint8_t i=0; i< RGBLED_NUM; i++) {
            led[i] = led[RGBLED_NUM];
        }
        //rgblight_set();
    }
}

void rgblight_sethsv(uint16_t hue, uint8_t sat, uint8_t val)
{
    if (rgblight_config.enable) {
        if (rgblight_config.mode == 1) {
            // same static color
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
    //rgblight_set();
}

void rgblight_clear(void)
{
    memset(&led[0], 0, 3 * RGBLED_NUM);
}

void rgblight_set(void)
{
    //if (!rgblight_config.enable) {
    //    rgblight_clear();
    //} 
    ws2812_setleds(led, RGBLED_NUM);
}

inline
void rgblight_task(void)
{
    if (rgblight_config.enable && rgblight_timer_enabled) {
        // Mode = 1, static light, do nothing here
        switch (rgblight_config.mode) {
            case 2 ... 5:
                rgblight_effect_breathing(rgblight_config.mode-1);
                break;
            case 6 ... 8:
                rgblight_effect_rainbow_mood(rgblight_config.mode-5);
                break;
            case 9 ... 14:
                rgblight_effect_rainbow_swirl(rgblight_config.mode-7);
                break;
            case 15 ... 20:
                rgblight_effect_snake(rgblight_config.mode-14);
                break;
            case 21 ... 23:
                rgblight_effect_knight(rgblight_config.mode-20);
                break;
        }
    }
}

// effects
void rgblight_effect_breathing(uint8_t interval)
{
    static uint8_t pos = 0;
    static int8_t increament = 1;
    rgblight_sethsv_noeeprom(rgblight_config.hue, rgblight_config.sat, pgm_read_byte(&RGBLED_BREATHING_TABLE[pos]));
    pos = pos + interval*increament;
    if (pos < interval || pos+interval > 126) {
        increament *= -1;
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
    uint8_t i;
    uint8_t interval2 = interval/2;
    for (i=0; i<RGBLED_NUM; i++) {
        hue = (768/16*i+current_hue)%768;
        sethsv(hue, rgblight_config.sat, rgblight_config.val, &led[i]);
    }
    //rgblight_set();
    if (interval % 2) {
        current_hue = (current_hue + interval2*16) % 768;
    } else {
        current_hue = (current_hue + 768 - interval2*16) % 768;
    }
}
void rgblight_effect_snake(uint8_t interval)
{
    static int8_t pos = 0 - RGBLIGHT_EFFECT_SNAKE_LENGTH;
    static uint8_t led_step = 0;
    uint8_t i;
    int8_t increament = 1;
    uint8_t interval2 = interval/2; // speed
    if (++led_step > interval2) {
        led_step = 0;
        rgblight_clear();
        if (interval%2) increament = -1;
        for (i=0; i<RGBLIGHT_EFFECT_SNAKE_LENGTH; i++) {
            sethsv((rgblight_config.hue+i*50)%768, rgblight_config.sat, rgblight_config.val, &led[(pos+i*increament+RGBLED_NUM)%RGBLED_NUM]);
        }
        pos += increament;
        if (pos > RGBLED_NUM) pos = 0;
        else if (pos < 0 ) pos = RGBLED_NUM;
        //rgblight_set();
    }
}

void rgblight_effect_knight(uint8_t interval)
{
    static int8_t pos = RGBLED_NUM - 1;
    static uint8_t sled_step = 0;
    static uint16_t current_hue=0;
    uint8_t i,j;
    static int8_t increament = 1;
    if (++sled_step > interval) {
        bool need_update = 0;
        sled_step = 0;
        rgblight_clear();
        for (i=0; i<RGBLIGHT_EFFECT_KNIGHT_LENGTH; i++) {
            if (pos+i < RGBLED_NUM && pos+i >= 0){
                need_update = 1;
                uint8_t tmp_col = pos+i;
                led[(tmp_col)%RGBLED_NUM] = led[RGBLED_NUM];
            }
        }
        if (need_update) rgblight_set(); //Keep the first or last col on when increament changes.
        pos += increament;
        if (pos <= 0 - RGBLIGHT_EFFECT_KNIGHT_LENGTH || pos >= RGBLED_NUM) {
            increament *= -1;
            current_hue = (current_hue + 40) % 768;
            sethsv(current_hue, rgblight_config.sat, rgblight_config.val, &led[RGBLED_NUM]);
        }
    }
}

#ifdef SUSPEND_ACTION
void suspend_power_down_action(void)
{
    softpwm_led_disable_all();
    rgblight_timer_disable();
    rgblight_setrgb(0,0,0);
}

void suspend_wakeup_init_action(void)
{
    rgblight_init();
}

#endif

void hook_keyboard_loop()
{
    static uint16_t rgb_update_timer = 0;
    if (rgblight_timer_enabled && timer_elapsed(rgb_update_timer) > 40) {
        rgb_update_timer = timer_read();
        rgblight_task();
    }
}
