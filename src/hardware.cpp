/*
 * hardware.cpp
 *
 *  Created on: Dec 11, 2016
 *      Author: kbp
 */

#include <stm32f10x_rcc.h>

// This comes from FreeRTOSv9.0.0/FreeRTOS/Demo/CORTEX_STM32F103_GCC_Rowley

void setupHardware() {
	/* RCC system reset(for debug purpose). */
	//RCC_DeInit ();

	/* Enable HSE. */
	//RCC_HSEConfig( RCC_HSE_ON );

	/* Wait till HSE is ready. */
	//while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

	/* HCLK = SYSCLK. */
	RCC_HCLKConfig( RCC_SYSCLK_Div1 );

	/* PCLK2  = HCLK. */
	//RCC_PCLK2Config( RCC_HCLK_Div1 );

	/* PCLK1  = HCLK/2. */
	//RCC_PCLK1Config( RCC_HCLK_Div2 );

	/* ADCCLK = PCLK2/4. */
	//RCC_ADCCLKConfig( RCC_PCLK2_Div4 );

	/* Flash 2 wait state. */
	//*( volatile unsigned long  * )0x40022000 = 0x01;

	/* PLLCLK = 8MHz * 9 = 72 MHz */
	//RCC_PLLConfig( RCC_PLLSource_HSE_Div1, RCC_PLLMul_9 );

	/* Enable PLL. */
	//RCC_PLLCmd( ENABLE );

	/* Wait till PLL is ready. */
	//while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

	/* Select PLL as system clock source. */
	//RCC_SYSCLKConfig (RCC_SYSCLKSource_PLLCLK);

	/* Wait till PLL is used as system clock source. */
	//while (RCC_GetSYSCLKSource() != 0x08);

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE and AFIO clocks */
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC
//													| RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE );

	// KBP new
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // Port B
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // Port C
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // Alternate functions

	/* Set the Vector Table base address at 0x08000000. */
	NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x0 );
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	/* Configure HCLK clock as SysTick clock source. */
	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );

	/* Initialise the IO used for the LED outputs. */
	//vParTestInitialise();

	/* SPI2 Periph clock enable */
	//RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE );

}


