/***************************************************************************
 *  $Id: panic.h 219 2008-10-12 17:23:57Z merdmann $
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
#ifndef _PANIC_H
#define _PANIC_H

/** @file
 *  This is the interface specification for the panic interface. Panic will
 *  be called by the kernel in case of unrecoverable problems.
 */

/** Display panic information
 *
 * @param nbr panic number
 *
 * This procedure is expected to handle a panic situation, by displaying
 * the panic number, some additional information and to stop the operating
 * system. The implemenation is hardware dependend.
 */

#define PANIC_STACK_UNDERFLOW		1
#define PANIC_UNDERFLOW_TRAP		2
#define PANIC_UNKNOWN_PROC_STATE	3
#define PANIC_WDT			4


#endif /* _PANIC_H */
