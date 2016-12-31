/*
 * nmea2000Task.cpp
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
 *  1301  USA */

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <nmea2000Task.h>
#include <N2kMsg.h>
#include <mcpNMEA2000.h>

nmea2000Task::nmea2000Task()
{

}

nmea2000Task::~nmea2000Task() {
}

void nmea2000Task::run() {
	mcpNMEA2000* obj = &mcpNMEA2000::getInstance();
	while(1) {
		obj->ParseMessages();
	}
}

extern "C" void nmea2000TaskEntry (void *pvParameters)
{
	(void) pvParameters;

	nmea2000Task hdl;

	// never returns
	hdl.run();

}

