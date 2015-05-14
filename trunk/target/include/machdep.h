/****************************************************************************
 *  $Id: proc.c 273 2011-03-03 21:03:19Z merdmann $
 *
 *  Mon Nov 26 06:50:00 2007
 *  Copyright  2007-2011 Michael Erdmann
 *  Email:Michael.Erdmann@snafu.de
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/** Process Manager
 * This is the process manager of mkernel.
 * The scheduler is called either by the tick handler which is called by
 * a low priority interrupt for by the YIELD method.
 * Additionally to the scheduler the module contains also the task API.
 *
 */
#ifndef _MACHDEP_H_
#define _MACHDEP_H_

#include "typedefs.h"
#include "proc.h"

DATA POINTER _machdep_initialize_stack(DATA POINTER, DATA POINTER, TASK_ARGUMENT); 
void _machdep_initialize_timer(void);
void _machdep_yield(void);


/** Enter a critical section in the code. All interrupts are disabled till the
 *  EXIT_CRITICAL has been executed. Both functions should be used with
 *  exterem care.
 */
void _machdep_critical_begin(void);

/** Exit an critical section.Since the INTCON is stored on the stack, this function
 *  can only be used together with ENTER_CRITICAL.
 */
void _machdep_critical_end(void);
void _machdep_boot(void);
void _machdep_initialize_wdt(void);
void _machdep_clear_wdt(void);


#define TRACE_ALIVE		1
#define TRACE_PANIC_UNDERFLOW	2

void _machdep_trace(unsigned);

inline BOOL _machdep_cas_byte(volatile void *addr, dword expected, dword store);

// context handling
void _machdep_restore_context(void);
void _machdep_save_context(void);

#endif