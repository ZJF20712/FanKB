#ifndef IS31FL3733_H
#define IS31FL3733_H

#include <stdint.h>
#include <stdbool.h>
void IS31FL3733_init(uint8_t addr);
void IS31FL3733_write_register(uint8_t addr, uint8_t reg, uint8_t data);
void IS31FL3733_write_pwm_buffer(uint8_t addr, uint8_t *pwm_buffer);

void IS31FL3733_set_color(uint8_t index, uint8_t red, uint8_t green, uint8_t blue);
void IS31FL3733_set_color_all(uint8_t red, uint8_t green, uint8_t blue);
void IS31FL3733_update_pwm_buffers(uint8_t addr);
void IS31FL3733_update_pwm_buffers_all();

#endif 
