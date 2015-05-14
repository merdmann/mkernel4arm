/***************************************************************************
 *  $Id: parser.h 219 2008-10-12 17:23:57Z merdmann $
 *
 *  Aug 29, 2008 6:40:06 PM
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

#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>
#include <setjmp.h>

#include "scanner.h"
#include "objects.h"

typedef struct {
	char *filename;
	t_lexer *lex;
	t_token last_token;
	char    *last_value;
	char    *current;
	t_token token;
	jmp_buf on_error;

	t_object *application;
} t_parser;

t_parser *create_parser( char *name);
void delete_parser(t_parser *p);


#endif /* PARSER_H_ */
