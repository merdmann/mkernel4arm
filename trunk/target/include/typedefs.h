/***************************************************************************
 *  $Id: typedefs.h 271 2011-02-20 10:31:23Z merdmann $
 *
 *  Mon Nov 26 06:59:07 2007
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
 *  This file contains commonly used type definitions.
 */

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

typedef unsigned char   byte;           // 8-bit
typedef unsigned int    word;           // 16-bit
typedef unsigned long   dword;          // 32-bit

/* ......................... OSEK/VDX Types .................................. */
/* OSEK/VDX data types */
typedef byte   		StatusType;
typedef byte   		TaskStateType;
typedef TaskStateType*  TaskStateRefType;

/** TaskStateType may have the following values */
#define INVALID_TASK		0
#define RUNNING			1
#define WAITING			2
#define READY			3
#define SUSPENDED		4

typedef byte   AppModeType;

/* ............................................................................ */

//#define HANDLE void*
#define POINTER void*

#define CODE 			/* code */
#define DATA     		/* data in general */
#define KDATA			/* indicates kenerel data */

typedef enum _BOOL { FALSE = 0, TRUE  } BOOL;

#define OK      TRUE
#define FAIL    FALSE
#define NULL	((void*)0)
typedef dword t_task_id;

#endif //TYPEDEFS_H
