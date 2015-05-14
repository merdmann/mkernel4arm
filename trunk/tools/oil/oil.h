/***************************************************************************
 *  $Id: oil.h 219 2008-10-12 17:23:57Z merdmann $
 *
 *  Aug 29, 2008 6:40:06 PM
 *  Copyright 2008 Michael Erdmann
 *  Email: Michael.Erdmann@snafu.de
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

#ifndef OIL_H_
#define OIL_H_

#define FALSE  0
#define TRUE  (!FALSE)

#define MSG_SIZE	256

#define PRIVATE static

void report(char *s);
void alert(char *s);
void info(char *s);


extern unsigned max_tasks;
extern unsigned max_alarms;
extern unsigned event_bits;

#define OIL_NOTHING	 1
#define OIL_ERROR	-1
#define OIL_OK		 0

#endif /* OIL_H_ */
