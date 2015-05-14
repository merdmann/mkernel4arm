/***************************************************************************
 *  $Id: bucket.c 242 2009-05-22 16:39:55Z merdmann $
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

 /** Description of the module
  *  A bucket stored peices of information in memory.
  */
#include <stdio.h>
#include <stdlib.h>

#include "bucket.h"

#define new(x)  malloc(sizeof(x))


t_bucket *bucket_create(void) {
	t_bucket *result;

	if((result = new(t_bucket))==NULL )
		return NULL;

	result->head = NULL;
	result->tail = NULL;

	return result;
}

void bucket_delete(t_bucket *b) {
	t_bucket_element *p, *next;

	for(p=b->head; p!=NULL; p=next) {
		next = p->fw;
		free(p);
	}
}


void bucket_append(t_bucket *b, void *data ) {
	t_bucket_element *e;

	e = new(t_bucket_element);
	e->fw = NULL;
	e->data = data;

	if( b->head == NULL )
		b->head = e;
	else
		b->tail->fw = e;

	b->tail = e;
}

t_bucket_cursor *bucket_cursor_create(t_bucket *b) {
	t_bucket_cursor *result;

	result = new(t_bucket_cursor);

	result->next = b->head;

	return result;
}

void bucket_cursor_delete(t_bucket_cursor *p) {
	free(p);
}

void *bucket_cursor_next(t_bucket_cursor *p) {
	void *result;

	if( p->next == NULL )
		return NULL;

	result = p->next->data;

	p->next = p->next->fw;

	return result;
}
