/*
 * Serial.cpp
 *
 *  Created on: Dec 20, 2016
 *      Author: kbp
 */

#include <Serial.h>
#include <string.h>
#include <cstdio>

Serial::Serial() {
	// TODO Auto-generated constructor stub

}

Serial::~Serial() {
	// TODO Auto-generated destructor stub
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
