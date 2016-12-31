/*
 * main.cpp
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
// ----------------------------------------------------------------------------

#include <FreeRTOS.h>
#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include <portmacro.h>
#include <task.h>
#include <queue.h>
#include <hardware.h>
#include <nmea0183Task.h>
#include <serialDev.h>
#include <nmea0183Handler.h>
#include <nmea2000Task.h>
#include <N2kMsg.h>

// ----------------------------------------------------------------------------
//
// Semihosting STM32F1 empty sample (trace via DEBUG).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

struct ledCfg {
	GPIO_TypeDef* port;
	uint16_t      pin;
	uint32_t      mstime;
};

static void ledBlinkTask( void *pvParameters )
{
	// Configure Pins
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	struct ledCfg* cfg = (struct ledCfg*)pvParameters;

	GPIO_InitStructure.GPIO_Pin = cfg->pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(cfg->port, &GPIO_InitStructure);

    int ledval = 1;

	while (1) {
	  GPIO_WriteBit(cfg->port, cfg->pin, (ledval) ? Bit_SET : Bit_RESET);
	  ledval = 1 - ledval;

	  vTaskDelay(cfg->mstime / portTICK_PERIOD_MS);

	}
}


// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int
main(int argc, char* argv[])
{
  setupHardware();

  // At this stage the system clock should have already been configured
  // at high speed.

  static struct ledCfg led[2] = {
	  {GPIOC, GPIO_Pin_13, 500U},
  };

  xTaskCreate(ledBlinkTask, "LED1", 400, &led[0], 10, NULL);

  struct nmea0183Task::Params params0183 = {
		  nmea0183HandlerCb,
		  NULL
  };

  xTaskCreate(nmea2000TaskEntry, "Nmea2000", 400, NULL, 10, NULL);
  xTaskCreate(nmea0183TaskEntry, "Nmea0183", 400, (void* const) &params0183, 11, NULL);

  vTaskStartScheduler();

  while (1) { /* Keep running */ };
  return 0;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
