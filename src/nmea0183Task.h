#pragma once
/*
 * nmea0183Task.h
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
#include <queue.h>

#include <NMEA0183.h>
#include <NMEA0183Msg.h>

/* A FreeRTOS task function */
/* pvParameters must point to a nmea0183Task::Params object */
extern "C" void nmea0183TaskEntry (void *pvParameters);

class nmea0183Task {
public:
	struct Params {
		msgHdlType    msgHdl;
		void* msgHdlArgs;
	};

	nmea0183Task(msgHdlType msgHdl, void* msgHdlArgs, unsigned long baud=4800);
	virtual ~nmea0183Task();

	void run();

private:
	tNMEA0183 nmea0183;
	HardwareSerial uart;
	QueueHandle_t sendQueue;
};


