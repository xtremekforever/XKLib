/*
 * This file is part of the XKLib project.
 *
 * XKTimer+.h
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

#ifndef XKTIMERPLUS_H
#define XKTIMERPLUS_H

#include "includes.h"

#include "xktimer.h"

typedef xktimer_t   			XKTimer;
typedef xktimer_ptr_t  			XKTimerPtr;

#define XKTimer_Init          	xktimer_init
#define XKTimer_Add           	xktimer_add
#define XKTimer_AddDual       	xktimer_add_dual
#define XKTimer_Timeout			xktimer_timeout
#define XKTimer_Timeout2		xktimer_timeout2
#define XKTimer_NextTimeout   	xktimer_next_timeout
#define XKTimer_SetTimeout    	xktimer_set_timeout
#define XKTimer_SetTimeoutDual 	xktimer_set_timeout_dual
#define XKTimer_Start         	xktimer_start
#define XKTimer_Stop          	xktimer_stop
#define XKTimer_Running       	xktimer_running
#define XKTimer_Wait          	xktimer_wait
#define XKTimer_Periodic      	xktimer_periodic
#define XKTimer_Handle        	xktimer_handle
#define XKTimer_Task          	xktimer_task

#endif
