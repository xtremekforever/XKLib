/*
 * This file is part of the XKLib project.
 *
 * xktimer.h
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
 * \brief The XKTimer Module (Header)
 *
 * \section intro 			Introduction
 * The XKTimer module uses the global tc_ticks variable to implement a timer
 * which calls a timeout (or callback) function once the timer has timed out.
 * The callback is optional, though it is often used to provide a nice way to
 * handle the timer timeout. In order to create a new timer, an instance of
 * the xktimer_t struct must be defined and passed to the xktimer_add() or the
 * xktimer_add_dual() function. The xktimer_t struct contains information to 
 * keep track of the timer's elapsed time, running status, and a pointer to the
 * timer callback.
 *
 * Currently, there are 3 different types of timer available:
 *  - Single Shot Timer
 *  - Periodic Timer
 *  - Dual State Timer
 *
 * The XKTimer module also provides a timer_periodic() function that can be
 * used to create a simple periodic timer using an external ticks variable.
 * Please see xktimer_periodic() for information on how to use this.
 *
 *
 * \section internals		Internals
 * The XKTimer module is designed around the standard C clock() function. The
 * clock() function returns a ticks value indicating the number of ticks
 * since the program started. In order to convert the ticks variable to ms,
 * the XKTimer module divides the variable by the CLOCKS_PER_SEC constant.
 * The platform must provide clock() and CLOCKS_PER_SEC for this module to
 * function. Please see the section \ref embedded-support for more information
 * on how to support the XKTimer module on platforms not completely supporting
 * the standard C clock() function.
 *
 * After dividing the ticks value by CLOCKS_PER_SEC, the XKTimer module divides
 * again by 1000 to convert the value to milliseconds. The timer function
 * xktimer_clock() does this conversion and can be used by other modules to
 * keep track of time in ms independently and without using the other XKTimer
 * module functions.
 *
 * The XKTimer module uses the ticks returned by xktimer_clock() to keep track 
 * of the elapsed ms per timer. So, if a timer is started at a certain time, 
 * the XKTimer module gets the current ms ticks as returned by xktimer_clock() 
 * and adds the timeout value to the internal timer ticks variable for the 
 * given timer. Then, while looping through the list of registered timers, the 
 * timer  module then compares the current ticks with the ticks variable in the 
 * timer. If the current ms ticks value is greater than the timer's ticks value, 
 * the XKTimer module will call the timer's callback function.
 *
 *
 * \section timer-single 	Single-State Timers
 * The operation of so called "single-state" timers is simple. Basically, the
 * user creates an instance of the xktimer_t struct and passes it to the
 * xktimer_add() function along with the type of timer, the timeout for the 
 * timer, and a callback. When the timer is started using xktimer_start(), the
 * XKTimer module will begin to monitor the timer until it has timed out. Once
 * this occurs, the callback function (if provided) is run, and the timer is 
 * either stopped (for a single shot timer) or the timer is reset and continues
 * running (for periodic timers).
 *
 * The timeout value of single-state timers may be changed using the function
 * xktimer_set_timeout(). This will set the timeout for the provided timer and
 * reset the ticks value as well. The timer will continue to run as normal.
 *
 * Please read \ref timer-ex for a working example of a single-state timer in
 * action. The example also shows how to use the xktimer_task() function as 
 * well as the xktimer_set_timeout() function.
 *
 * If a simpler or smaller type of single-state periodic timer is desired, the
 * xktimer_periodic() function provides this functionality. To use this function,
 * all that needs to be provided is a pointer to a ticks variable defined as
 * clock_t and a time in ms. This function is reentrant and is designed to be 
 * polled until it returns true. Please see the section \ref timer-simple-ex 
 * for a working example of the xktimer_periodic() function.
 *
 *
 * \section timer-dual  Dual-State Timers
 * Dual-state timers are similar to periodic timers but have one distinction.
 * What sets them apart is that they provide two timeout states as opposed to
 * just one. For example, to initialize a dual-state timer, you pass a pointer
 * to the timer struct, two timeout values, and an optional callback. The timer
 * alternates between the two provided timeouts continuously until the timer is
 * stopped. 
 * 
 * A more detailed description of a dual-state is as follows. First, when the 
 * timer is started, it begins by first setting the timer's timeout value to
 * the first timeout. Upon timeout, the XKTimer module will call the callback
 * function and provide a single integer state parameter to show which timeout
 * was completed (0 for first timeout, 1 for second timeout). Then the timer
 * proceeds to run the second timeout value, and upon timeout, returns to the
 * first timeout value and state 0.
 *
 * Please see \ref timer-dual-ex for a working example of a dual-state timer.
 *
 * \section timer-ex 	Simple Timer Example
 * \code
 *
 * #include "xktimer.h"
 *
 * xktimer_t timer;
 *
 * void timer_single_shot()
 * {
 *     // Do stuff...
 *
 *     // You can also change the timeout:
 *     xktimer_set_timeout(&timer, 5000);
 *
 *     // We can also start the timer again
 *     xktimer_start(&timer);
 * }
 *
 * void main()
 * {
 *     // Create a new single shot timer that expires after 1 second
 *     xktimer_add(&timer, XKTIMER_SINGLE_SHOT, 1000, &timer_single_shot);
 *
 *     // Now we start the timer
 *     xktimer_start(&timer);
 *
 *     // Run the timer_task() function
 *     while (1) {
 *         xktimer_task();
 *     }
 * }
 *
 * \endcode
 *
 * This above code will run the timer_single_shot() function after 1 second
 * of program execution. Upon timeout, the timer_single_shot() function will
 * be called. Then, the timeout value is changed to 5 seconds and restarted.
 *
 * Here, we are only demonstrating a single shot timer. A periodic timer
 * functions the same way as a single shot timer, except that TIMER_PERIODIC
 * must be passed to the timer_add() function. A periodic timer will restart
 * the timer upon each timeout as opposed to a single shot timer.
 *
 *
 * \section timer-dual-ex	Dual-State Timer Example
 *
 * \code
 *
 * #include <stdio.h>
 *
 * #include "xktimer.h"
 *
 * xktimer_t timer;
 *
 * void timer_dual_state(int state)
 * {
 *     // Do some stuff...
 *
 *     printf("State: %d\n", state);
 *
 *     // To change the timeout:
 *     xktimer_set_timeout_dual(&timer, 1500, 900);
 * }
 *
 * void main()
 * {
 *     // Add a new dual state timer
 *     xktimer_add_dual(&timer, 700, 300, &timer_dual_state);
 *
 *     // Start the timer...
 *     xktimer_start(&timer);
 *
 *     // This is required so the XKTimer module can call
 *     // the timer's callback function
 *     while (1) {
 *         xktimer_task();
 *     }
 * }
 *
 * \endcode
 *
 * The dual-state timer differs from the other timers in that you must
 * specify 2 timeout values rather than simply 1. The dual-state timer
 * functions by using the first specified timeout value to expire, then
 * passes a state of 0 to the callback function. Then, it does the same
 * with the second timeout but passes a state of 1 to the callback function.
 * After this, it restarts itself and runs the first timeout.
 *
 * \note The xktimer_task() function must be run from the main program loop
 * 		 in order for this module to work, since this function is in charge
 * 		 of keeping track of the list of timers. So, if the user wants to
 * 		 temporarily "disable" the timers, this function can be kept from
 *       getting executed in order to do so.
 *
 * \todo Finish documenting the XKTimer module and also provide a version
 * 		 with upper case names and dynamic allocating of memory for timer
 *       pointers.
 *
 * - Compiler:      	GNU GCC on any platform with support for the standard
 * 						C/C++ libraries including clock() and CLOCKS_PER_SEC.
 *
 * \author 				Jesse L. Zamora - xtremekforever@gmail.com
 *
 ******************************************************************************/

#ifndef XKTIMER_H
#define XKTIMER_H

#include <time.h>

/**
 * \name Timer Types
 */
//! @{
/**
 * \brief		Single Shot Timer
 *
 * This timer simply runs for the specified time until it has timed
 * out, upon which it stops.
 */
#define XKTIMER_SINGLE_SHOT			1

/**
 * \brief		Periodic Timer
 *
 * This timer runs for the specified time and continues until the
 * user stops it.
 */
#define XKTIMER_PERIODIC			2

/**
 * \brief		Dual-State Timer
 *
 * This timer type has two timeout values and alternates between them
 * after each timeout. Also, this type of timer passes an int parameter
 * to the callback function with 0 or 1 based on which timeout value was
 * completed for that timer.
 */
#define XKTIMER_DUAL_STATE			3
//! @}

/**
 * \brief		The resolution of the XKTimer module
 *
 * This defines the resolution of the timers in the XKTimer module. Since
 * we use the global CLOCKS_PER_SEC variable to determine the number of
 * ticks in one second, we then use this variable to convert that value
 * into ms so we can provide full milli-second timer resolution.
 */
#define XKTIMER_RESOLUTION			1000

//! A convenient define to get the size of the xktimer_t struct
#define XKTIMER_SIZE				sizeof(xktimer_t)
//! A convenient define to get the size of the xktimer_ptr_t struct
#define XKTIMER_PTR_SIZE		sizeof(xktimer_ptr_t)

//! This is the maximum allowed timers
#define XKTIMER_MAX_TIMERS			30

/**
 * \brief 		The basic XKTimer timer structure.
 *
 * This timer structure allows other modules to create an instance of this
 * structure and pass it to the XKTimer module functions in order to imple-
 * ment simple timer functionality. This struct contains all the information
 * needed for a timer including the timeout and elapsed time, as well as a
 * pointer to a callback function (optional) and other information required
 * for a the complete timer implementation.
 *
 * The XKTimer module keeps an array of pointers to these timer structs that
 * have been added using timer_add() or timer_add_dual() by calling modules
 * in order to keep track of the timeouts and call the callback function.
 */
typedef struct {
	//! The timer type
	uint8_t type;

	//! If TRUE, the timer is running.
	bool enabled;

	//! The timer state. Can be 0 or 1 for dual state timers.
	uint8_t state;

	//! The first timeout value, used for all timer types
	uint32_t timeout;

	//! The second timeout value, used only for dual-state timers
	uint32_t timeout2;

	//! Holds the elapsed time
	clock_t ticks;

	//! Pointer to the callback function to call on timeout
	void (*callback)(int);
} xktimer_t;

//! Defines a pointer to an xktimer_t struct
typedef xktimer_t * xktimer_ptr_t;

/**
 * \brief			Initialize XKTimer module
 *
 * This initializes the XKTimer module by allocating the necessary resources
 * for adding timers. This must be called before using timer_add(); otherwise
 * the program will experience a segmentation fault.
 */
extern void xktimer_init();

/**
 * \brief			Add a new timer to the array
 *
 * Adds a new timer to the array using the specified id, type, timeout,
 * and callback.
 *
 * If the type parameter is anything other than single shot or periodic, it
 * will be set to single shot by default.
 *
 * \param timer		A pointer to the timer struct to add
 * \param type		The timer type to use.
 * \param timeout	The timeout in ms for this timer. This can be changed by
 * 					calling timer_set_timeout() with the ID after the timer has
 * 					been created.
 * \param callback	A pointer to the callback function to call upon timeout.
 * 					This can be NULL if the callback is not desired.
 *
 * \return true		Successfully added timer
 * \return false	Either the timer already existed or it exceeded the maximum
 * 					number of timers.
 */
extern bool xktimer_add(xktimer_ptr_t timer,
					    uint8_t type,
					    uint32_t timeout,
					    void (*callback)());

/**
 * \brief 			Add a new dual-state timer
 * 
 * Adds a new dual-state timer to the internal timer array and initializes it 
 * with the parameters provided to this function. 
 * 
 * The callback defines a single integer parameter that gives information of 
 * which timeout value was executed. Please see the \ref timer-dual section 
 * for more information on the callback function. The callback is optional and
 * can be substituted by NULL if no callback is desired.
 * 
 * 
 * 
 * \param timer		A pointer to the timer struct to add
 * \param timeout	The first timeout value for this timer
 * \param timeout2 	The second timeout value for this timer
 * \param callback	A pointer to the callback function to call upon timeout.
 * 
 * \return true		Successfully added timer
 * \return false	Either the timer already existed or it exceeded the maximum
 * 					number of timers.
 */
extern bool xktimer_add_dual(xktimer_ptr_t timer,
						     uint32_t timeout,
						     uint32_t timeout2,
						     void (*callback)(int));

extern clock_t xktimer_clock();

/**
 * \brief			Update the ticks for a given timer
 *
 * Updates the internal ticks variable for the given timer to timeout
 * based on the current value as returned by timer_clock() and the
 * internal timeout variable.
 *
 * For dual state timers, this will also update the ticks based on
 * the internal state variable using timeout2 when the state is equal
 * to 1.
 *
 * \param timer		A pointer to the timer to use to update
 * 					the internal ticks.
 */
extern void xktimer_update_ticks(xktimer_ptr_t timer);

extern uint32_t xktimer_timeout(xktimer_ptr_t timer);
extern uint32_t xktimer_timeout2(xktimer_ptr_t timer);

/**
 * \brief			Get the next timeout for the given timer
 *
 * This function calculates how many ms are left before the given timer
 * will time out.
 *
 * \param timer		A pointer to the timer to calculate the next
 * 					timeout from.
 *
 * \retval clock_t	The next timeout value in ms.
 */
extern clock_t xktimer_next_timeout(xktimer_ptr_t timer);

/**
 * \brief			Set the timeout for the given timer
 *
 * \param timer		A pointer to the timer to modify
 * \param timeout	The timeout value to set.
 */
extern void xktimer_set_timeout(xktimer_ptr_t timer,
							  uint32_t timeout);

/**
 * \brief			Set the timeout for a dual-state timer
 *
 * \param timer		A pointer to the timer to modify
 * \param timeout	The first timeout value
 * \param timeout2	The second timeout value
 */
extern void xktimer_set_timeout_dual(xktimer_ptr_t timer,
								   uint32_t timeout,
								   uint32_t timeout2);

/**
 * \brief			Starts the specified timer
 *
 * This routine resets the elapsed time and sets the enabled field to 1.
 *
 * \param timer		A pointer to the timer to start
 */
extern void xktimer_start(xktimer_ptr_t timer);

/**
 * \brief			Stops the specified timer
 *
 * This routine resets the elapsed time and sets the enabled field to 0.
 *
 * \param timer		A pointer to the timer to stop
 */
extern void xktimer_stop(xktimer_ptr_t timer);

/**
 * \brief			Returns the timer running state.
 *
 * \param timer		A pointer to the timer to get the running
 * 					status from
 *
 * \return true		The timer is running.
 * \return false	The timer is NOT currently running.
 */
extern bool xktimer_running(xktimer_ptr_t timer);

/**
 * \brief 			Delay for the specified ms.
 *
 * This function will basically block for the specified ms in order to produce
 * a time delay or "wait". This function can also run the xktimer_wait_task() 
 * function to perform tasks while the function is blocking. This can be useful
 * for clearing a watchdog timer or performing essential I/O tasks on embedded 
 * devices. 
 * 
 * In order to use the xktimer_wait_task() function, it must be defined as 
 * follows:
 * 
 * \code
 * #define xktimer_wait_task		task_function
 * \endcode
 * 
 * Replace task_function with the name of the function you want to execute, and
 * the XKTimer module will run it while waiting for a timeout.
 *
 * \param ms		A time in milli-seconds to block
 */
extern void xktimer_wait(uint32_t ms);

/**
 * \brief			A simple reentrant periodic timer function
 *
 * This function allows to create a simple periodic timer using just a ticks
 * variable and a time period. Essentially, the ticks variable is used to keep
 * track of the elapsed time for that timer, and the period is the periodic
 * timeout value. This function is designed to be run constantly in an context
 * using an if-statement to check if the periodic timer has timed out.
 *
 * \param ticks		A pointer to a ticks variable to keep track of the time
 * 					for this periodic timer. This is used instead of xktimer_t
 * 					since it is an extremely simple implementation.
 * \param period	A time in ms for the timer to timeout periodically. So if
 * 					the period is 1000, this function will return true every
 * 					second.
 *
 * \retval true		The timer has timed out
 * \retval false	The timer has not timed out yet
 */
extern bool xktimer_periodic(clock_t * ticks, uint32_t period);


/**
 * \brief			Handle a specific timer.
 *
 * This function is used to "handle" a specific timer by checking if the timer
 * has timed out as well as update the timeout as well as the state (if it is
 * a dual-state timer). Also, this will call the callback function on timeout
 * as well.
 *
 * This function can be used to handle a specific timer only instead of using
 * xktimer_task(). For example, instead of using xktimer_add() to create timers
 * and using xktimer_task() to monitor them, the xktimer_t struct can be 
 * initialized manually and this function can be used to monitor that timer. Or,
 * if we want to disable all other timers temporarily but one or two, we can use
 * this to keep the desired timers running.
 *
 * \param timer		A pointer to the timer to handle
 */
extern void xktimer_handle(xktimer_ptr_t timer);

/**
 * \brief			Handles the list of added timers
 *
 * This routine checks each timer which was added to the XKTimer module using
 * xktimer_add(). The xktimer_handle() function is used to check each timer 
 * one at a time in a simple loop through the list of pointers to all active 
 * timer structs.
 */
extern void xktimer_task();

#endif
