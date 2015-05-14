/***************************************************************************
 *  $Id: objects.c 219 2008-10-12 17:23:57Z merdmann $
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

 /** Description of the module
  *
  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bucket.h"
#include "attributes.h"
#include "objects.h"

#define new(x)  malloc(sizeof(x))

t_object *document = NULL;

t_object *object_create( char *class, char *name, t_object *parent) {
	t_object *result;

	if((result=new(t_object))==NULL)
		return NULL;

	result->class = strdup(class);
	result->name  = strdup(name);
	result->attributes = attribute_table_create();
	result->childs = attribute_table_create();
	result->parent = parent;
	result->next   = NULL;

	return result;

	if(document==NULL)
		document = new(t_object);

	object_add_attribute( document, class, result);
}

int object_add_attribute(t_object *o, char *name, t_object *value) {
	attribute_add( o->attributes, name, value );
}

t_object *object_get_attribute(t_object *o, char *name ) {
	return (t_object*)attribute_get( o->attributes, name);
}


t_object_query *object_query_create(t_object *root, char *class) {
	t_object_query *query;
	t_attrib_pair *pair;

	if( (query = new(t_object_query)) == NULL )
		return NULL;

	query->class = strdup(class);
	query->next = bucket_cursor_create(ATTRIBUTE_BUCKET(root->childs));

	return query;
}

void object_query_delete(t_object_query *query) {
	free(query->class);
	bucket_cursor_delete(query->next);
	free( query );
}

t_object *object_query_next(t_object_query *query) {
	t_object *result;
	t_attrib_pair *pair;

	pair = NULL;
	while( (pair=bucket_cursor_next(query->next))!=NULL )
		if(!strcmp(pair->name, query->class))
			break;

	if( pair != NULL )
		return pair->value;
	else
		return NULL;
}

void object_dump(t_object *root) {
	t_bucket_cursor *c;
	t_attrib_pair *pair;

	printf("%s %s\n", root->class, root->name );

	c = bucket_cursor_create(ATTRIBUTE_BUCKET(root->attributes));

	while((pair=bucket_cursor_next(c))!= NULL )
		printf("%s\t%s\n", pair->name, (char*)pair->value);

	bucket_cursor_delete(c);

	printf("Dumping Childs\n");
	printf("==============\n");

	c = bucket_cursor_create(ATTRIBUTE_BUCKET(root->childs));

	while( (pair=bucket_cursor_next(c))!=NULL )
		object_dump(pair->value);

	bucket_cursor_delete(c);
}

void object_add_child(t_object *o, char *name, t_object *child) {
	attribute_add( o->childs, name, child );
}

void object_append_object(t_object *o, t_object *item) {
	t_object *p;

	if(o==NULL)
		return;

	for(p=o; p!=NULL; p=p->next) {
		if( p->next == NULL ){
			p->next=item;
			break;
		}
	}
}

