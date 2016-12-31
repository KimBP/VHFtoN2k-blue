/*
 * SPI.h
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
#include <spiDev.h>

#ifndef SPI_H_
#define SPI_H_

class SPIClass {
public:
	SPIClass(SPI_TypeDef* SPIx);
	virtual ~SPIClass();

	void begin();
	void transfer(unsigned char* tx, unsigned char* rx, uint8_t len);
	void transfer(unsigned char* txrx, uint8_t len) { transfer(txrx, txrx, len); };
	unsigned char transfer(unsigned char val);
	unsigned char transfer(char val) { return transfer( static_cast<unsigned char>(val)); };
	unsigned char transfer(int val)  { return transfer( static_cast<unsigned char>(val)); };

private:
	spiPortHandle spiHdl;

};

#endif /* SPI_H_ */
