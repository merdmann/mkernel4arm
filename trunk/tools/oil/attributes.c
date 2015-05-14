/***************************************************************************
 *  $Id: attributes.c 219 2008-10-12 17:23:57Z merdmann $
 *
 *  Aug 31, 2008 9:52:18 AM
 *  Copyright 2008 mkernel
 *  Email:
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
  *
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bucket.h"
#include "attributes.h"

#define new(x)  malloc(sizeof(x))


t_attribute_table *attribute_table_create() {
	t_attribute_table *result;

	if( (result=new(t_attribute_table))==NULL )
		return NULL;

	result->attrs = bucket_create();

	return result;
}

void attribute_table_delete(t_attribute_table *t) {
	bucket_delete(t->attrs);
}

void attribute_add(t_attribute_table *t, char *name, void *value ) {
	t_attrib_pair *data;

	data = new( t_attrib_pair );
	data->name = strdup(name);
	data->value = value;

	bucket_append( t->attrs, data );
}

void *attribute_get(t_attribute_table *t, char *name) {
	t_bucket_cursor *c;
	t_attrib_pair *p;

	if( (c = bucket_cursor_create(t->attrs))==NULL )
		return NULL;

	while( (p=bucket_cursor_next(c))!=NULL ) {
		if( (p->name != NULL) && !strcmp(p->name, name ) )
			break;
	}
	bucket_cursor_delete(c);

	if(p!=NULL)
		return p->value;
	else
	    return NULL;
}

