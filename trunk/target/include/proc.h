/***************************************************************************
 *  $Id: proc.h 273 2011-03-03 21:03:19Z merdmann $
 *
 *  Wed Dec  5 07:07:57 2007
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
 * This file contains all definitions and types specific to the scheduling
 * and task control.
 *
 * \section intro_sec  Introduction
 *
 * A task is described by its context which consists of the following elements:
 *
 * \par Compiler Stack.
 *      This is the stack which is simulated by the compiler. The implementation
 *      of the stack depends on the compiler which is used. The current
 *      implmentation supports only stack grwoing from high addess towards
 *      low addresses.
 * \par Cpu stack.
 *      This is the build in stack of the cpu.
 * \par Code section.
 *      The code of the process to be executed. Multiple processes may share
 *      the same code.
 * \par Data section.
 *      There is no such thing as a data segment. Every process can
 *      access every address in the RAM at any time.
 * \par CPU registers.
 *      All computing related registers.Hardware and interrupt
 *      registers do _not_ belong to the process context including the registers
 *      in the access file.
 *
 * \section states_sec State Model
 *
 * A process is either computing or waiting. Computing and waiting for events
 * or resources.
 *
 * \section scheduling_sec Scheduling
 *
 * The kernel provides two task control approaches.
 *
 * a) Coorperative scheduling where the application decides when
 * to pass the control to a different task (OIL Task directive
 * SCHEDULE=NONE).
 *
 * b) premptive scheduling where the current task is interrupted by a clock and
 * the scheduler switches to a different process (OIL Task directive
 * SCHEDULE=NONE).
 *
 *
 * \section priority_sec Priority
 *
 * Each process has a priority which is statically assigned to the task
 * in the OIL definition file by the PRIORITY attribute. The TCB array
 * entries are sorted according the priority of the tasks.
 * The task selection process starts allways with the lowest index (highest priority) of
 * the TCB array. Consequently the task with the highest priority is more likely
 *  being selected then the once with the highest priority.
 *
 * \section wdt_sec Watchdog timer and scheduling
 *
 * If the watch doc timer is used and high prio processes are running longer
 * then the watchdog timer, the system will be restarted. There for it might
 * make sense to include the Watchdog_Clear primitive in the main loop of your
 * application
 *
 * \section intr_handler Interrupt handling
 *
 */
#include "typedefs.h"
#include "resource.h"

#ifndef PROC_H
#define PROC_H

/* process states. */

/** The process is free. This indicates an unused TCB entry */
#define PROC_FREE       	0

/** The task is ready for CPU usage. Task in this state a subject to
 * execution.
 */
#define PROC_COMPUTING		1

/** Task is terminated. A TCB in this state indicates a task which
 *  is in the process of termination. The scheduler will release
 *  on the next scheduling of this process all resources which are
 *  registered with this process.
 */
#define PROC_TERMINATE  	2

/** Wait for an EVENT. The scheduler provides a simple event mail
 *  box (see kernel.h). The process waits for an event from this
 *  mail box for a given time.
 */
#define PROC_WAIT_EVENT 	4

/** Wait for a resource to become free */
#define PROC_WAIT_RESOURCE  	5

/** OSEK/VDX: suspended state */
#define PROC_SUSPENDED	    	6

/** Is returned if you enquire the status of an task with an id which is
 * not in the range of the allowes task id's
 */
#define PROC_NOT_EXIST		8

/** this type is used to denote tasks. */
//typedef byte t_task_id;

/** OSEK:VDX task identification */
typedef t_task_id TaskType;
typedef TaskType* TaskRefType;

/** address of an data area */
//#define DADDR __near byte*

/** NULL reference to the code of the task */
//typedef void (*t_task_code)(void*);

/** VDX/OIL: full preemptive scheduling */
#define FULL	1
/** VDX/OIL: non preemptive scheduling */
#define NON	0


/** Task Argument. A task is represented as function which gets one argument of
 *  the type TASK_ARGUMENT.
 */
typedef POINTER TASK_ARGUMENT;

typedef void (*t_entry)(TASK_ARGUMENT);

/** the static part of an task.
 * A process descriptor constist of an static part which describes the process
 * and a dynamic part which realtes to the dynamic behaviour of the task.
 */
typedef struct {
	byte max_activations;
	byte priority;
	byte schedule;
	t_entry entry;
	DATA POINTER tos;
} t_task_descriptor;



/** The task control block. The scheduling is based on this control block. The
 *  scheduling state depends on the state variable. A task control block is free
 *  if the variable state has the value PROC_FREE.
 */
typedef struct {
	DATA POINTER	stack;     	/**< base address of the stack */
	t_task_id       admin;		/**< owner of the control block */
	byte       		state;    	/**< state of the process */
	byte			prio;	  	/**< priority of the process */
	unsigned  		event;    	/**< event word */
	unsigned		mask;	  	/**< the events we are waiting for */
	DATA POINTER    wait_addr; 	/**< the address of the tasks lock resource */
	uint32_t	activations;    /**< activation requested */

	const t_task_descriptor *descriptor;/**< points to task descriptor */
} t_tcb;


/** Idle task number. This id may be used if you want to send some events to the idle
 *  task. The idle task will consume the event without any side effect.
 */
#define NULL_TASK_ID	128

extern t_tcb _TCB[];
extern const int _oil_max_tcb;
extern BOOL _oil_use_preemption;

/** The size of the kernel stack. When the tick handler is called it runs on the so called
 *  kernel stack. This is done to safe stack space for each task, otherwise the space
 *  would have been needed on each individial stack.
 */
#define STACK_SIZE_KERNEL   150

/** Schedule shall not be called directly by any user process. It is called by the
 * interrupt handlers to select the next process.
 *
 */
extern void _os_schedule( void );

/*
 * This method is used by the system generator to create a task
 * instance.
 */
StatusType _os_task_create( const t_task_id id, const t_task_descriptor *descr );

/** claim the tcb for administration purpose
 *  @param pointer to the control block
 *
 *  This function claims the tcb for exclusive use. The claimed tcb
 *  will not be considered by the scheduler until it is unlocked. If the tcb is
 *  claimed by a different user (process), the call will get blocked until
 *  the tcb is released.
 *
 *  This function shall be used by all methods which are modifying the
 *  contents of the tcb asynchronously to the Schedule procedure. If the
 *  tcb is claimed the schedule will _not_ change the tcb until it
 *  has been released.
 *
 *  If called from an interrupt handler the method does nothing.
 *
 */
void _os_claim_tcb(const t_task_id id);

/** release a tcb which has been claimed with _os_claim_tcb.
 *  @param pointer to the control block.
 *
 *  If a tcb has been locked by means of the _os_claim_tcb method this method is
 *  used to unlock the tcb after all modifications to the tcb have been
 *  completed.
 *
 *  If called from an interrupt handler is does nothing.
 *
 */
void _os_release_tcb(const t_task_id id);

/**
 * @brief Trap in case the process terminates unexpected
 * @details This is expected to be called in case the process terminates due to an
 *          stack underflow.
 */
void _os_process_trap(void);

/** _os_kernel_stack should not be used by any application since it is
 * used as an interfaces between the oil generated code and the kernel.
 *
 */
extern POINTER _os_kernel_stack;

/** The user mode indicates whether an interrupt or the applicaiton
 * has called the kernel.
 */
extern byte _os_mode;

#define USER_MODE 		0				/* Call from application */
#define KERNEL_MODE 	255				/* call from kernel */

#define UserMode(X) if(_os_mode == USER_MODE) { X ; }
#define IsUserMode(x) (_os_mode == USER_MODE)

/**
 * This is beeing generated by oil
 */

void _os_underflow_trap(void);
void _os_task_terminated(void);
void _os_initialize_scheduler(void) ;

#endif
