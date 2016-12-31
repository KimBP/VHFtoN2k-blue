/*
 * Serial.cpp
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

#include <Serial.h>
#include <string.h>
#include <cstdio>

Serial::Serial() {
}

Serial::~Serial() {
}

void Serial::print(const char* str)
{
	while (*str != '\0') {
		while (!availableForWrite()) {};
		write(*str);
		str++;
	}
}

void Serial::print(uint8_t val, Serial::SerialFormat format)
{
	char str[4];
	if (format == DECIMAL) {
		sprintf(str, "%d", val);
	} else {
		sprintf(str, "%x", val);
	}
	print(str);
}

char Serial::read()
{
	return getchar();
}
void Serial::write(char ch)
{
	putchar(ch);
}
