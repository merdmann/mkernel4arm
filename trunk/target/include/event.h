/***************************************************************************
 *  $Id: event.h 219 2008-10-12 17:23:57Z merdmann $
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

 /** @file
  * This file contains the interfaces for the OSEK/VDX compatible event
  * handling.
  */
#ifndef _EVENT_H
#define _EVENT_H

#include "typedefs.h"
#include "errno.h"

#define DeclareEvent(x) extern const EventMaskType x

/** The EventMaskType represents events, for each one bit. */
typedef unsigned EventMaskType;
typedef EventMaskType* EventMaskRefType;

/** The service may be called from an interrupt service routine and
 *  from the task level, but not from hook routines.
 *  The events of task <TaskID> are set according to the event
 *  mask <Mask>. Calling SetEvent causes the task <TaskID> to
 *  be transferred to the ready state, if it was waiting for at least
 *  one of the events specified in Mask.
 *
 *  @param TaskID  Reference to the task for which one or several events are to be set.
 *                 Please the, if NULL_TASK_ID is given the method returns E_OK and does
 *                 nothing.
 *  @param Mask    Mask of the events to be set.
 */
StatusType SetEvent ( TaskType TaskID, EventMaskType Mask );

/** This service returns the current state of all event bits of the task
 *  <TaskID>, not the events that the task is waiting for.
 *  The service may be called from interrupt service routines, task
 *  level and some hook routines (see Figure 12-1).
 *  The current status of the event mask of task <TaskID> is copied
 *  to <Event>.
 *
 * @param Mask Mask of the events to be cleared.
 *
 */
StatusType ClearEvent ( EventMaskType Mask );

/** This service returns the current state of all event bits of the task
 * TaskID, not the events that the task is waiting for.
 * The service may be called from interrupt service routines, task
 * level and some hook routines (see Figure 12-1).
 * The current status of the event mask of task <TaskID> is copied
 * to Event.
 *
 * @param TaskID Task whose event mask is to be returned.
 * @param Event  reference to the place wehre the event will be stored.
 */
StatusType GetEvent ( TaskType TaskID, EventMaskRefType Event );

/** The state of the calling task is set to waiting, unless at least one
 *  of the events specified in <Mask> has already been set.
 *  This call enforces rescheduling, if the wait condition occurs. If
 *  rescheduling takes place, the internal resource of the task is
 *  released while the task is in the waiting state. This service shall
 *  only be called from the extended task owning the event.
 *
 *  @param Mask Mask of the events waited for.
 */
StatusType WaitEvent ( EventMaskType Mask );

#endif
