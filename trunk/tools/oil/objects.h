/***************************************************************************
 *  $Id: objects.h 219 2008-10-12 17:23:57Z merdmann $
 *
 *  Aug 30, 2008 6:16:17 PM
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
  * This module provides the object representation of for the OIL objects.
  *
  */

#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include <stdio.h>

#include "oil.h"
#include "bucket.h"
#include "attributes.h"

typedef struct _t_object {
	char *class;
	char *name;
	t_attribute_table *attributes;
	t_attribute_table *childs;
	struct _t_object *parent;
	struct _t_object *next;
} t_object;

/**
 * create an object of a certain class and name.
 * @param class  - the oil object type name
 * @param name   - the instance name or name
 * @param parent - parent of the object.
 */
t_object *object_create( char *class, char *name, t_object *parent);

/**
 * add an attributr to the given object.
 *
 * @param o the object
 * @param name name of the attribute
 * @param value the actual which is a reference to an object.
 */
int object_add_attribute(t_object *o, char *name, t_object *value);
t_object *object_get_attribute(t_object *o, char *name );

void object_add_child(t_object *o, char *name, t_object *child);

#define object_first_object(o) ((o)->next)
#define object_next_object(o) ((o)->next)

void object_append_object(t_object *o, t_object *item);
void object_dump( t_object *root );

/*
 * locate a given application by name in the object table
 */
t_object *locate_application(char *name) ;

#define OBJECT_CHILDS_BUCKET ATTRIBUTE_BUCKET(((o)->childs))
#define OBJECT_ATTRIBUTES_BUCKET ATTRIBUTE_BUCKET(((o)->attributes))

typedef struct {
	t_bucket_cursor *next;
	char *class;
} t_object_query;


t_object_query *object_query_create(t_object *root, char *class);
void object_query_delete(t_object_query *query);
t_object *object_query_next(t_object_query *query);


extern t_object *document;


#endif



