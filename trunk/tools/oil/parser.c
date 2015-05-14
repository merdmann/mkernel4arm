/***************************************************************************
 *  $Id: parser.c 273 2011-03-03 21:03:19Z merdmann $
 *
 *  Aug 29, 2008 6:40:06 PM
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
#include <setjmp.h>

#include "oil.h"
#include "parser.h"
#include "objects.h"
#include "bucket.h"

/* the list of allowed object names */
static char *object_names[] = { "OS", "TASK", "COUNTER", "ALARM", "RESOURCE", "EVENT", "ISR",
		"MESSAGE", "COM", "NM", "APPMODE", "IPDU"
};

static char *oil_attribute_list[] = { "DEVICE", 	"STATUS", "SCHEDULERTYPE", "STARTUPHOOK",
		"TIMERS", "PRIORITY", "STACKSIZE", "SCHEDULE", "AUTOSTART", "MASK",
		"MAXALLOWEDVALUE", "TICKSPERBASE", "COUNTER", "ALARMTIME", "CYCLETIME", "MINCYCLE", "ACTION",
		"ALARMCALLBACK", "ALARMCALLBACKNAME", "TASK", "EVENT", "PRETASKHOOK", "POSTTASKHOOK",
		"SCHEDULEMODE",
		NULL
};

t_bucket *oil_tasks = NULL;
t_bucket *oil_appmodes = NULL;
t_bucket *oil_events = NULL;

/*
 * Create a parser instance for a given file.
 */
t_parser *create_parser( char *name) {
    t_parser *result = (t_parser*) malloc( sizeof(t_parser) );

    if( (result->lex = create_lexer(name, (void*) result)) == NULL ) {
	free(result);
	return NULL;
    }

    result->token = e_null;
    result->last_token = e_null;
    result->current = NULL;
    result->filename = strdup(name);
    result->application = NULL;

    return result;
}

/*
 * Delete a parser instance
 */
void delete_parser(t_parser *p) {
    if( p->lex != NULL )
	free(p->lex);
}

/*
 * get the current token
 */
static char *current( t_parser *p) {
    return p->current;
}

/*
 * compare the value of the current token
 */
static int is_current(t_parser *p, char *s) {
    return( !strcmp( current(p), s));
}

/* check the veilidty of an item */
static int valid_item( char *s, char *list[]) {
    int i;

    for(i=0; list[i] != NULL; ++i )
	if(!strcmp(s,list[i]))
	    return TRUE;

    return FALSE;
}


static char *str_append(char *s1, char *s2) {
    char *result;

    if( (result = malloc( strlen(s1)+strlen(s2)+1)) == NULL )
	return NULL;

    strcpy(result, s1);
    strcat(result, s2);

    return result;
}

/*
 * Display the error messages
 */
void error(t_parser *p, char *msg) {
     char tmp[MSG_SIZE];

     sprintf(tmp,"Error %s:%d: %s\n", p->filename, line_number(p->lex), msg);
     alert(tmp);
     longjmp( p->on_error, 1 );
}

/*
 * A wrapper for the next_token primitive in order to implement
 * a push back storage for one token and its value.
 */
static t_token get_token( t_parser *p ) {
    t_token result;

    if(p->current != NULL) {
	free(p->current);
	p->current = NULL;
    }

    if( p->last_token != e_null ) {
	p->token = p->last_token;
	p->current = p->last_value;

	p->last_value = NULL;
	p->last_token = e_null;
    }
    else {
	p->token = next_token(p->lex);
	p->current = strdup( glyph(p->lex) );
    }

    //printf("get_token %d <%s>\n", p->token, p->current == NULL ? "": p->current);
    return p->token;
}

/*
 * Last token will be saved, which means the next get_token will
 * return the saved value,
 */
static t_token unget_token( t_parser *p) {
    p->last_token = p->token;
    if( p->current != NULL)
	p->last_value = strdup(p->current);
    else
	p->last_value = NULL;
}

static int oil_object_table_add(t_bucket *p, char *name) {
    t_bucket_cursor *c = bucket_cursor_create(p);
    char *id;

    while((id=bucket_cursor_next(c))!=NULL)
        if( !strcmp(name,id) )
	    break;

    bucket_cursor_delete(c);

    if( id == NULL ) {
	bucket_append(p, name);
	return TRUE;
    }

    return FALSE;
}


int oil_object_table_size(t_bucket *p) {
    t_bucket_cursor *c = bucket_cursor_create(p);
    char *id;
    int result;

    for(result=0; (id=bucket_cursor_next(c))!=NULL; ++result );
    bucket_cursor_delete(c);

    return result;
}

/*
 * Check whether the object defintion is valid.
 */
static char *oil_object_check( char *object, char *name ) {
	char msg[MSG_SIZE];

	if(oil_tasks == NULL )
		oil_tasks = bucket_create();

	if( !oil_object_table_add(oil_tasks, name ) ) {
		sprintf(msg,"*** error: Instance %s of type %s already defined", name, object );
		return strdup(msg);
	}

	return NULL;
}

/*
 * Read a token and complain if it is not the expected value.
 */
static void expect(t_parser *p, t_token token, char *s) {
	t_token tmp;
	char msg[MSG_SIZE];

	tmp = get_token(p);

	if( tmp == e_string || tmp == e_identifier ) {
		if( !strcmp(current(p),s) ) {
			sprintf( msg, "expected '%s', found '%s'\n", s, tmp);
			error(p, msg);
		}
		return;
	}

	if( token != tmp ) {
		sprintf( msg, "expected token %s\n", tmp);
		error(p, msg);
	}
}

static void add_attribute(t_object *o, char *attr_name, t_object *attr_value) {
	/* for cases like EVENT attribute we append the different values */
	t_object *attrib;

	if( (attrib=object_get_attribute(o,attr_name)) == NULL )
		object_add_attribute(o, attr_name, attr_value);
	else
		object_append_object(attrib,attr_value);
}

/*
 * <description> ::=
 *       <empty>
 *     | ":" <string>
 */
static void oil_description( t_parser *p) {
	t_token token;

	if((token = get_token(p))==e_semicolon) {
		unget_token(p);
		return;
	}

	if((token = get_token(p))!=e_string) {
		char msg[MSG_SIZE];
		sprintf(msg, "unexpected token %s in description clause", token_string(token));
		error(p, msg);
	}
}

/*
 * <OIL_version> ::=
 *     "OIL_VERSION" "=" <version> <description> ";"
 * <version> ::= <string>
 */
void oil_version( t_parser *p) {
	t_token token;

	expect(p, e_equal, "");

	if((token = get_token(p))!= e_string ) {
		char msg[MSG_SIZE];

		sprintf(msg, "unexpected token %s in OIL_VERSION clause", token_string(token));
		error(p, msg);
		return;
	}

	//printf("*** OIL Version <%s>\n", current(p));

	oil_description(p);
}


/*
 * <implementation_definition> ::=
 *     "IMPLEMENTATION" <name> "{" <implementation_spec_list> "}"
 *     <description> ";"
 */
static void oil_implementation(t_parser *p) {
	t_token token;

	token = get_token(p);
}

static t_object *attribute_value(t_parser *p, t_object *parent, char *attrib);

/*
 * Check whether the attribute is valid at all.
 *
 * TODO:
 */
static int attribute_valid(char *s) {
	int i;

	for(i=0; oil_attribute_list[i]!=NULL; ++i)
		if( !strcmp(oil_attribute_list[i], s) )
			return TRUE;

	return FALSE;
}
/* <parameter> ::=
 *     <attribute_name> "=" <attribute_value> <description> ";"
 */
static void oil_parameter(t_parser *p, t_object *o) {
	t_token token;
	char *attr_name;
	t_object *attr_value;
	t_object *attrib;

	if( (token = get_token(p)) != e_identifier ) {
		char msg[MSG_SIZE];

		sprintf(msg, "ERROR: attribute name expected in parameter clause; found %d", token);
		alert(msg);
	}

	attr_name = strdup(current(p));

    if( !attribute_valid(attr_name) ) {
		char msg[MSG_SIZE];

		sprintf(msg, "unknown attribute name %s\n", attr_name);
		alert(msg);
    }

	expect(p, e_equal, "");

	attr_value = attribute_value(p, o, attr_name);

	oil_description(p);

	add_attribute( o, attr_name, attr_value );
}

/*
 * <attribute_value> ::=
 *     <name>
 *     | <name> "{" <parameter_list> "}"
 *     | <boolean>
 *     | <boolean> "{" <parameter_list> "}"
 *     | <number>
 *     | <float>
 *     | <string>
 *     | "AUTO"
 */
static t_object *attribute_value(t_parser *p, t_object *parent, char *attrib) {
	t_object *o;
	t_token token;
	char *name;

	token = get_token(p);

	switch( token ) {
		case e_string:
		case e_number:
			o =object_create(parent->name,current(p), parent);
			break;

		case e_identifier:
			o = object_create(parent->name, current(p), parent);
			//printf(">>>>> %s=%s\n", attrib, o->name );
			if((token=get_token(p))==e_begin) {
				//printf(">>>>> %s.%s.%s\n", parent->name, attrib, o->name );
				while( (token=get_token(p))!=e_end ) {
					unget_token(p);

					oil_parameter(p, o);
					expect(p, e_semicolon, "");
				}
				//printf("<<<<<\n");
			}
			else
				unget_token(p);


	}
	return o;
}

/*
 * <object_definition> ::=
 *     <object_name> <description> ";"
 *     | <object_name> "{" <parameter_list> "}" <description> ";"
 */
static t_object* oil_object_definition(t_parser *p) {
	t_token token;
	char *object;
	char *name;
	char *msg;
	t_object *o;

	token = get_token(p);

	/* handle an empty definition */
	if( token == e_end ) {
		unget_token(p);
		return;
	}

	/* get the object class */
	if( token != e_identifier )
		error(p, "object expected in object definition clause");
	object = strdup( current(p) );
	if(!valid_item(object,object_names))
		error(p, "invalid object name in object definition clause");

	/* name of the object */
	if( (token = get_token(p)) != e_identifier )
		error(p, "object name expected in object definition clause");

	name = strdup( current(p) );

	if( (msg=oil_object_check( object, name )) == NULL ) {
		o = object_create( object, name, NULL);
		object_add_child( p->application, object, o);

		add_attribute(p->application, object, o);
	}
	else
		error(p, msg);


	/* get list of parameters */
	if( (token = get_token(p)) == e_begin ) {
		while(token != e_end) {
			token = get_token(p);

			if( token == e_end )
				break;

			unget_token(p);
			oil_parameter(p, o);
			expect(p, e_semicolon, "");
		}
	}
	else
		unget_token(p);

	oil_description(p);

	return o;
}

/*
 * <application_definition> ::=
 *     "CPU" <name> "{" <object_definition_list> "}" <description> ";"
 *
 * <object_definition_list> ::=
 *       <empty definition>
 *     | <object_definition>
 *     | <object_definition_list> <object_definition>
 *
 */
static t_object *oil_application_definition(t_parser *p) {
	t_token token;
	char msg[MSG_SIZE];
	char *app_name;

	if( (token=get_token(p)) != e_identifier )
		error(p, "Name of Application definition expected");

	app_name = strdup(current(p));
	p->application = object_create("CPU", app_name, NULL);

	//printf("processing application '%s' \n", app_name );

	expect(p, e_begin, "" );

	/* process the object definition list */
	while(token != e_end ) {
		oil_object_definition(p);

		if( (token = get_token(p))==e_eof ) {
			sprintf(msg,"unexpected eof in object definition list of %s", app_name);
			error(p, msg);
		}

		if( token != e_semicolon && token != e_end ) {
			sprintf(msg,"unexpected token %s object definition list of %s",
					   token_string(token), app_name);
			error(p,msg);
		}
	}
	return p->application;
}

/* <file> ::=
 *     <OIL_version>
 *     <implementation_definition>
 *     <application_definition>
 */
int parse_file(t_parser *p) {
	t_token token;
	int result = OIL_OK;

	document = object_create( "", "", NULL );

	while( (token = get_token(p)) != e_eof ) {
		if(setjmp( p->on_error ) ) {
			result = OIL_ERROR;
			break;
		}

		if (token==e_identifier) {
			if( is_current(p, "OIL_VERSION") )
				oil_version(p);
			else if( is_current(p, "IMPLEMENTATION"))
				oil_implementation(p);
			else if( is_current(p, "CPU")) {
				t_object *cpu;

				cpu = oil_application_definition(p);

				add_attribute( document, "CPU", cpu);
			}
			else {
				char msg[MSG_SIZE];
				sprintf(msg, "Unexpected token %s", token_string(token));

				error(p, msg);
				continue;
			}
			expect(p, e_semicolon, "");
		}
		else {
			char msg[MSG_SIZE];
			sprintf(msg, "Unexpected token %s, identifier expected", token_string(token));

			error(p, msg);
		}
	}

	return result;
}

