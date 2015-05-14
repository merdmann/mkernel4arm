/***************************************************************************
 *  $Id: wdt.h 150 2008-06-16 20:17:25Z merdmann $ - Trace facility
 *
 *  Wed Dec  5 19:25:02 2007
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
 *  This module contains the watch dog timer.
 *  The watchdog timer expiers aproximatlay every second and causes
 *  a restart of the cpu. The idle task clears regulary the watch dog
 *  counter. If the idle task is not scheduled for a long time, the 
 *  cpu performs a restart.
 */ 
#ifndef _WDT_H
#define _WDT_H

/** Clear the watchdog timer  */
void Watchdog_Clr(void);

/** This procedure initializes the watch dog function */
void Watchdog_Init(void);

#endif /* _TRACE_H */
