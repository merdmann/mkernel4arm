/***************************************************************************
 *  $Id: errno.h 219 2008-10-12 17:23:57Z merdmann $
 *
 *  Sat Dec  8 09:47:04 2007
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
 *  Error codes of the operating system.
 *  This file contains all return codes of the oeprating system. Each Service 
 *  function of the operating system returns a status code of type StatusType.
 *  The error code are based on the OSEK/VDX standard.
 */

#ifndef _ERRNO_H
#define _ERRNO_H

/* return codes according to OSEK/VDX. */
#define E_OK			0
#define E_OS_ACCESS             1
#define E_OS_CALLEVEL           2
#define E_OS_ID			3
#define E_OS_LIMIT		4
#define E_OS_NOFUNC		5
#define E_OS_RESOURCE           6
#define E_OS_STATE		7
#define E_OS_VALUE		8

/* this section is used for internal error codes only */
#define E_OS_SYS_BASE	128
#define E_OS_SYS_ERROR(x)	((x)+E_OS_SYS_BASE)

#endif /* _ERRNO_H */
