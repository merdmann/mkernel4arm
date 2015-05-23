/***************************************************************************
 *  $Id: resource.h 156 2008-06-28 10:05:26Z merdmann $
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

/** @file
 *  OSDEK/VDX Task interface
 *
 */
#ifndef _TASK_H
#define _TASK_H

#include "typedefs.h"
#include "proc.h"

/** DeclareTask serves as an external declaration of a task. The
 * function and use of this service are similar to that of the external
 * declaration of variables.
 */
#define DeclareTask(s)

/** The task <TaskID> is transferred from the suspended state into
 * the ready state. The operating system ensures that the task
 * code is being executed from the first statement.
 * The service may be called from interrupt level and from task
 * level.
 * Rescheduling after the call to ActivateTask depends on the
 * place it is called from (ISR, non preemptable task, preemptable
 * task).
 * If E_OS_LIMIT is returned the activation is ignored.
 * When an extended task is transferred from suspended state
 * into ready state all its events are cleared.
 */
StatusType ActivateTask ( TaskType id );

/** This service causes the termination of the calling task. The
 * calling task is transferred from the running state into the
 * suspended state.
 */
StatusType TerminateTask ( void );

/** This service causes the termination of the calling task. After
 * termination of the calling task a succeeding task <TaskID> is
 * activated. Using this service, it ensures that the succeeding task
 * starts to run at the earliest after the calling task has been
 * terminated.
 */
StatusType ChainTask ( TaskType id );

/** If a higher-priority task is ready, the internal resource of the task
 * is released, the current task is put into the ready state, its
 * context is saved and the higher-priority task is executed.
 * Otherwise the calling task is continued.
 */
//StatusType Schedule ( void );

/** GetTaskID returns the information about the TaskID of the task
 * which is currently running.
 */
StatusType GetTaskID ( TaskRefType task );

/** Returns the state of a task (running, ready, waiting, suspended)
 * at the time of calling GetTaskState.
 */
StatusType GetTaskState( TaskType id, TaskStateRefType state );

/** OSEK/VDX Naming convention for tasks */
#define TASK(x) void x(TASK_ARGUMENT arg __attribute__((unused)))


#endif

