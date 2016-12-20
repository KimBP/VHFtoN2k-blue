#pragma once
/*
 * Serial.h
 *
 *  Created on: Dec 20, 2016
 *      Author: kbp
 */


#include <stdint.h>

class Serial {
public:
	Serial();
	virtual ~Serial();
	enum SerialFormat {
		DECIMAL,
		HEX
	};
	void print(const char* str);
	void print(uint8_t val, SerialFormat format=DECIMAL);

	bool available() {return true; };
	char read();
	bool availableForWrite() {return true;};
	void write(char ch);
};

