#pragma once
/*
 * arduino.h
 *
 * 2016 Copyright (c) Kim Bøndergaard, www.fam-boendergaard.dk  All right reserved.
 *
 * Author: Kim Bøndergaard (kim@fam-boendergaard.dk)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This module is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this module; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-
 *  1301  USA

 */

#include <FreeRTOS.h>
#include <SPI.h>
#include <task.h>

#include <HardwareSerial.h>


/* Binary macros - who can't read hex!!! */
#define B00000000		(0x00U)
#define B00000001		(0x01U)
#define B00000010		(0x02U)
#define B00000100		(0x04U)
#define B00001000		(0x08U)
#define B00010000		(0x10U)
#define B00100000		(0x20U)
#define B01000000		(0x40U)
#define B10000000		(0x80U)


#define lowByte(x)		((unsigned char)(x))
#define highByte(x)		((unsigned char)((x) >> 8))

#define LOW				(0U)
#define HIGH			(1U)

#define delay(x)
#define delayMicroseconds(x)

#define pinMode(pin, mode)

typedef unsigned char	byte;

/* Flash initializer - not used */
#define PROGMEM

/* References to data in flash - not relevant for now */
inline const char* F(const char* str) {return str; };

#define pgm_read_byte(addr)		(*(const unsigned byte*)(addr))
#define pgm_read_word(addr)		(*(const unsigned short*)(addr))
#define pgm_read_dword(addr)	(*(const unsigned int*)(addr))



inline unsigned long millis() { return xTaskGetTickCount() * portTICK_PERIOD_MS; };

