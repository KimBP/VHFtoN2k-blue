#pragma once
/*
 * nmea0183Handler.h
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

#include <NMEA0183Msg.h>
#include <VDMStack.h>

/* args must be a pointer to a FreeRTOS queue object to which
 * n2k objects allocated at the heap are pushed
 */
void nmea0183HandlerCb(const tNMEA0183Msg &NMEA0183Msg, void* args);


class nmea0183Handler {
public:
	static const struct codeHdl {
		const char* code;
		void (*hdl) (const tNMEA0183Msg &NMEA0183Msg);
	} handlers[];

	static void HandleVDM(const tNMEA0183Msg &NMEA0183Msg);
	static void HandleDPT(const tNMEA0183Msg &NMEA0183Msg);
	static double to_double(long inp, double precision) { return (inp*precision); };
	static uint16_t to_date(uint8_t month, uint8_t day);
	static double to_time(uint8_t hour, uint8_t minute);

public:
	nmea0183Handler();
	virtual ~nmea0183Handler();

private:
	static uint8_t nextSID();
	static VDMStack vdmStack; // Used for fragmented VDM messages

	static const uint16_t VDM_HDG_UNDEFINED = 511;
};


