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

#define SLED_NUM 61
#define LED_3733_NUM_0 61
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

const pwm_control_register g_pwm_control_register[LED_3733_NUM_0] PROGMEM= //Light Reg address
{
	//ROW1 R, G, B FORMAT
    { 0x00, 0x20, 0x10 }, // RGB0  ESC
    { 0x01, 0x21, 0x11 }, // RGB1  1
    { 0x02, 0x22, 0x12 }, // RGB2  2
    { 0x03, 0x23, 0x13 }, // RGB3  3
    { 0x04, 0x24, 0x14 }, // RGB4  4
    { 0x05, 0x25, 0x15 }, // RGB5  5
    { 0x06, 0x26, 0x16 }, // RGB6  6
    { 0x07, 0x27, 0x17 }, // RGB7  7
    { 0x08, 0x28, 0x18 }, // RGB8  8 
    { 0x09, 0x29, 0x19 }, // RGB9  9
    { 0x0A, 0x2A, 0x1A }, // RGB10 0
    { 0x0B, 0x2B, 0x1B }, // RGB11 -
    { 0x0C, 0x2C, 0x1C }, // RGB12 =
    { 0x0D, 0x2D, 0x1D }, // RGB13 BACKSPACE
    //ROW2
    { 0x30, 0x50, 0x40 }, // RGB14 TAB
    { 0x31, 0x51, 0x41 }, // RGB15 Q
    { 0x32, 0x52, 0x42 }, // RGB16 W
    { 0x33, 0x53, 0x43 }, // RGB17 E
    { 0x34, 0x54, 0x44 }, // RGB18 R
    { 0x35, 0x55, 0x45 }, // RGB19 T
    { 0x36, 0x56, 0x46 }, // RGB20 Y
    { 0x37, 0x57, 0x47 }, // RGB21 U
    { 0x38, 0x58, 0x48 }, // RGB22 I
    { 0x39, 0x59, 0x49 }, // RGB23 O
    { 0x3A, 0x5A, 0x4A }, // RGB24 P
    { 0x3B, 0x5B, 0x4B }, // RGB25 [
    { 0x3C, 0x5C, 0x4C }, // RGB26 ]
    { 0x3D, 0x5D, 0x4D }, // RGB27 \
    //ROW3
    { 0x60, 0x80, 0x70 }, // RGB28 CAPSLOCK
    { 0x61, 0x81, 0x71 }, // RGB29 A
    { 0x62, 0x82, 0x72 }, // RGB30 S
    { 0x63, 0x83, 0x73 }, // RGB31 D
    { 0x64, 0x84, 0x74 }, // RGB32 F
    { 0x65, 0x85, 0x75 }, // RGB33 G
    { 0x66, 0x86, 0x76 }, // RGB34 H
    { 0x67, 0x87, 0x77 }, // RGB35 J
    { 0x68, 0x88, 0x78 }, // RGB36 K
    { 0x69, 0x89, 0x79 }, // RGB37 L
    { 0x6A, 0x8A, 0x7A }, // RGB38 ;
    { 0x6B, 0x8B, 0x7B }, // RGB39 '
    { 0x6C, 0x8C, 0x7C }, // RGB40 ENTER
    //ROW4
    { 0x90, 0xB0, 0xA0 }, // RGB41 LSHIFT
    { 0x91, 0xB1, 0xA1 }, // RGB42 Z
    { 0x92, 0xB2, 0xA2 }, // RGB43 X
    { 0x93, 0xB3, 0xA3 }, // RGB44 C
    { 0x94, 0xB4, 0xA4 }, // RGB45 V
    { 0x95, 0xB5, 0xA5 }, // RGB46 B
    { 0x96, 0xB6, 0xA6 }, // RGB47 N
    { 0x97, 0xB7, 0xA7 }, // RGB48 M
    { 0x98, 0xB8, 0xA8 }, // RGB49 ,
    { 0x99, 0xB9, 0xA9 }, // RGB50 .
    { 0x9A, 0xBA, 0xAA }, // RGB51 /
    { 0x9B, 0xBB, 0xAB }, // RGB52 RSHIFT
    //ROW5
    { 0x0F, 0x2F, 0x1F }, // RGB53 LCTRL
    { 0x3F, 0x5F, 0x4F }, // RGB54 LGUI
    { 0x6F, 0x8F, 0x7F }, // RGB55 LALT
    { 0x6E, 0x8E, 0x7E }, // RGB56 SPACE
    { 0x9F, 0xBF, 0xAF }, // RGB57 RALT
    { 0x9E, 0xBE, 0xAE }, // RGB58 FN
    { 0x9C, 0xBC, 0xAC }, // RGB59 APP
    { 0x9B, 0xBB, 0xAB }, // RGB60 RCTRL

	//EXTRA
	/*    
	{ 0x0E, 0x2E, 0x1E }, // RGB61 NULL
    { 0x3E, 0x5E, 0x4E }, // RGB62 NULL
    { 0x6D, 0x8D, 0x7D }, // RGB63 NULL
    */
    
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
    for (int i = 0x00; i <= 0x17; i++)
    {
        IS31FL3733_write_register(addr, i, 0b11111111);
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
