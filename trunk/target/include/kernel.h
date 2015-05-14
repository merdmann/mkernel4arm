/***************************************************************************
 *  $Id: kernel.h 271 2011-02-20 10:31:23Z merdmann $
 *
 *  Fri Dec  7 21:14:02 2007
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
 *  This file contains the complete interface description of the kernel.
 *
 *  \section concepts_section Concepts
 *  Mkernel in inspired by the ideas OSEK/VDX, which means it provides the concept
 *  of tasks, alarms and events.
 *
 *  The implementation of a task looks as shown below. The task function may defines
 *  some local variables which allocated on the stack of the task. Typically there
 *  will be a loop which does something use full until the job is done and the
 *  process may terminate.
 *  \code
 *  void TASK(Receiver) {
 *		int some_local_data = .... ;
 *
 *		while( .. not end ...) {
 *
 *			... do some thing usefull ...
 *		}
 *
 *		Task_Terminate();
 *  }
 *  \endcode
 *
 *  The termination of the process is done by means of the Task_Terminate call
 *  which terminates the current process.
 *  \note Before terminating the process make sure that all common resources
 *  have been released correctly, since mkernel does not keep track of these
 *  resources.
 *
 *  In order to create the infra structure around this task, the task needs to be
 *  specified in the OIL file as well:
 *
 *  \code
 *	TASK Receiver {
 *		PRIORITY = 1;
 *		STACKSIZE = 128;
 *		SCHEDULE = FULL;
 *		AUTOSTART = TRUE;
 *	};
 *  \endcode
 *
 *  This section will generate automatically init code which will startup the
 *  task Receiver after system startup as a fully preemptive task with a stack
 *  of 128 byte.
 *
 *  The application developer has to provide a main function which has to detect
 *  the application mode and to startup the operating system with the identified
 *  application mode.
 *
 *  \code
 *  #include "myapp_defs.h"
 *
 *  int main() {
 *     AppModeType mode;
 *
 *     .... determine application mode ....
 *		mode = Mode_ ..... ;
 *
 *	   StartOS(mode);
 *  }
 *  \endcode
 *
 *
 *  \section oil_systembuiler Systembuider and static resources
 *
 *  In contradiction to fully fledged multitasking operating systems which
 *  allow multiple instances of the same task running in parallel mkernel
 *  allows only one instance per task. This has been done to avoid complex
 *  dynamic resource management at run time. All resources are allocated
 *  statically during the system generator run (oil).
 *
 * \section ipc_section Inter Process Communication
 *
 *  In order to communication between different processes events are used. Events
 *  are represented as a bit mask, where each bin represents an event.
 *
 *  A task may wait for one or more events by means of the WaitEvent service.
 *
 * \section interrupt_prio Interrupt Priorities
 *
 * There are two types of interrupt handlers. If an low priority interrupt occures,
 * the kernel will save the context of the interrupted process, check if this interrupt
 * has been caused by the tick timer. If yes the preemption code will be called and the
 * context of the next task restored. Other wise the device interrupt handler will be
 * invoked. After this has been done the scheduler will be executed and the context
 * of the selected task will be restored.
 * In case of an high priority, the kernel will check if the interrupt is related
 * the dedicated timer T1. If not, the device handler is called to check what
 * kinf of interrupt has occured. After execution of the interrupt handler the
 * execution returns to the interrupted task. No scheduling will take place.
 *
 * \section interrupt_sec Process control from interrupt handlers
 *
 */

/** \mainpage
 *  \version 0.7.0
 *  \date    27 Sept. 2008
 *  \author  Michael Erdmann
 *  \section intro_sec Introduction
 *  Mkernel is a minimalistic kernel for the pic18f4550 series. Mkernel in inspired by the
 *  ideas OSEK/VDX, which means it provides tasks, alarms and events. It is GPL licensed
 *  (see licensing section).
 *
 *  \section intro_install Unpacking the distribution
 *
 *  After down loading the source tar ball the sources need to be unpacked:
 *  \code
 *  gunzip -c mkernel-x.y.z-src.tar.gz | tar xvf -
 *  \endcode
 *  This command will create in the current directory a sub directory with the name
 *  mkernel-x.y.z-src which contains the sources of mkernel version x.y.z.
 *
 *  After unpacking the source tar ball, you will find the following directories:
 *  \code
 *  ./bin			- initially empty will contain all native tools
 *  ./doc			- Documentation source files (currently empty)
 *  ./fsusb			- The USB loader for linux
 *  ./include		- all include files
 *  ./ior5e			- sources for related to the IOR5e board
 *  ./kernel		- sources of the kernel
 *  ./lib			- This directory will contain all libs after building
 *  ./pic18fdevices	- sources of all driver handling pic18f internal hardware
 *  ./regression	- sources of regression tests
 *  ./oil			- sources of the OIL compiler
 *  ./tools			- some helpfull tools
 *  ./usb			- usb endpoint handling
 *  ./var			- temporary directory
 *  \endcode
 *
 *  ./include   - all include files
 *  ./kernel    - the kernel sources
 *
 *  \section config_sec Configuration
 *  The MKernel can be configured by adopting setting in the file mkernel_config.h.
 *  Do not change settings in any other include file unless you know exactly
 *  what you are doing.
 *
 *  \section build_sec Building
 *  Currently the following tools are needed to build the kernel
 *
 *  \li SDCC - Version 2.8.0
 *  \li gputils 0.13.4
 *  \li fsub 0.1.11-2
 *  \li bawk
 *
 *  In order to build mkernel the following command has to be executed
 *  in the main directory.
 *  \code
 *      $ make deps
 *      $ make clean
 *      $ make
 *  \endcode
 *
 *  In order to reuse the existing code it is very likely that some of the
 *  hidden hardware dependencies have to be resolved. Pls. check the following
 *  files:
 *
 *  panic.c, trace.c, digio,c
 *
 *  In order to test yor installation you may use the regression tests which are
 *  located in the directory ./regression. In order to compile and load a certain
 *  test case use the following commands:
 *
 *  \code
 *      $ cd ./regression
 *      $ make TC=bas_XXXX load
 *  \endcode
 *
 *  \section doc_sec Documentation
 *  Documentation is included in the include files using doxygen syntax.
 *  If doxygen is available on your system you may create the documentation
 *  file by executing the command below in the top level directory of the
 *  mkernel.
 *  The output of this step are html files which are stored in the directory
 *  var in subdirectories.
 *
 *  \code
 *      $ make doc
 *  \endcode
 *
 *  \section test_sec Testing of the kernel
 *  The directory regression contains the set of regression tests for the
 *  kernel functions.
 *  Documentation is inline in the test cases and can be found after generating
 *  the documentation in var/regression.
 *
 *  \section example_sec How to start
 *  If you like to start with your own application it makes sense to start
 *  from an existing example.
 *  For time being use the source files in the regression test directory.
 *
 *  \section hardware_sec Hardware
 *  The current implementation is based on:
 *  \li SBC44UC - A single board computer (www.modtroinix.xom)
 *  \li IOR5E - IO Extension board (www.modtronix.com)
 *
 *  \section porting_sec Porting
 *  Currently porting the kernel to other pic's, hardware or software environments
 *  is not yet taken into consideration but is expected to be possible.
 *
 *  \subsection port_comp Porting to a different compiler
 *
 *  This will be a bit difficult since i am using some peculiarities of the
 *  sdcc compiler, as for example __code, __near keyword and the fact
 *  that a certain space in the page 0 of the memory is used by the compiler.
 *
 *  \subsection port_hw  Porting to a different embedding hardware
 *
 *  The current kernel has been developed and tested using an SBC board from
 *  modtronix which includes an RTC, 16 LED's and other i/o interface including
 *  relays. The kernel does not depend on these component but is using
 *  some of them to indicate the CPU status to the outside (see directory ior5e)
 *  For more detail consult the include file trace.h. The functions listed here have
 *  to be ported if needed.
 *
 *  \subsection port_cpu Porting to a different CPU family.
 *
 *  Since most of the kernel is written in C only the assembler parts have
 *  to be reworked. Please note that the stack point is expected to be
 *  16 Bit's.
 *
 *  \section copy_sec Copyright Notice
 *  This software is licensed under GPL. See the COPYING file.
 *  \verbinclude COPYING
 *
 *
 */
#ifndef KERNEL_H
#define KERNEL_H

#include "typedefs.h"
#include "errno.h"
#include "proc.h"
#include "timer.h"
#include "event.h"
#include "resource.h"
#include "task.h"
#include "alarm.h"
#include "execution.h"
#include "interrupts.h"

/** Initiate a scheduling. This function should _not_ be called
 *  by applications. Applications should use Task_Switch(ANY_TASK)
 */
void _os_yield(void);

void _os_save_context(void);
void _os_restore_context(void);

/** My own task id. */
extern t_task_id  _os_my_task_id;
extern t_task_id  _os_next_task;

extern void TaskBeginHook(t_task_id id);
extern void TaskEndHook(t_task_id id);

/** Panic handler
 * The pnaic handler has to be implemented for a specific hardware but
 * it is used all over the kernel.
 *
 * @param nbr - number of the error condtion.
 */
extern void _os_panic(int nbr);

#endif
