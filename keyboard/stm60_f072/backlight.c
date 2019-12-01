#include "ch.h"
#include "hal.h"

#include "backlight.h"
#include "action.h"
#include "rgblight.h"

const uint8_t BREATHING_TABLE[128] = {0,0,0,0,1,1,1,2,2,3,4,5,5,6,7,9,10,11,12,14,15,17,18,20,21,23,25,27,29,31,33,35,37,40,42,44,47,49,52,54,57,59,62,65,67,70,73,76,79,82,85,88,90,93,97,100,103,106,109,112,115,118,121,124,127,131,134,137,140,143,146,149,152,155,158,162,165,167,170,173,176,179,182,185,188,190,193,196,198,201,203,206,208,211,213,215,218,220,222,224,226,228,230,232,234,235,237,238,240,241,243,244,245,246,248,249,250,250,251,252,253,253,254,254,254,255,255,255};

static const uint16_t backlight_table[] = {
    10000, 9000, 5000, 0
};

uint8_t backlight_config_level = 0;
uint8_t backlight_on = 0;

static void backlight_effect_breathing(uint8_t cc);

static void backlight_task(void)
{
  static step = 0;
  if (++step > 30) {
    step = 0;
    rgblight_task();
  }
  if (backlight_config_level < 4) return;
  if (backlight_config_level < 7) backlight_effect_breathing(8-backlight_config_level);
}

static void backlight_effect_breathing(uint8_t cc)
{
  static uint16_t step = 0;
  static int8_t inc = 1;
  static uint8_t bright = 3;
  if (++step > cc*2) {
    step = 0;
    uint16_t pwm_value = BREATHING_TABLE[bright]*40;
    pwm_value = (pwm_value > 10000)? 10000:pwm_value;
    pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, pwm_value));
    bright += inc;
    if (bright == 127 || bright == 0) inc *= -1;
  }
}

static PWMConfig pwmcfg = {
  10000,                                    /* 10kHz PWM clock frequency.   */
  20,                                    /* Original: 10000, Initial PWM period 1S.       */
  backlight_task,                                   /* callback */
  {
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},                /* mode, callback */
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_DISABLED, NULL},
    {PWM_OUTPUT_DISABLED, NULL}
  },
  0,
  0,
};

void backlight_init(void)
{
  pwmStart(&PWMD1, &pwmcfg);
  pwmEnablePeriodicNotification(&PWMD1);
  palSetPadMode(GPIOA, 8, PAL_MODE_ALTERNATE(2));
  pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 10000));
  //default on and level=4
  backlight_config_level = 4;
  backlight_on = 1;
  backlight_level(4);
}

void backlight_enable(void)
{

}

void backlight_disable(void)
{

}

void backlight_toggle(void)
{
  backlight_on ^= 1;
  if (backlight_on) backlight_level(0);
  else backlight_level(backlight_config_level? backlight_config_level:1);
}

void backlight_step(void)
{
  if (backlight_config_level == 6) backlight_config_level = 1;
  else backlight_config_level++;
  backlight_level(backlight_config_level);
}

void backlight_increase(void)
{
  if (backlight_config_level < 6) backlight_config_level++;
  backlight_level(backlight_config_level);
}

void backlight_decrease(void)
{
  if (backlight_config_level > 0) backlight_config_level--;
  backlight_level(backlight_config_level);
}

void backlight_level(uint8_t level)
{
  switch (level) {
    case 1 ... 3:
      backlight_set_raw(backlight_table[level]);
      break;
    case 4 ... 6:
      break;
    default:
      backlight_set_raw(backlight_table[0]);
      break;      
  }
}

inline void backlight_set_raw(uint16_t raw)
{
    pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, raw));
}
