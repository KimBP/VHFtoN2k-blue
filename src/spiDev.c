/*
 * spiDev.c
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


/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/* Library includes. */
#include <stm32f10x_spi.h>
#include <misc.h>

#include <spiDev.h>


/*-----------------------------------------------------------*/
static const unsigned char* txPtr;
static unsigned char* rxPtr;
static unsigned int   txrxCnt;
static SemaphoreHandle_t txrxSem;

/*-----------------------------------------------------------*/



/*-----------------------------------------------------------*/
int spiReadWrite(spiPortHandle spiHdl, unsigned char *rbuf, const unsigned char *tbuf, unsigned int cnt)
{
	configASSERT(SPI2 == spiHdl);

	txPtr = tbuf;
	rxPtr = rbuf;
	txrxCnt = cnt;

	SPI_I2S_ITConfig(spiHdl, SPI_I2S_IT_TXE, ENABLE);

	xSemaphoreTake( txrxSem, portMAX_DELAY);

	return 0;
}

/*-----------------------------------------------------------*/
spiPortHandle spiInit(SPI_TypeDef *SPIx)
{
	configASSERT(SPI2 == SPIx); // Hardcoded for now

	//Initialize semaphore
	txrxSem = xSemaphoreCreateBinary();

	/* Enable SPI2 clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE );

	/* Reset SPI interface */
	//SPI_I2S_DeInit(SPI2);

	/* Enable SPI pins */

	GPIO_InitTypeDef GPIO_InitStructure;
	/* Configure SPI2 CLK as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	/* Configure SPI2 MISO as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	/* Configure SPI2 MOSI as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	/* Configure SPI Mode */
	SPI_InitTypeDef SPI_InitStructure;
	SPI_StructInit(&SPI_InitStructure);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // This speed is traceable using a cheap Saleae clone
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPIx, &SPI_InitStructure);

	/* Configure interrupts */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_KERNEL_INTERRUPT_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );


	SPI_Cmd(SPIx, ENABLE);

	return SPIx;
}



/*-----------------------------------------------------------*/
/* SPI2 interrupt handler. */
unsigned long SPI_loop_cnt;

void SPI2_IRQHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE)) {
		if (0 != txrxCnt) { // Just guard - ought not happen
			txrxCnt--;
			if (txrxCnt == 0) {
			// This is last byte
			SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
			}
		}

		if (txPtr) {
			SPI_I2S_SendData(SPI2, *txPtr);
			txPtr++;
		} else {
			SPI_I2S_SendData(SPI2, 0xFFU);
		}

		/* Wait for input data to become ready - ought come immediately */
		while( SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE)) {SPI_loop_cnt++;};
		if (rxPtr) {
			*rxPtr = SPI_I2S_ReceiveData(SPI2);
			rxPtr++;
		}
		if (0 == txrxCnt) {
			xSemaphoreGiveFromISR(txrxSem, &xHigherPriorityTaskWoken);
		}
	}

	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}
