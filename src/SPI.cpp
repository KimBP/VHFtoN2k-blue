/*
 * SPI.cpp
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

#include <SPI.h>

SPIClass::SPIClass(SPI_TypeDef* SPIx) {
	spiHdl = spiInit(SPIx);
}

SPIClass::~SPIClass() {
}

void SPIClass::begin()
{

}

unsigned char SPIClass::transfer(unsigned char val)
{
	unsigned char rx;

	spiReadWrite(spiHdl, &rx, &val, 1);

	return rx;
}

void SPIClass::transfer(unsigned char* tx, unsigned char* rx, uint8_t len)
{
	spiReadWrite(spiHdl, rx, tx, len);
}
