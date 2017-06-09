/*
 * mcpNMEA2000.cpp
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
#include <cstring>


#include <mcpNMEA2000.h>

extern "C" void canIsrTaskEntry(void *pvParameters);
extern "C" void EXTI9_5_IRQHandler(void);

/* Semaphore common between mcpNMEA2000 object and ISR routine */
static SemaphoreHandle_t canIsrSem;

const GPIO_TypeDef* mcpNMEA2000::spiCSport = static_cast<const GPIO_TypeDef*>(GPIOA);
const uint16_t mcpNMEA2000::spiCSpin = GPIO_Pin_11;

mcpNMEA2000& mcpNMEA2000::getInstance()
{
	static mcpNMEA2000 theOneAndOnly;

	return theOneAndOnly;
}

mcpNMEA2000::mcpNMEA2000()
: tNMEA2000(NULL),
  spi(SPI2),
  mcp(spi, spiCS),
  canOutQ(xQueueCreate(50, sizeof(struct canFrame))),
  canInQ(xQueueCreate(10, sizeof(struct canFrame)))
{

	canIsrSem = xSemaphoreCreateBinary();
    sendMutex = xSemaphoreCreateMutex();

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	/* Configure GPIO A8 as input for ISR*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	/* Configure  Chip Select for SPI */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = spiCSpin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( const_cast<GPIO_TypeDef*>(spiCSport), &GPIO_InitStructure );


	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);

	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_StructInit(&EXTI_InitStructure);
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_KERNEL_INTERRUPT_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

    xTaskCreate(canIsrTaskEntry, "canIsr", 400, (void* const) this, 6, NULL);


    // Initialize parent objects product information
    SetProductInformation("00000111", // Manufacturer's Model serial code
                                   111, // Manufacturer's product code
                                   "VHFtoN2k",  // Manufacturer's Model ID
                                   "1.0.0.1 (2016-10-17)",  // Manufacturer's Software version code
                                   "1.0.0.0 (2015-10-17)" // Manufacturer's Model version
                                   );
    // Device information
    SetDeviceInformation(111, // Unique number. Use e.g. Serial number.
                                  135, // Device function=NMEA 0183 Gateway. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20%26%20function%20codes%20v%202.00.pdf
                                  25, // Device class=Inter/Intranetwork Device. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20%26%20function%20codes%20v%202.00.pdf
                                  2046 // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                                 );

    SetN2kCANMsgBufSize(10);
    SetN2kCANSendFrameBufSize(0);
    SetMode(tNMEA2000::N2km_ListenAndNode,N2K_Def_DevId);
    EnableForward(false); // Disable all msg forwarding to USB (=Serial)
    Open();
}

mcpNMEA2000::~mcpNMEA2000() {
}

bool mcpNMEA2000::CANOpen()
{
	mcp.begin(CAN_250KBPS, MCP_8MHz);

	return true;
}

struct canStat {
	uint32_t sendCnt;
	uint32_t isrCnt;
	uint32_t semCnt;
	uint32_t txCnt;
	uint32_t rxCnt;
	uint32_t txEmpty;
	uint32_t isrEnable;
	uint32_t isrDisable;
	uint32_t lostFrame;
} canStat;

/* Realization of tNMEA2000 virtual function. Notice that this implementation ignores wait_sent flag.
 * It always wait for frame to be sent.
 * This is considered acceptable because it was called either once for short packages with 'no-wait'
 * or multiple times for long packages with 'wait'. There seems to be no reason why we can't also wait
 * on short packages.
 */
bool mcpNMEA2000::CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent)
{
	(void) wait_sent;
	struct canFrame frame;
	frame.id = id;
	frame.dlc = len;
	memcpy(frame.data, buf, len);

	if (pdPASS == xQueueSend(canOutQ, &frame, portMAX_DELAY)) {
		xSemaphoreGive(canIsrSem); // Signal to isrTask there is work to do
		canStat.sendCnt++;
		return true;
	}
	canStat.lostFrame++;

	return false;
}

/* Realization of tNMEA2000 virtual function. Notice that this implementation actually
 * blocks if no messages exists.
 * In current implementation it gets called from ParseMessages() which might never
 * return and thus never get a change to call SendFrames() and SendPendingInformation()
 * - but this implementation don't use the internal framebuffer and there will never
 * be any pending information as frames are always queued
 */
bool mcpNMEA2000::CANGetFrame(unsigned long &id, unsigned char &len, unsigned char *buf)
{
	struct canFrame frame;
	if (pdPASS == xQueueReceive(canInQ, &frame, portMAX_DELAY)) {
		id = frame.id;
		len = frame.dlc;
		for(int i=0; i < len; i++) {
			buf[i] = frame.data[i];
		}
		return true;
	}
	return false;
}


bool mcpNMEA2000::SendMsg(const tN2kMsg &N2kMsg, int DeviceIndex)
{
	bool res;

	if (dbMode == dm_None) {
		xSemaphoreTake(sendMutex, portMAX_DELAY);
		res = tNMEA2000::SendMsg(N2kMsg, DeviceIndex);
		xSemaphoreGive(sendMutex);
	} else {
		res = tNMEA2000::SendMsg(N2kMsg, DeviceIndex);
	}
	return res;
}

void mcpNMEA2000::isrTaskRun()
{
	struct canFrame frame;
	bool txEmpty = true;
	bool isExtId = true; // TODO: Figure out how to determine if it is extended

	while (1) {
		xSemaphoreTake(canIsrSem, portMAX_DELAY); // Wait for ISR

		canStat.semCnt++;
		// Check incoming
		unsigned char stat = mcp.mcp2515_readStatus();

		if (stat & MCP_STAT_RX0IF) {
			mcp.clearInterrupt(MCP_RX0IF);

			canStat.rxCnt++;
			mcp.readMsgUnconditional(rxBufNo); // Ignore result - we'll always have a packet
			mcp.clearInterrupt(MCP_RX0IF);
			frame.id = mcp.getCanId();
			mcp.getData(&frame.dlc, frame.data);

			xQueueSend(canInQ, &frame, 0); // We might loose it - that's life
		}

		if (stat & MCP_STAT_TX0IF) {
			mcp.clearInterrupt(MCP_TX0IF);
			if (pdTRUE == xQueueReceive(canOutQ, &frame, 0)) {
				// We have data to send
				mcp.sendMsgBufUnconditional(txBufNo,frame.id, isExtId, frame.dlc, frame.data);
				canStat.txCnt++;
			} else {
				// No data to send - disable TX-Empty interrupt
				mcp.disableInterrupt(MCP_TX0IF);
				// But remember we are empty
				txEmpty = true;
				canStat.isrDisable++;
			}
		} else if (txEmpty) {
			canStat.txEmpty++;

			// Better check if we've got data to send
			if (pdTRUE == xQueueReceive(canOutQ, &frame, 0)) {
				txEmpty = false;
				canStat.isrEnable++;

				mcp.enableInterrupt(MCP_TX0IF);
				mcp.sendMsgBufUnconditional(txBufNo, frame.id, isExtId, frame.dlc, frame.data);
			}
		}
	}
}

void mcpNMEA2000::spiCS(uint8_t val)
{
	GPIO_WriteBit(const_cast<GPIO_TypeDef*>(spiCSport), spiCSpin, (val) ? Bit_SET : Bit_RESET);
}

void EXTI9_5_IRQHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
		// Our interrupt
		EXTI_ClearITPendingBit(EXTI_Line8);
		canStat.isrCnt++;
		xSemaphoreGiveFromISR(canIsrSem, &xHigherPriorityTaskWoken);
	}
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}


void canIsrTaskEntry(void *pvParameters)
{
	mcpNMEA2000* obj = (mcpNMEA2000*)pvParameters;

	obj->isrTaskRun(); // Never returns
}


