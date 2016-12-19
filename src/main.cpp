//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <FreeRTOS.h>
#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include <portmacro.h>
#include <task.h>
#include <hardware.h>

#include <serial.h>

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

static unsigned char chConvCase(unsigned char ch)
{
	if ((ch >= 'a') && (ch <= 'z')) {
		ch -= ('a'-'A');
	} else if ((ch >= 'A') && (ch <= 'Z')) {
		ch += ('a'-'A');
	}
	return ch;
}

static void uartComTask (void *pvParameters)
{
	(void) pvParameters;

	xComPortHandle usart = xSerialPortInitMinimal(57600,100);

	vSerialPutString(usart,(const unsigned char*)"Hello world\n",12);
	while(1) {
		unsigned char rcv;
		if (pdTRUE == xSerialGetChar(usart, &rcv, 1000)) {
			rcv = chConvCase(rcv);
			xSerialPutChar(usart, rcv, 1000);
		}
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
	  {GPIOB, GPIO_Pin_15, 750U}
  };

  xTaskCreate(ledBlinkTask, "LED1", 400, &led[0], 10, NULL);
  xTaskCreate(ledBlinkTask, "LED2", 400, &led[1], 9, NULL);

  xTaskCreate(uartComTask, "UART", 400, NULL, 11, NULL);
  vTaskStartScheduler();

  while (1) { /* Keep running */ };
  return 0;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
