/***************************************************************************
 *  $Id: alarm.h 219 2008-10-12 17:23:57Z merdmann $
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
  * This file contains the interfaces for the OSEK/VDX compatible alarm
  * handling.
  */
#ifndef _ALARM_H
#define _ALARM_H

#define ALARMCALLBACK(x)  void x(AlarmType AlarmID)

/** This data type represents count values in ticks.*/
typedef unsigned long TickType;

/* This data type points to the data type TickType.*/
typedef TickType *TickRefType;

/** This data type represents a structure for storage of counter characteristics */
typedef struct {
	TickType maxallowedvalue; 	/* Maximum possible allowed count value in ticks */
	TickType ticksperbase;	  	/* Number of ticks required to reach a counter-specific unit*/
	TickType mincycle;		  	/* Smallest allowed value for the cycle-parameter */
} AlarmBaseType;

typedef AlarmBaseType* AlarmBaseRefType;

typedef struct {
	unsigned long value;
	unsigned long ticks;

	AlarmBaseType *base;
} t_counter;

/* internal presentation of an alarm */
typedef struct {
	BOOL     	active;
	unsigned long	expire;
	unsigned long 	cycle;
	t_counter 	*counter;
} t_alarm;


typedef t_alarm* AlarmType;  	/* This data type represents an alarm object. */


typedef void (*t_alarm_handler)(void);

extern t_alarm_handler _oil_alarm_handlers[];


/**
 * The system service GetAlarmBase reads the alarm base
 * characteristics. The return value <Info> is a structure in which
 * the information of data type AlarmBaseType is stored.
 *
 * @param AlarmId      Reference to alarm
 * @param Info         Reference to structure with constants of the alarm base.
 *
 */
<<<<<<< .merge_file_a05052
StatusType GetAlarmBase ( AlarmType AlarmId, AlarmBaseRefType info );
=======
StatusType GetAlarmBase ( const AlarmType AlarmId, const AlarmBaseRefType info );
>>>>>>> .merge_file_a00592

/**
 * The system service GetAlarm returns the relative value in ticks
 * before the alarm <AlarmID> expires.
 *
 * @param AlarmID      Reference to an alarm
 * @param Info         Relative value in ticks before the alarm <AlarmID> expires.
 *
 */
<<<<<<< .merge_file_a05052
StatusType GetAlarm ( AlarmType AlarmID, TickRefType Info);
=======
StatusType GetAlarm ( const AlarmType AlarmID, const TickRefType Info);
>>>>>>> .merge_file_a00592

/**
 * The system service occupies the alarm <AlarmID> element.
 * After <increment> ticks have elapsed, the task assigned to the
 * alarm <AlarmID> is activated or the assigned event (only for
 * extended tasks) is set or the alarm-callback routine is called.
 * The behaviour of <increment> equal to 0 is up to the
 * implementation.
 * If the relative value <increment> is very small, the alarm may
 * expire, and the task may become ready or the alarm-callback
 * may be called before the system service returns to the user.
 * If <cycle> is unequal zero, the alarm element is logged on again
 * immediately after expiry with the relative value <cycle>.
 * The alarm <AlarmID> must not already be in use.
 * To change values of alarms already in use the alarm shall be
 * cancelled first.
 * If the alarm is already in use, this call will be ignored and the
 * error E_OS_STATE is returned.
 * Allowed on task level and in ISR, but not in hook routines.
 *
 * @param AlarmID   Reference to the alarm element
 * @param increment Relative value in ticks
 * @param cycle     Cycle value in case of cyclic alarm. In case of single alarms, cycle shall be zero.
 *
 */
<<<<<<< .merge_file_a05052
StatusType SetRelAlarm ( AlarmType AlarmID, TickType increment, TickType cycle );
=======
StatusType SetRelAlarm ( const AlarmType AlarmID, const TickType increment, const TickType cycle );
>>>>>>> .merge_file_a00592

/**
 * The system service occupies the alarm <AlarmID> element.
 * When <start> ticks are reached, the task assigned to the alarm
 * <AlarmID> is activated or the assigned event (only for extended
 * tasks) is set or the alarm-callback routine is called.
 * If the absolute value <start> is very close to the current counter
 * value, the alarm may expire, and the task may become ready or
 * the alarm-callback may be called before the system service
 * returns to the user.
 * If the absolute value <start> already was reached before the
 * system call, the alarm shall only expire when the absolute value
 * <start> is reached again, i.e. after the next overrun of the
 * counter.
 * If <cycle> is unequal zero, the alarm element is logged on again
 * immediately after expiry with the relative value <cycle>.
 * The alarm <AlarmID> shall not already be in use.
 * To change values of alarms already in use the alarm shall be
 * cancelled first.
 * If the alarm is already in use, this call will be ignored and the
 * error E_OS_STATE is returned.
 * Allowed on task level and in ISR, but not in hook routines.
 *
 * @param AlarmID Reference to the alarm element
 * @param start   Absolute value in ticks
 * @param cycle   Cycle value in case of cyclic alarm. In case of single alarms, cycle shall be zero.
 *
 */
<<<<<<< .merge_file_a05052
StatusType SetAbsAlarm ( AlarmType AlarmID, TickType start, TickType cycle );
=======
StatusType SetAbsAlarm ( const AlarmType AlarmID, const TickType start, const TickType cycle );
>>>>>>> .merge_file_a00592

/**
 * The system service cancels the alarm <AlarmID>.
 *
 * @param AlarmID Reference to an alarm
 */
<<<<<<< .merge_file_a05052
StatusType CancelAlarm ( AlarmType AlarmID );
=======
StatusType CancelAlarm ( const AlarmType AlarmID );
>>>>>>> .merge_file_a00592

void _os_initialize_alarm(void);
void _os_alarm_scheduler(void);

#endif
