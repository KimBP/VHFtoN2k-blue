/*
 * mcpNMEA2000.h
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

#ifndef MCPNMEA2000_H_
#define MCPNMEA2000_H_

#include <FreeRTOS.h>
#include <portmacro.h>

#include <NMEA2000.h>
#include <mcp_can.h>
#include <SPI.h>
#include "semphr.h"
#include <queue.h>

class mcpNMEA2000: public tNMEA2000 {
public:
	struct canFrame {
		unsigned long id;
		uint8_t dlc;
		uint8_t data[8];
	};

public:
	static mcpNMEA2000& getInstance();

	virtual ~mcpNMEA2000();

    virtual bool CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent=true);
    virtual bool CANOpen();
    virtual bool CANGetFrame(unsigned long &id, unsigned char &len, unsigned char *buf);

    void isrTaskRun(); // Handles deferred interrupts from MCP2515
    bool SendMsg(const tN2kMsg &N2kMsg, int DeviceIndex=0); // Override super class variant

private:
	mcpNMEA2000();

	// Ensure no copies of the one and only is made
	mcpNMEA2000(mcpNMEA2000 const&) = delete;
	void operator=(mcpNMEA2000 const&) = delete;

	// SPI Chipselect handling
    static void spiCS(uint8_t val);
    static const GPIO_TypeDef* spiCSport;
    static const uint16_t spiCSpin;

private:
    SPIClass spi; // The SPI driver to use when communicating with MCP2515
    MCP_CAN mcp; // The driver for MCP2515

    QueueHandle_t canOutQ;
    QueueHandle_t canInQ;
    static const uint16_t rxBufNo = 0; // Always only use first RX buf in MCP2515
    static const uint16_t txBufNo = 0; // Always only use first TX buf in MCP2515
    static const unsigned long N2K_Def_DevId = 25;

    SemaphoreHandle_t sendMutex;
};

#endif /* MCPNMEA2000_H_ */
