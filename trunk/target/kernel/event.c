/***************************************************************************
 *  $Id: event.c 273 2011-03-03 21:03:19Z merdmann $
 *
 *  Thu Nov 29 20:55:57 2007
 *  Copyright  2007 Michael Erdmann
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

 /*
  * OSEK/VDX: This module contains the VDX event management interface.
  *
  * TODO: review the usage of critical sections
  *
  * Note:
  *   Please note in case of mon preemptive processes we are
  *   using the a spin lock to wait for specific events.
  */

#include "kernel.h"
#include "proc.h"
#include "event.h"

/* set an event flag */
StatusType SetEvent ( TaskType TaskID, EventMaskType Mask ) {
	/*
	 * OPTIMITIC APPROACH: If not suspended the event will be set and the
	 * process is forced to running. Either the process is busy with something
	 * else or it waits for the event. In any case the event flag is set.
	 * ONLY if the process is waiting it will be forced to running.
	 */
	_TCB[TaskID].event |= Mask;

	if(_TCB[TaskID].state == PROC_WAIT_EVENT )
		_TCB[TaskID].state = PROC_COMPUTING;
	_os_next_task = TaskID;

	UserMode(_machdep_yield());

	return E_OK;
}

/* clear event flags */
StatusType ClearEvent ( EventMaskType Mask ) {
	if( !IsUserMode() )
		return E_OS_CALLEVEL;

	_os_claim_tcb(_os_my_task_id );
	_TCB[_os_my_task_id].event &= ~Mask;
	_os_release_tcb(_os_my_task_id);

	return E_OK;
}

/* return the events stored with the given task */
StatusType GetEvent ( TaskType TaskID, EventMaskRefType Event ) {
	*Event =_TCB[TaskID].event;
	return E_OK;
}

/* wait for an event */
StatusType WaitEvent ( EventMaskType Mask ) {

	if(!IsUserMode())
		return E_OS_CALLEVEL;

	_os_claim_tcb( _os_my_task_id );

	_TCB[_os_my_task_id].state = PROC_WAIT_EVENT;
	_TCB[_os_my_task_id].mask = Mask;

	_os_release_tcb( _os_my_task_id );

	if( _TCB[_os_my_task_id].descriptor->schedule == NON)
		while ( !(_TCB[_os_my_task_id].event & Mask) );
	else
   	   _machdep_yield();

	return E_OK;
}
