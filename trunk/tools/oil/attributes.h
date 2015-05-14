/***************************************************************************
 *  $Id: attributes.h 219 2008-10-12 17:23:57Z merdmann $
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
#ifndef _ATTRIBUTES_H_
#define _ATTRIBUTES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bucket.h"

typedef struct {
	char *name;
	void *value;
} t_attrib_pair;


typedef struct  {
	t_bucket *attrs;
} t_attribute_table;


/**
 * create an attribute table
 */
t_attribute_table *attribute_table_create();

/**
 * delete an attribute table. The function assumes that the elements have
 * already been removed from the memory if required.
 *
 * @param t pointer to table
 */
void attribute_table_delete(t_attribute_table *t);

/**
 * add an item to the attribute table
 *
 * @param t pointer to attribute table
 * @param name name of the attribute
 * @param value a pointer refering to anykind of memory instance.
 */
void attribute_add(t_attribute_table *t, char *name, void *value );

/**
 * get an element from the attribute table
 *
 * @param t pointer to the attribute table
 * @param name the name under which the object is expected to be found.
 *
 * The function returns the pointer to the value or NULL if not found.
 */
void *attribute_get(t_attribute_table *t, char *name);

#define ATTRIBUTE_BUCKET(t) ((t)->attrs)

#endif
