/*
 * nmea0183Task.cpp
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

#include <nmea0183Task.h>
#include <NMEA0183.h>

nmea0183Task::nmea0183Task(msgHdlType msgHdl, void* msgHdlArgs, unsigned long baud)
{
	nmea0183.Begin(&uart, 0, baud, true);
	nmea0183.SetMsgHandler(msgHdl, msgHdlArgs);
}

nmea0183Task::~nmea0183Task()
{
}

void nmea0183Task::run()
{
	while (1) {
		nmea0183.ParseMessages();
	}
}




extern "C" void nmea0183TaskEntry (void *pvParameters)
{
	struct nmea0183Task::Params* params = (struct nmea0183Task::Params*)(pvParameters);

	nmea0183Task hdl(params->msgHdl, params->msgHdlArgs, 4800);

	// never returns
	hdl.run();

}

