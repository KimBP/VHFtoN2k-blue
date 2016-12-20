/*
 * HardwareSerial.cpp
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

#include "HardwareSerial.h"

HardwareSerial::HardwareSerial()
	:hdl(0)
{
}

HardwareSerial::~HardwareSerial() {
	if (0 != hdl) {
		vSerialClose(hdl);
	}
}


void HardwareSerial::begin(unsigned long baudrate)
{
	if (0 != hdl) {
		return;
	}
	hdl = xSerialPortInitMinimal(baudrate, 40);
}

unsigned long HardwareSerial::available()
{
	return xSerialGetCharReady(hdl);
}

char HardwareSerial::read()
{
	unsigned char ch;
	if (pdTRUE == xSerialGetChar(hdl, &ch, (block ? portMAX_DELAY : 0))) {
		return (char) ch;
	} else {
		return '\0';
	}
}

unsigned long HardwareSerial::availableForWrite()
{
	return xSerialPutCharReady(hdl);
}

void HardwareSerial::write(char ch)
{
	xSerialPutChar(hdl, (unsigned char)ch, 0);
}
