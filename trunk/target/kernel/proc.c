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
 */
#include "kernel.h"
#include "panic.h"
#include "wdt.h"
#include "timer.h"
#include "idle.h"

#include "machdep.h"

/* Public variables */
KDATA t_task_id  _os_next_task;			  /* this is the next task to be selected */
KDATA t_task_id  _os_my_task_id;      /* Id of the current task */
KDATA byte _os_mode = 0;				      /* will be set by the interrupt handler */
KDATA byte _os_intr_active = 0;			  /* indicate an active interrupt handler */
KDATA POINTER _os_current_stack; /* current stack catched by save_context */

/*
 * 
 */
void _os_process_trap(void) {
	_machdep_trace(TRACE_PANIC_UNDERFLOW);
}

/*
 * This procedure will only be called if a task terminates.
 */
void _os_task_terminated(void) {

	_os_claim_tcb(_os_my_task_id);
	_TCB[_os_my_task_id].state = PROC_SUSPENDED;
	_os_release_tcb(_os_my_task_id);

  _os_schedule();
  
	_machdep_yield();
	/* should never return to this point */
}

/*
 * Claim the TCB for exclusive administration.
 */
void _os_claim_tcb(TaskType id) {
	UserMode(while( !_machdep_cas_byte( (DATA POINTER)&_TCB[id].admin, NULL_TASK_ID, _os_my_task_id) ));
}

/*
 * release the tcb for other user.
 */
void _os_release_tcb(TaskType id) {
	UserMode(_machdep_cas_byte( (POINTER)&_TCB[id].admin, _os_my_task_id, NULL_TASK_ID ));
}

/*
 * This function is directly called either from the preemption
 * timer handler (_os_cpu_tick) or from the yield function.e Consequently
 * this function is executed on the user and the kernel stack!
 *
 *  The purpose is to select the next task which is to be running.
 *  As sideeffect this procedure set the variables which are describing
 *  the next process to be executed.
 *
 *  Schedule is called either from an interrupt handler or a
 *  user process.
 */
void _os_schedule() {
    byte start = 0;
    t_task_id last_task_id;

    /* if non preemptable tasks is running or an admin task is pedning no recscheduling */
    if( (_TCB[_os_my_task_id].descriptor->schedule == NON  && _TCB[_os_my_task_id].state == PROC_COMPUTING) 
      || _TCB[_os_my_task_id].admin != NULL_TASK_ID )
        return;

    /* save the context by storing the current stack */
    last_task_id = _os_my_task_id;
    _TCB[last_task_id].stack = _os_current_stack;


    /* define the startpoint for the scan */
    if( _os_next_task != NULL_TASK_ID ) {
       	start = _os_next_task;
    	_os_next_task = NULL_TASK_ID;
    }
    else
   	start = 0;

    TaskEndHook(_os_my_task_id);

    /*
     * Scan all tasks and select an applicable task. TCB's are sorted according to
     * priority of the task.
     */
    {
       byte i = 0;
       byte current = start;
       byte state = PROC_FREE;

       while ( i < _oil_max_tcb ) {
    	   current = (start + i++) % _oil_max_tcb;
           state = _TCB[current].state;

           /* tasks with this flags set are manipulated by a task */
           if( _TCB[current].admin != NULL_TASK_ID || state == PROC_SUSPENDED )
        	   continue;

           if( state == PROC_WAIT_RESOURCE ) {
        	   /* OSEK/VDX: we are waiting for an resource to become free */
               ResourceType p = (ResourceType)(_TCB[current].wait_addr);

               if( p->owner == NO_OWNER ) {
            	   p->owner = current;

         		   state = PROC_COMPUTING;
               	   break;
               }
           }
           else if( state == PROC_WAIT_EVENT ) {
               /* OSEK/VDX: process wait for an set event flag, check for events */
               if( _TCB[current].event & _TCB[current].mask ) {
            	   state = PROC_COMPUTING;
                   break;
               }
           }
           else if( state == PROC_COMPUTING ) {
               break;
          }
       }
       if( state == PROC_COMPUTING ) {
    	   /*
    	    * if the current task id has not changed we can assume that nobody else
    	    * has executed schedule while we have searched for an executable task
     	    */
     	   if( _os_my_task_id == last_task_id ) {
    		     _TCB[current].state = state;
    		     _os_my_task_id = current;
       		   _os_current_stack = _TCB[current].stack;
    	   }
       }
    }

    TaskBeginHook(_os_my_task_id);
}

/*
 * This is an internal API to create all data for a task
 */
StatusType _os_task_create( t_task_id id, t_task_descriptor *descr) {

	_TCB[id].wait_addr = (DATA POINTER)0;
	_TCB[id].stack = _machdep_initialize_stack(descr->tos, descr->entry, (TASK_ARGUMENT)0);
	_TCB[id].state = PROC_SUSPENDED;
	_TCB[id].event = 0;
	_TCB[id].mask  = 0;
	_TCB[id].admin = NULL_TASK_ID;
	_TCB[id].prio =  descr->priority;
	_TCB[id].activations = 0;
	_TCB[id].descriptor = descr;

	return E_OK;
}

/*
 * Activate a task
 */
StatusType ActivateTask (TaskType id) {
	StatusType result = E_OS_LIMIT;

	_os_claim_tcb(id);
	if( _TCB[id].state  == PROC_SUSPENDED ) {
		t_task_descriptor *descr = _TCB[id].descriptor;

		_TCB[id].wait_addr = (DATA POINTER)0;
		_TCB[id].event = 0;
		_TCB[id].mask = 0;
		_TCB[id].stack = _machdep_initialize_stack(descr->tos, descr->entry, (TASK_ARGUMENT)0);
		_TCB[id].state = PROC_COMPUTING;
		_TCB[id].prio =  descr->priority;
		result = E_OK;
	}
	_os_release_tcb(id);

	_os_next_task = id;

	if( _os_mode == USER_MODE)
	    _machdep_yield();

	return result;
}

/*
 * Terminate the current task gracefully
 */
StatusType TerminateTask (void) {
  _os_claim_tcb( _os_my_task_id );
	_TCB[_os_my_task_id].state = PROC_SUSPENDED;
	_os_release_tcb ( _os_my_task_id );

	UserMode(_machdep_yield());

	return E_OK;
}

/*
 * Chain to next task by suspending the current state are activating the
 * new kas.
 */
StatusType ChainTask (TaskType id) {
    StatusType RC;

    _os_claim_tcb(id);

    RC = ActivateTask(id);
    _os_next_task = id;
	  _TCB[_os_my_task_id].state = PROC_SUSPENDED;

	  _os_release_tcb ( id );

	  UserMode(_machdep_yield());

	  return RC;
}

/*
 * Return the state of the task in terms of the OSEK Statemodel.
 */
StatusType GetTaskState ( TaskType id, TaskStateRefType state ) {
	switch( _TCB[id].state ) {
		case PROC_WAIT_EVENT | PROC_WAIT_RESOURCE:
			*state = WAITING;
			break;
		case PROC_COMPUTING:
			*state = RUNNING;
			break;

		case PROC_TERMINATE | PROC_SUSPENDED :
			*state = SUSPENDED;
			break;

		case PROC_FREE:
			*state = INVALID_TASK;
			break;
	}
	return E_OK;
}

StatusType GetTaskID ( TaskRefType task ) {
	*task =_os_my_task_id;

	return E_OK;
}

/*
 * Initialize the scheduler. Interrupts etc are expected to the setup
 * during the system initialization phase, but the interrupts are
 * turned off.
 */
void _os_initialize_scheduler(void) {
	_os_my_task_id = 0;
	_os_current_stack = _TCB[0].stack;
	_os_next_task = NULL_TASK_ID;

	_os_schedule();
}



