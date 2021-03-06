/***************************************************************************
 *  $Id: execution.h 261 2011-01-25 19:18:09Z merdmann $
 *
 *  Fri Dec  7 21:19:36 2007
 *  Copyright  2007  Michael Erdmann
 *  Email: michael.erdmann@snafu.de
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

/** @file
 *  This file contains basic hardware definitions as for example the clock frequency
 *  of the used hardware.
 */

#include "typedefs.h"


#ifndef _EXECUTION_H_
#define _EXECUTION_H_

/**
 * The user can call this system service to start the operating
 * system in a specific mode
 * Only allowed outside of the operating system, therefore
 * implementation specific restrictions may apply. See also
 * chapter 11.3, System start-up, especially with respect to
 * systems where OSEK and OSEKtime coexist. This call does not
 * need to return.
 */
void StartOS(AppModeType mode);

/**
 * The user can call this system service to abort the overall
 * system (e.g. emergency off). The operating system also calls
 * this function internally, if it has reached an undefined internal
 * state and is no longer ready to run.
 * If a ShutdownHook is configured the hook routine
 * ShutdownHook is always called (with <Error> as argument)
 * before shutting down the operating system.
 * If ShutdownHook returns, further behaviour of ShutdownOS is
 * implementation specific.
 * In case of a system where OSEK OS and OSEKtime OS
 * coexist, ShutdownHook has to return.
 * <Error> needs to be a valid error code supported by OSEK OS.
 * In case of a system where OSEK OS and OSEKtime OS
 * coexist, <Error> might also be a value accepted by OSEKtime
 * OS. In this case, if enabled by an OSEKtime configuration
 * parameter, OSEKtime OS will be shut down after OSEK OS
 * shutdown.
 */
void ShutdownOS( StatusType error);

/**
 * This service returns the current application mode. It may be
 * used to write mode dependent code.
 * See chapter 5 for a general description of application modes.
 * Allowed for task, ISR and all hook routines.
 */
AppModeType GetActiveApplicationMode ( void );

/*
 * This method is generated by the system generator and contains the
 * application mode specific init code.
 */
void _oil_init_application_mode(AppModeType mode);

/**
 * This hook routine is called by the operating system at the end of
 * the operating system initialization and before the scheduler is
 * running. At this time the application can initialize device drivers
 * etc.
 */
void StartupHook(void);

void ShutdownHook(void);

#endif
/* _EXECUTION_H_ */
