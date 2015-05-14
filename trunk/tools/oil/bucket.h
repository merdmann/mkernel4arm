/***************************************************************************
 *  $Id: bucket.h 219 2008-10-12 17:23:57Z merdmann $
 *
 *  Aug 30, 2008 6:20:01 PM
 *  Copyright (C) 2008 Michael Erdmann
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

 /**
  * This is a support function which provide a simple storage bucket.
  */

#ifndef _BUCKET_H_
#define _BUCKET_H_


typedef struct _t_bucket_element {
	struct _t_bucket_element *fw;
	void *data;
} t_bucket_element;

typedef struct {
	t_bucket_element *head;
	t_bucket_element *tail;
} t_bucket;

t_bucket *bucket_create(void);
void bucket_delete(t_bucket *p);
void bucket_append(t_bucket *p, void *data );

typedef struct {
	t_bucket_element *next;
} t_bucket_cursor;

t_bucket_cursor *bucket_cursor_create(t_bucket *p);
void bucket_cursor_delete(t_bucket_cursor *c);
void *bucket_cursor_next(t_bucket_cursor *p);

#endif

