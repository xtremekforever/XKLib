/*
 * This file is part of the XKLib project.
 *
 * xktimer.c
 *
 * Copyright (C) 2011 Jesse L. Zamora <xtremekforever@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.

 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief The XKTimer Module (Source)
 *
 * Go to xktimer.h for full timer documentation.
 *
 * \author 				Jesse L. Zamora - xtremekforever@gmail.com
 *
 ******************************************************************************/

/* Standard Library Includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Time Includes */
#include <time.h>

#include "includes.h"

#include "xktimer.h"

/**
 * \brief		The array of timers.
 *
 * This array is used to store pointers to the various timers that have been
 * added using timer_add() or timer_add_dual().
 */
#ifdef XKTIMER_NO_MALLOC
xktimer_ptr_t	xktimer_ref[XKTIMER_MAX_TIMERS];
#else
xktimer_ptr_t * xktimer_ref;
#endif

//! The index of the last added timer + 1
int	 			xktimer_ref_idx;

void xktimer_init()
{
    xktimer_ref_idx = 0;
    
#ifndef XKTIMER_NO_MALLOC
    xktimer_ref = (xktimer_ptr_t *)malloc(1 * XKTIMER_PTR_SIZE);
#endif
    
#ifdef DEBUG
    printf("XKTimer Init\n");
    printf("Timer Resolution: %d\n", XKTIMER_RESOLUTION);
    printf("Clocks Per Sec: %ld\n", CLOCKS_PER_SEC);
#endif
}

bool xktimer_assert(xktimer_ptr_t timer)
{
	return timer != NULL;
}

bool xktimer_add_ptr(xktimer_t * timer)
{
#ifdef XKTIMER_NO_MALLOC
    // Now copy the pointer to the timer struct
    xktimer_ref[xktimer_ref_idx++] = timer;
#else
    if (xktimer_ref == NULL) {
        return false;
    } else {
        int ref_size = sizeof(xktimer_ref) / XKTIMER_PTR_SIZE;
        if (xktimer_ref_idx + 1 >= ref_size) {
            // Reallocate for more timers
            xktimer_ref = (xktimer_ptr_t *)realloc(xktimer_ref, (xktimer_ref_idx + 1) * 
															     XKTIMER_PTR_SIZE);
        }
    }

    // Allocate the memory for this timer pointer
    xktimer_ref[xktimer_ref_idx] = (xktimer_ptr_t)malloc(XKTIMER_PTR_SIZE);
    // Now copy the pointer to the timer struct
	// and increment to the next timer
    xktimer_ref[xktimer_ref_idx++] = timer;
#endif
    
    return true;
}

bool xktimer_add(xktimer_ptr_t timer,
			     uint8_t type,
			     uint32_t timeout,
			     void (*callback)())
{
	if (!xktimer_assert(timer) || xktimer_ref_idx >= XKTIMER_MAX_TIMERS) {
		return false;
	}

	timer->type = type;
	timer->enabled = false;
	timer->state = 0;
	timer->timeout = timeout;
	timer->timeout2 = 0;
	timer->callback = callback;

	xktimer_update_ticks(timer);
    xktimer_add_ptr(timer);
    
	return true;
}

bool xktimer_add_dual(xktimer_ptr_t timer,
				      uint32_t timeout,
				      uint32_t timeout2,
				      void (*callback)(int))
{
	if (!xktimer_assert(timer) || xktimer_ref_idx >= XKTIMER_MAX_TIMERS) {
		return false;
	}

	timer->type = XKTIMER_DUAL_STATE;
	timer->enabled = false;
	timer->state = 0;
	timer->timeout = timeout;
	timer->timeout2 = timeout2;
	timer->callback = callback;

	xktimer_update_ticks(timer);
    xktimer_add_ptr(timer);
    
	return true;
}

clock_t xktimer_clock()
{
	return clock() / (CLOCKS_PER_SEC / XKTIMER_RESOLUTION);
}

void xktimer_update_ticks(xktimer_ptr_t timer)
{
	if (!xktimer_assert(timer)) return;

	if (timer->state == 0) {
		timer->ticks = xktimer_clock() + timer->timeout;
	} else {
		timer->ticks = xktimer_clock() + timer->timeout2;
	}
}

uint32_t xktimer_timeout(xktimer_ptr_t timer)
{
	if (!xktimer_assert(timer)) return 0;
	
	return timer->timeout;
}

uint32_t xktimer_timeout2(xktimer_ptr_t timer)
{
	if (!xktimer_assert(timer)) return 0;
	
	return timer->timeout2;
}

clock_t xktimer_next_timeout(xktimer_ptr_t timer)
{
	if (!xktimer_assert(timer)) return 0;

	return timer->ticks - clock();
}

void xktimer_set_timeout(xktimer_ptr_t timer, 
						 uint32_t timeout)
{
	if (!xktimer_assert(timer)) return;

	// Set the timeout value
	timer->timeout = timeout;

	// Now update the timer ticks
	xktimer_update_ticks(timer);
}

void xktimer_set_timeout_dual(xktimer_ptr_t timer,
							  uint32_t timeout,
							  uint32_t timeout2)
{
	if (!xktimer_assert(timer)) return;

	timer->state = 0;

	// Set the timeouts
	timer->timeout = timeout;
	timer->timeout2 = timeout2;

	// Now update the timer ticks
	xktimer_update_ticks(timer);
}


void xktimer_start(xktimer_ptr_t timer)
{
	if (!xktimer_assert(timer)) return;

	timer->enabled = true;
	timer->state = 0;

	xktimer_update_ticks(timer);
}

void xktimer_stop(xktimer_ptr_t timer)
{
	if (!xktimer_assert(timer)) return;

	timer->enabled = false;
}

bool xktimer_running(xktimer_ptr_t timer)
{
	if (!xktimer_assert(timer)) return false;

	return timer->enabled;
}

void xktimer_wait(uint32_t ms)
{
	clock_t timestamp = xktimer_clock();

	while (xktimer_clock() < timestamp + ms) {
#ifdef timer_wait_task
		xktimer_wait_task();
#endif
	}
}

bool xktimer_periodic(clock_t * ticks, uint32_t period)
{
	if (xktimer_clock() >= *ticks + period) {
		*ticks = xktimer_clock();
		return true;
	}

	return false;
}

void xktimer_handle(xktimer_ptr_t timer)
{
	if (!xktimer_assert(timer) || !timer->enabled) return;

	if (xktimer_clock() >= timer->ticks) {

		switch (timer->type) {
		case XKTIMER_SINGLE_SHOT:
			timer->enabled = false;
			break;
		case XKTIMER_PERIODIC:
			break;
		case XKTIMER_DUAL_STATE:
			if (timer->state == 0) {
				timer->state = 1;
			} else {
				timer->state = 0;
			}
			break;
		}

		xktimer_update_ticks(timer);

		if (timer->callback) {
			timer->callback(timer->state);
		}
	}
}

void xktimer_task()
{
	int i;

	for (i = 0; i < xktimer_ref_idx; i++) {
		xktimer_handle(xktimer_ref[i]);
	}
}
