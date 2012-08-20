/*
    ChibiOS/RT - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS-LCD-Driver.

    ChibiOS-LCD-Driver is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS-LCD-Driver is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef S6D1121_H
#define S6D1121_H

#include <stdint.h>

// I/O assignments
#define LCD_CS_GPIO			GPIO_PORTE_BASE
#define LCD_CS_PIN			0

#define LCD_RS_GPIO			GPIO_PORTE_BASE
#define LCD_RS_PIN			3

#define LCD_RST_GPIO		GPIO_PORTE_BASE
#define LCD_RST_PIN			4

#define LCD_RD_GPIO			GPIO_PORTE_BASE
#define LCD_RD_PIN			1

#define LCD_WR_GPIO			GPIO_PORTE_BASE
#define LCD_WR_PIN			2

#define LCD_DH_GPIO			GPIO_PORTA_BASE
#define LCD_DL_GPIO			GPIO_PORTB_BASE

#define LCD_DL_PERIPH		SYSCTL_PERIPH_GPIOB
#define LCD_IO_PERIPH		SYSCTL_PERIPH_GPIOE

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998



#define swap(type, i, j) {type t = i; i = j; j = t;}


void lld_lcdInit();
void lld_lcdSetCursor(uint16_t x, uint16_t y);
void lld_lcdSetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);


void lld_lcdClear();
void lld_fillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lld_setColor(uint8_t r, uint8_t g, uint8_t b);
void lld_setBackColor(uint8_t r, uint8_t g, uint8_t b);
void lld_lcdResetWindow();
void drawGBLCD(int x , int y , uint8_t lcd[160][144]);


#endif

