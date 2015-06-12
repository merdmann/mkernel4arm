 /***************************************************************************
 *  $Id: alarm.c 220 2008-10-12 17:31:27Z merdmann $
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
  * OSEK/VDX: This module contains the alarm management.
  *
  *
  * An alarm is specified by two parameters; the alarm value and the
  * cycle length which is to be specified in the oil file:
  *
  * 	ALARM UpdateAlarm {
  * 		COUNTER = T1_Counter;  // source for the alarm
  *
  *     	AUTOSTART = TRUE {
  *				ALARMTIME = 2000;
  *				CYCLETIME = 200;
  *			};
  *		};
  *
  *     The timer T1_ounter will be used to clock the alarm. The alarm
  *     time is the absolute time in counter units from the
  *		start of the system. This condition fires only once.
  *		The cycle specifies the time between alarms in counter units.
  *
  *
  *
  */
#include "kernel.h"
#include "proc.h"
#include "alarm.h"
#include "machdep.h"

void _os_initialize_alarm() {
}


void _os_alarm_scheduler() {
	int i;

	for(i=0; _oil_alarm_handlers[i] != NULL; ++i ) 
		(*_oil_alarm_handlers[i]) ();	
}



StatusType GetAlarmBase ( const AlarmType AlarmId, const AlarmBaseRefType info ) {
	info->maxallowedvalue = AlarmId->counter->base->maxallowedvalue;
	info->mincycle = AlarmId->counter->base->mincycle;
	info->ticksperbase = AlarmId->counter->base->ticksperbase;

	return E_OK;
}

StatusType GetAlarm ( const AlarmType AlarmID, const TickRefType info) {
    *info = AlarmID->expire - AlarmID->counter->value;
	return E_OK;
}

StatusType SetRelAlarm ( const AlarmType AlarmID, const TickType increment, const TickType cycle ) {
	if( AlarmID->active )
	    return E_OS_STATE;

	AlarmID->cycle = cycle;
	_machdep_critical_end();
	AlarmID->expire = AlarmID->counter->value + increment;
	_machdep_critical_begin();

	AlarmID->active = TRUE;
	return E_OK;
}

StatusType SetAbsAlarm ( const AlarmType AlarmID, const TickType start, const TickType cycle ) {
	if( AlarmID->active )
	    return E_OS_STATE;

	AlarmID->expire = start;
	AlarmID->cycle = cycle;

	AlarmID->active = TRUE;
	return E_OK;
}

StatusType CancelAlarm ( const AlarmType AlarmID ) {
	AlarmID->active = FALSE;

	AlarmID->expire = 0;
	AlarmID->cycle = 0;

	return E_OK;
}
