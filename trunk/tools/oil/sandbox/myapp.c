/***************************************************************************
 *  $Id: myapp.c 265 2011-02-04 16:32:11Z merdmann $
 *
 *  Fri Nov 23 06:52:21 2007
 *  Copyright  2007  Michael Erdmann
 *  michael.erdmann@snafu.de
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

#include "typedefs.h"
#include "kernel.h"
#include "alarm.h"
#include "ior5e/ior5e.h"
#include "myapp_defs.h"

DeclareEvent(LED_ON);
DeclareEvent(LED_OFF);
DeclareEvent(LED_Tick);

/** this resource lock is used to communicate between recevier and sender task. */
DeclareResource( myresource );

#pragma udata userbank state

static int state;

void Toggle_LED() {

	LED_On(LD4_A);

	if( state )
		SetEvent( TASK_ID_Receiver, LED_ON );
	else
		SetEvent( TASK_ID_Receiver, LED_OFF);

	state = ~state;
}

/** Demonstrates the usage of claim, wait */
TASK(Sender) {
	TaskType my_id;

	GetTaskID( &my_id );

	state = 0;

	LED_On(LD5_B);

	SetAbsAlarm( myAlarm, 400, 10);

	while(1) {
		unsigned event;

		WaitEvent( LED_Tick );
		GetEvent( my_id, &event );
		ClearEvent( LED_Tick );

		if( event & LED_Tick ) {
			if(state) {
				LED_Off(LD7_B);
				SetEvent(TASK_ID_Receiver, LED_ON);
			}
			else {
				LED_On(LD7_B);
				SetEvent(TASK_ID_Receiver, LED_OFF);
			}
			state = ~state;
		}
	}
}


TASK(Receiver) {
	TaskType my_id;

	GetTaskID( &my_id);

	LED_On(LD4_B);

	while(1) {
		unsigned event;

		WaitEvent(LED_OFF|LED_ON);
		GetEvent( my_id, &event );
		ClearEvent( LED_ON|LED_OFF);

		if( event & LED_ON )
			LED_On( LD8_B);
	    else
	    	LED_Off( LD8_B);
	}

	TerminateTask();
}



/*
 * This piece of code has to figure out the application mode and call
 * the correct init procedure. In our case there is only one mode.
 */
int main() {
	StartOS(Application_Mode_IOR5E);
	return 0;
}
