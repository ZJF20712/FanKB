#include "IS31FL3731.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "TWIlib.h"
#include "progmem.h"

// This is a 7-bit address, that gets left-shifted and bit 0
// set to 0 for write, 1 for read (as per I2C protocol)
#define ISSI_ADDR_DEFAULT 0x74

#define ISSI_REG_CONFIG  0x00
#define ISSI_REG_CONFIG_PICTUREMODE 0x00
#define ISSI_REG_CONFIG_AUTOPLAYMODE 0x08
#define ISSI_REG_CONFIG_AUDIOPLAYMODE 0x18

#define ISSI_CONF_PICTUREMODE 0x00
#define ISSI_CONF_AUTOFRAMEMODE 0x04
#define ISSI_CONF_AUDIOMODE 0x08

#define ISSI_REG_PICTUREFRAME  0x01

#define ISSI_REG_SHUTDOWN 0x0A
#define ISSI_REG_AUDIOSYNC 0x06

#define ISSI_COMMANDREGISTER 0xFD
#define ISSI_BANK_FUNCTIONREG 0x0B    // helpfully called 'page nine'

// Transfer buffer for TWITransmitData()
uint8_t g_twi_transfer_buffer[TXMAXBUFLEN];

// These buffers match the IS31FL3731 PWM registers 0x24-0xB3.
// Storing them like this is optimal for I2C transfers to the registers.
// We could optimize this and take out the unused registers from these
// buffers and the transfers in IS31FL3731_write_pwm_buffer() but it's
// probably not worth the extra complexity.
uint8_t g_pwm_buffer[144];
bool g_pwm_buffer_update_required = false;

//uint8_t g_led_control_registers[2][18] = { { 0 }, { 0 } };
//bool g_led_control_registers_update_required = false;

// map of LED index to register (matrix A and matrix B)
// i.e. this is LA0..LA17,LB0..LB17 and also LC0..LC17,LD0..LD17
// Index of LED (0..36) will map to a register.
// Subtract 0x24 to get the second index of g_pwm_buffer
const uint8_t g_pwm_control_register[SLED_NUM] PROGMEM = {
    0xA5, 0x95, 0x85, 0x75, 0x65, 0x55, 0x45, 0x35, 0x25, 0xAD, 0x9D, 0x8D, 0x7D, 0x6D, 0x5D,
    0xA6, 0x96, 0x86, 0x76, 0x66, 0x56, 0x46, 0x36, 0x26, 0xAE, 0x9E, 0x8E, 0x7E, 0x6E, 0x5E,
    0xA7, 0x97, 0x87, 0x77, 0x67, 0x57, 0x47, 0x37, 0x27, 0xAF, 0x9F, 0x8F, 0x7F, 0x6F, 0x5F,
    0xA8, 0x98, 0x88, 0x78, 0x68, 0x58, 0x48, 0x38, 0x28, 0xB0, 0xA0,       0x80, 0x70, 0x60,
    0xA9, 0x99, 0x89, 0x79, 0x69,       0x49,       0x29, 0xB1, 0xA1,       0x81, 0x71, 0x61
};

const uint8_t s_leds_mask[0x12] PROGMEM= {
    0b00111110, 0b11111100,
    0b00011110, 0b11111100,
    0b00111110, 0b11111100,
    0b00011110, 0b00111110,
    0b00111110, 0b00111110,
    0b00111110, 0b00111110,
    0b00111110, 0b00001110,
    0b00111110, 0b00111110,
    0b00111110, 0b00111110
};
void IS31FL3731_write_register(uint8_t addr, uint8_t reg, uint8_t data)
{
    g_twi_transfer_buffer[0] = (addr << 1) | 0x00;
    g_twi_transfer_buffer[1] = reg;
    g_twi_transfer_buffer[2] = data;

    // Set the error code to have no relevant information
    TWIInfo.errorCode = TWI_NO_RELEVANT_INFO;
    // Continuously attempt to transmit data until a successful transmission occurs
    while (TWIInfo.errorCode != 0xFF)
    {
        TWITransmitData(g_twi_transfer_buffer, 3, 0);
    }
}

void IS31FL3731_write_pwm_buffer(uint8_t addr, uint8_t *pwm_buffer)
{
    // set the I2C address
    g_twi_transfer_buffer[0] = (addr << 1) | 0x00;
    // iterate over the pwm_buffer contents at 16 byte intervals
    for (uint8_t i = 0; i < 144; i += 16){
        // set the first register, e.g. 0x24, 0x34, 0x44, etc.
        g_twi_transfer_buffer[1] = 0x24 + i;
        // copy the data from i to i+15
        // device will auto-increment register for data after the first byte
        // thus this sets registers 0x24-0x33, 0x34-0x43, etc. in one transfer
        for (uint8_t j = 0; j < 16; j++) {
            g_twi_transfer_buffer[2 + j] = pwm_buffer[i + j];
        }
        // Set the error code to have no relevant information
        TWIInfo.errorCode = TWI_NO_RELEVANT_INFO;
        // Continuously attempt to transmit data until a successful transmission occurs
        while (TWIInfo.errorCode != 0xFF) {
            TWITransmitData(g_twi_transfer_buffer, 16 + 2, 0);
        }
    }
}

void IS31FL3731_turn_on_LED_control_register(uint8_t addr) {
    // select bank 0
    IS31FL3731_write_register(addr, ISSI_COMMANDREGISTER, 0);
    // turn on LEDs in the LED control register
    for (int i = 0x00; i <= 0x11; i++) {
        IS31FL3731_write_register(addr, i, pgm_read_byte(&s_leds_mask[i]));
    }
}

void IS31FL3731_init(uint8_t addr)
{
    // In order to avoid the LEDs being driven with garbage data
    // in the LED driver's PWM registers, first enable software shutdown,
    // then set up the mode and other settings, clear the PWM registers,
    // then disable software shutdown.
    // select "function register" bank
    IS31FL3731_write_register(addr, ISSI_COMMANDREGISTER, ISSI_BANK_FUNCTIONREG);

    // enable software shutdown
    IS31FL3731_write_register(addr, ISSI_REG_SHUTDOWN, 0x00);
    // this delay was copied from other drivers, might not be needed
    _delay_ms(10);

    // picture mode
    IS31FL3731_write_register(addr, ISSI_REG_CONFIG, ISSI_REG_CONFIG_PICTUREMODE);
    // display frame 0
    IS31FL3731_write_register(addr, ISSI_REG_PICTUREFRAME, 0x00);
    // audio sync off
    IS31FL3731_write_register(addr, ISSI_REG_AUDIOSYNC, 0x00);

    // select bank 0
    IS31FL3731_turn_on_LED_control_register(addr);

    // turn off all LEDs in the blink control register (not really needed)
    for (int i = 0x12; i <= 0x23; i++) {
        IS31FL3731_write_register(addr, i, 0);
    }

    // set PWM on all LEDs to 0
    for (int i = 0x24; i <= 0xB3; i++) {
        IS31FL3731_write_register(addr, i, 0);
    }

    // select "function register" bank
    IS31FL3731_write_register(addr, ISSI_COMMANDREGISTER, ISSI_BANK_FUNCTIONREG);

    // disable software shutdown
    IS31FL3731_write_register(addr, ISSI_REG_SHUTDOWN, 0x01);

    // select bank 0 and leave it selected.
    // most usage after initialization is just writing PWM buffers in bank 0
    // as there's not much point in double-buffering
    IS31FL3731_write_register(addr, ISSI_COMMANDREGISTER, 0);
    
    //other test
    #if 0
    IS31FL3731_write_register(0x74, 0xFD, 0x0B); //Point to Page Nine
    IS31FL3731_write_register(0x74, 0x08, 0x30); //Set fade in and fade out time for breath function
    IS31FL3731_write_register(0x74, 0x00, 0x3); //Configure the operation mode 
    IS31FL3731_write_register(0x74, 0xFD, 0x00); 
    #endif
}

void IS31FL3731_set_pwm(uint8_t index, uint8_t pwm_value)
{
    if (index >= 0 && index < SLED_NUM) {
        g_pwm_buffer[ pgm_read_byte(&g_pwm_control_register[index]) ] = pwm_value;
        g_pwm_buffer_update_required = true;
    }
}

void IS31FL3731_set_pwm_all(uint8_t pwm_value)
{
    for (uint8_t i=0; i< SLED_NUM; i++) {
        g_pwm_buffer[ pgm_read_byte(&g_pwm_control_register[i]) - 0x24 ] = pwm_value;
    }
    g_pwm_buffer_update_required = true;
}


void IS31FL3731_update_pwm_buffers(uint8_t addr)
{
    if (g_pwm_buffer_update_required) {
        g_pwm_buffer_update_required = false;
        IS31FL3731_write_pwm_buffer(addr, g_pwm_buffer);
    }
}

