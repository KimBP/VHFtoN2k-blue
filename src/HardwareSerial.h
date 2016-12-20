/*
 * HardwareSerial.h
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

#ifndef HARDWARESERIAL_H_
#define HARDWARESERIAL_H_

#include <Serial.h>
#include <serialDev.h>

class HardwareSerial : Serial {
public:
	HardwareSerial();
	virtual ~HardwareSerial();

	void begin(unsigned long baudrate);
	char read();
	void blockOnRead(bool doBlock) { block = doBlock; };
	void write(char ch);
	unsigned long available();
	unsigned long availableForWrite();

private:
	xComPortHandle hdl;
	bool block;
};

#endif /* HARDWARESERIAL_H_ */
