/***************************************************************************
 *  $Id: idle.c 265 2011-02-04 16:32:11Z merdmann $
 *
 *  Mon Nov 26 06:50:00 2007
 *  Copyright  2007 Michael Erdmann
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

/** Idle Task
 * This file contains the idle task. It is referenced from proc.c since
 * during the initialisation of the system, the idle task is started
 * unless the cooperative scheduling mode has been selected.
 *
 */
#include "typedefs.h"
#include "kernel.h"
#include "errno.h"
#include "wdt.h"

#include "idle.h"
#include "machdep.h"
/**
 * Since the idle task is the first task which executed it completes
 * the startup initialisation
 */
TASK(Idle_Task) {
    TASK_ARGUMENT myArgs __attribute__((unused)) = arg;
    long loop_count = 0;

    _machdep_initialize_wdt();

    /* idle and indicate idle activity */
    while(1) {
    	_machdep_clear_wdt();

    	if( ++loop_count % 400000  == 0 )
	    _machdep_trace(TRACE_ALIVE );
    }
}
