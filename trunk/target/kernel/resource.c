/***************************************************************************
 *  $Id: resource.c 273 2011-03-03 21:03:19Z merdmann $
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

/** OSEK/VDX Handle Resources
 *  This file contains the code to handle resources according to the
 *  OSEK/VDX standard.
 *
 *
 */
#include "typedefs.h"
#include "kernel.h"
#include "resource.h"
#include "errno.h"
#include "proc.h"
#include "machdep.h"

/** GetResource get a critical section
 *  This call serves to enter critical sections in the code that are
 *  assigned to the resource referenced by <ResID>. A critical
 *  section shall always be left using ReleaseResource.
 *
 *  @param ResID - Reference to resource
 */
StatusType GetResource( ResourceType ResID ) {
	StatusType RC = E_OK;

	while( !_machdep_cas_byte( (POINTER)&ResID->owner, NO_OWNER, _os_my_task_id ) ) {
		/*
		 * In case of an non  sheduled process we wait in a spin lock for the
		 * resource.
		 * TODO: if the resource is not owned by an interrupt handler
		 * there will be no chance to get the resource. This should raise an
		 * error.
		 */
		if( _TCB[_os_my_task_id].descriptor->schedule != NON) {
			_os_claim_tcb( _os_my_task_id );

			_TCB[_os_my_task_id].wait_addr = (DATA POINTER)ResID;
			_TCB[_os_my_task_id].state = PROC_WAIT_RESOURCE;
			_os_next_task = _os_my_task_id;

			_os_release_tcb( _os_my_task_id );
			_machdep_yield();
			break;
		}
	}

	return RC;
}

/** ReleaseResource makes a resource avaiable
 * ReleaseResource is the counterpart of GetResource and
 * serves to leave critical sections in the code that are assigned to
 * the resource referenced by <ResID>.
 *
 * @param ResID - Reference to the resource,
 */
StatusType ReleaseResource( ResourceType ResID ) {
	StatusType rc;

	rc = _machdep_cas_byte( (POINTER) &ResID->owner, _os_my_task_id, NO_OWNER) ? E_OK : E_OS_ID;

	if( _TCB[_os_my_task_id].descriptor->schedule != NON)
		_machdep_yield();

	return rc;
}
