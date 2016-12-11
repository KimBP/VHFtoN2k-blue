/*
 * FreeRTOSConfig.cpp
 *
 *  Created on: Dec 11, 2016
 *      Author: kbp
 */

#include <FreeRTOS.h>
#include <task.h>

extern "C" {
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
        /* This function will get called if a task overflows its stack. */

        ( void ) pxTask;
        ( void ) pcTaskName;

        for( ;; );
}
}
