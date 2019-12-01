#ifndef IS31FL3731_DRIVER_H
#define IS31FL3731_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

void IS31FL3731_turn_on_LED_control_register(uint8_t addr);
void IS31FL3731_init(uint8_t addr);
void IS31FL3731_write_register(uint8_t addr, uint8_t reg, uint8_t data);

void IS31FL3731_set_pwm(uint8_t index, uint8_t pwm_value);
void IS31FL3731_set_pwm_all(uint8_t pwm_value);

void IS31FL3731_write_pwm_buffer(uint8_t addr, uint8_t *pwm_buffer);


// This should not be called from an interrupt
// (eg. from a timer interrupt).
// Call this while idle (in between matrix scans).
// If the buffer is dirty, it will update the driver with the buffer.
void IS31FL3731_update_pwm_buffers(uint8_t addr);

#endif // IS31FL3731_DRIVER_H
