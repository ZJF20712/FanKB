/* Copyright 2017 Jack Humbert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "is31fl3733.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "TWIlib.h"
#include "progmem.h"

#define SLED_NUM 47
#define LED_3733_NUM_0 47
#define ISSI_COMMAND_REG 0xFD
#define ISSI_COMMAND_REG_LOCK 0xFE
#define ISSI_COMMAND_REG_WRITE_ENABLE 0xC5
#define ISSI_CONTROL_PAGE 0x00 // LED Control Register
#define ISSI_PWM_PAGE 0x01 // PWM Register
// 0x00 - 0xBF set PWM duty for LED (w)

#define ISSI_ABM_PAGE 0x02 // Auto Breath Mode Register 

#define ISSI_FUNCTION_PAGE 0x03 // Function Register

uint8_t g_twi_transfer_buffer[TXMAXBUFLEN];
bool g_pwm_buffer_update_required = false;
uint8_t g_pwm_buffer[1][0xC0] = {0};

typedef struct
{
	uint8_t r_pwm_reg;
	uint8_t g_pwm_reg;
	uint8_t b_pwm_reg;
} pwm_control_register;

const pwm_control_register g_pwm_control_register[LED_3733_NUM_0] PROGMEM=
{
	{ 0x20, 0x00, 0x10 }, // RGB0 - RGB11
	{ 0x21, 0x01, 0x11 },
	{ 0x22, 0x02, 0x12 }, 
	{ 0x23, 0x03, 0x13 }, 
	{ 0x24, 0x04, 0x14 },
	{ 0x25, 0x05, 0x15 }, 
	{ 0x28, 0x08, 0x18 },
	{ 0x29, 0x09, 0x19 },
	{ 0x2A, 0x0A, 0x1A },
	{ 0x2B, 0x0B, 0x1B }, 
	{ 0x2C, 0x0C, 0x1C }, 
	{ 0x2D, 0x0D, 0x1D }, 

	{ 0xB0, 0x90, 0xA0 }, // RGB12 - RGB23
	{ 0xB1, 0x91, 0xA1 }, 
	{ 0xB2, 0x92, 0xA2 }, 
	{ 0xB3, 0x93, 0xA3 }, 
	{ 0xB4, 0x94, 0xA4 }, 
	{ 0xB5, 0x95, 0xA5 }, 
	{ 0xB8, 0x98, 0xA8 }, 
	{ 0xB9, 0x99, 0xA9 }, 
	{ 0xBA, 0x9A, 0xAA }, 
	{ 0xBB, 0x9B, 0xAB },
	{ 0xBC, 0x9C, 0xAC },
	{ 0xBD, 0x9D, 0xAD }, 

	{ 0x50, 0x30, 0x40 }, // RGB24 - RGB 35
	{ 0x51, 0x31, 0x41 },
	{ 0x52, 0x32, 0x42 },
	{ 0x53, 0x33, 0x43 }, 
	{ 0x54, 0x34, 0x44 }, 
	{ 0x55, 0x35, 0x45 }, 
	{ 0x58, 0x38, 0x48 }, 
	{ 0x59, 0x39, 0x49 }, 
	{ 0x5A, 0x3A, 0x4A }, 
	{ 0x5B, 0x3B, 0x4B }, 
	{ 0x5C, 0x3C, 0x4C }, 
	{ 0x5D, 0x3D, 0x4D }, 

	{ 0x80, 0x60, 0x70 }, // RGB36 - RGB 46
	{ 0x81, 0x61, 0x71 }, 
	{ 0x82, 0x62, 0x72 },
	{ 0x83, 0x63, 0x73 },
	{ 0x84, 0x64, 0x74 },
	{ 0x85, 0x65, 0x75 }, 
	{ 0x89, 0x69, 0x79 }, 
	{ 0x8A, 0x6A, 0x7A }, 
	{ 0x8B, 0x6B, 0x7B },
	{ 0x8C, 0x6C, 0x7C }, 
	{ 0x8D, 0x6D, 0x7D }, 

};

void IS31FL3733_write_register(uint8_t addr, uint8_t reg, uint8_t data)
{
    g_twi_transfer_buffer[0] = (addr << 1) | 0x00;
    g_twi_transfer_buffer[1] = reg;
    g_twi_transfer_buffer[2] = data;

    TWIInfo.errorCode = TWI_NO_RELEVANT_INFO;
    while (TWIInfo.errorCode != 0xFF) {
        TWITransmitData(g_twi_transfer_buffer, 3, 0);
    }
}

void IS31FL3733_write_pwm_buffer(uint8_t addr, uint8_t *pwm_buffer)
{
    IS31FL3733_select_page(addr, ISSI_PWM_PAGE);
    g_twi_transfer_buffer[0] = (addr << 1) | 0x00;
    for (uint8_t i = 0; i< 0xC0; i += 16) {
        g_twi_transfer_buffer[1] = i;
        for (uint8_t j = 0; j < 16; j++) {
            g_twi_transfer_buffer[j+2] = pwm_buffer[i+j];
        }
        TWIInfo.errorCode = TWI_NO_RELEVANT_INFO;
        while (TWIInfo.errorCode != 0xFF) {
            TWITransmitData(g_twi_transfer_buffer, 16+2, 0);
        }
    }
}

void IS31FL3733_select_page(uint8_t addr, uint8_t page)
{
    IS31FL3733_write_register(addr, ISSI_COMMAND_REG_LOCK, ISSI_COMMAND_REG_WRITE_ENABLE);
    IS31FL3733_write_register(addr, ISSI_COMMAND_REG, page);
}

void IS31FL3733_init(uint8_t addr)
{

    IS31FL3733_select_page(addr, ISSI_FUNCTION_PAGE);
    IS31FL3733_write_register(addr, 0x0, 0);
    IS31FL3733_write_register(addr, 0x0, 0b00000001);
    IS31FL3733_write_register(addr, 0x1, 192); 
    _delay_ms(10);
    IS31FL3733_select_page(addr, ISSI_CONTROL_PAGE);
    for (int i = 0x00; i <= 24; i++)
    {
        IS31FL3733_write_register(addr, i, 0b00111111);
    }
}

void IS31FL3733_set_color(uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    if (index  <= LED_3733_NUM_0) {
        g_pwm_buffer[0][ pgm_read_byte(&g_pwm_control_register[index].r_pwm_reg) ] = red;
        g_pwm_buffer[0][ pgm_read_byte(&g_pwm_control_register[index].g_pwm_reg) ] = green;
        g_pwm_buffer[0][ pgm_read_byte(&g_pwm_control_register[index].b_pwm_reg) ] = blue;
        g_pwm_buffer_update_required = true;
    }
}

void IS31FL3733_set_color_all(uint8_t red, uint8_t green, uint8_t blue)
{
    for (uint8_t i = 0; i < SLED_NUM; i++) {
        IS31FL3733_set_color( i, red, green, blue );
    }
}

inline
void IS31FL3733_update_pwm_buffers_all()
{
    if (g_pwm_buffer_update_required) {
        g_pwm_buffer_update_required = false;
        IS31FL3733_write_pwm_buffer(0x50, g_pwm_buffer[0]);
    }
}