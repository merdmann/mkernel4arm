/***************************************************************************
 *  $Id: idle.h 219 2008-10-12 17:23:57Z merdmann $
 *
 *  Fri Dec  7 21:19:36 2007
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
 *  This file contains the defintions needed for the implementation
 *  of the idle task.
 */
#ifndef IDLE_H
#define IDLE_H

#include "kernel.h"

extern void Idle_Task( TASK_ARGUMENT arg );

/** stack size of the idle task. This value should be selected as small a possible and
 * should be found expermentally. In our case it has the size of exactly one segment.
 */
#define STACK_SIZE_IDLE_TASK 160



#endif
