/*
 * FreeRTOSConfig.cpp
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
#include <task.h>

#ifdef __cplusplus
extern "C" {
#endif

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
        /* This function will get called if a task overflows its stack. */

        ( void ) pxTask;
        ( void ) pcTaskName;

        for( ;; );
}

void *operator new(size_t size)
{
	void *p;

	p=pvPortMalloc(size);

#ifdef	__EXCEPTIONS
	if (p==0) // did pvPortMalloc succeed?
		throw std::bad_alloc(); // ANSI/ISO compliant behavior
#endif

	return p;
}

void *operator new[](size_t size)
{
	void *p;

	p=pvPortMalloc(size);

#ifdef	__EXCEPTIONS
	if (p==0) // did pvPortMalloc succeed?
		throw std::bad_alloc(); // ANSI/ISO compliant behavior
#endif

	return p;
}

void operator delete(void* p)
{
	vPortFree(p);

	p = NULL;
}

void operator delete[](void* p)
{
	vPortFree(p);

	p = NULL;
}


#ifdef __cplusplus
}
#endif
