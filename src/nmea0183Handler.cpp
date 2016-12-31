/*
 * nmea0183Handler.cpp
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

#include <nmea0183Handler.h>
#include <AIS.h>
#include <N2kMsg.h>
#include <NMEA0183Messages.h>
#include <N2kMessages.h>
#include <mcpNMEA2000.h>

const struct nmea0183Handler::codeHdl handlers[3] = {
			{"VDM", nmea0183Handler::HandleVDM },
			{"DPT", nmea0183Handler::HandleDPT },
			{0, 0},
	};


nmea0183Handler::nmea0183Handler()
{
}

nmea0183Handler::~nmea0183Handler() {
}

void nmea0183HandlerCb(const tNMEA0183Msg &NMEA0183Msg, void* args)
{
	(void) args;
	// Find handler
	for (unsigned int i=0; ; i++)
	{
		if (handlers[i].hdl == 0) {
			return;
		}
		if (NMEA0183Msg.IsMessageCode(handlers[i].code)) {
			handlers[i].hdl(NMEA0183Msg);
		}
	}
}

uint8_t nmea0183Handler::nextSID()
{
	static uint16_t SIDval = 0;

	return static_cast<uint8_t>(++SIDval);
}

void nmea0183Handler::HandleVDM(const tNMEA0183Msg &NMEA0183Msg)
{
  uint8_t pkgCnt;
  uint8_t pkgNmb;
  unsigned int seqMessageId;
  char channel;
  unsigned int length;
  char buf[60];
  unsigned int fillBits;

  length = 40;

  if (NMEA0183ParseVDM_nc(
		NMEA0183Msg,
		pkgCnt,
		pkgNmb,
		seqMessageId,
		channel,
		length,
		buf,
		fillBits)) {

	// Multi packets not supported yet
	if (pkgCnt != 1) return;

	AIS ais_msg(buf, fillBits);

	// Check if supported message
	uint8_t msgType = ais_msg.get_type();
	if ( (msgType != 1) && (msgType != 2) && (msgType != 3)) return;


	// Forward message in N2K domain
	// TODO: Act correctly upon Channel - for now always class A


	tN2kMsg N2kMsg;

	SetN2kPGN129038(N2kMsg,
					seqMessageId,
					static_cast<tN2kAISRepeat>(ais_msg.get_repeat()),
					ais_msg.get_mmsi(),
					ais_msg.get_longitude(),
					ais_msg.get_latitude(),
					ais_msg.get_posAccuracy(),
					ais_msg.get_raim(),
					millis()/60,
					ais_msg.get_COG(),
					ais_msg.get_SOG(),
					ais_msg.get_HDG(),
					ais_msg.get_rot(),
					static_cast<tN2kAISNavStatus>(ais_msg.get_navStatus()));
	mcpNMEA2000::getInstance().SendMsg(N2kMsg);
  }
}

void nmea0183Handler::HandleDPT(const tNMEA0183Msg &NMEA0183Msg)
{
	double depth;
	double offset;
	double range;

	if (NMEA0183ParseDPT_nc(NMEA0183Msg, depth, offset, range)) {
		tN2kMsg N2kMsg;

		SetN2kPGN128267(N2kMsg,
						nextSID(),
						depth,
						offset);
		mcpNMEA2000::getInstance().SendMsg(N2kMsg);
	}
}
