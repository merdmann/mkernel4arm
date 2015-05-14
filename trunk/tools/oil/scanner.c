/***************************************************************************
 *  $Id$
 *
 *  Aug 29, 2008 6:43:00 PM
 *  Copyright 2008 Michael Erdmann
 *  Email:  Michael.Erdmann@snafu.de
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

 /*
  * This the lexical scanner for the oil language as specified in chapter 5 of
  * the OSEK/VDX document:
  *
  * System Generation OIL: OSEK Implementation Language
  * Version 2.5 July 1, 2004
  *
  */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "scanner.h"
#include "parser.h"
#include "bucket.h"

  static t_bucket *include_path;

  static char *token_representations[] = {
  	"<identifier>", "=", ";", "<number>", "<string>",
  	"{", "}", "(", ")", "<eof>", "<comment>"
  };


  char *token_string( t_token token ){
  	return token_representations[token-1];
  }

/*
 * create an lexical scanner for a given file
 */
 t_lexer *create_lexer( char *name, void *user ) {
 	t_lexer *result;

 	result = malloc( sizeof(t_lexer));

 	if( (result->fp=fopen(name,"r")) == NULL) {
 		free(result);
 		return NULL;
 	}

 	result->token = e_null;
 	result->current = NULL;
 	result->line_nbr = 1;
 	result->char_pos = 0;
 	result->name = strdup(name);
 	result->user = user;
 	result->tos = 0;
 	
 	return result;
 }

/*
 * Delete the lexical scanner
 */
 void delete_lexer( t_lexer *lex ) {
 	fclose( lex->fp);

 	if(lex->current != NULL )
 		free(lex->current);

 	free(lex);
 }

/*
 * Get the line number which is currently under processing
 */
 unsigned line_number( t_lexer *lex) {
 	return lex->line_nbr;
 }

 char *glyph( t_lexer *lex ) {
 	return lex->current == NULL ? "" : lex->current;
 }

 void include_file_path(char *path) {
 	if( include_path == NULL )
 		include_path = bucket_create();

 	bucket_append( include_path, path );
 }

 static FILE *include_file_open(char *name) {
 	FILE *result = NULL;
 	t_bucket_cursor *c = bucket_cursor_create(include_path);
 	char *path;

 	while((path=bucket_cursor_next(c))!=NULL) {
 		char filename[1024];

 		strcat( strcpy( filename, path), "/" );
 		strcat(filename, name );

 		if( (result=fopen(filename, "r"))!=NULL)
 			break;
 	}
 	bucket_cursor_delete(c);

 	return result;
 }

/*
 * next_token
 *
 */

 t_token next_token( t_lexer *lex ) {
 	char     ch;
 	char 	 s[255];
 	unsigned l;

 	l = 0;
 	lex->token = e_null;

 	if(lex->current != NULL ) {
 		free(lex->current);
 		lex->current = NULL;
 	}

 	while(1) {
 		if( (ch=getc(lex->fp)) == EOF ) {
 			if( lex->tos > 0 ) {
 				fclose(lex->fp);

 				--lex->tos;
 				lex->fp = lex->inc[lex->tos].fp;
 				lex->name = lex->inc[lex->tos].name;
 				lex->line_nbr = lex->inc[lex->tos].line;
 				continue;
 			}
 			break;
 		}

 		if( ch == '\n') {
 			lex->line_nbr++;
 			lex->char_pos = 0;
 		}
 		else
 			lex->char_pos++;

	/* handle include commands */
 		if(lex->char_pos == 1 && ch == '#') {
 			FILE *incfp;
 			char ln[256], cmd[256], arg[256];

 			fgets( ln, 255, lex->fp);
 			sscanf( ln, "%s %s", &cmd, &arg );

 			if(!strcmp(cmd,"include")) {
 				if( (incfp = include_file_open( arg )) == NULL)
 					error((t_parser*)lex->user, "Include File not found");

 				if( lex->tos < MAX_INCLUDE_DEPTH ) {
 					lex->inc[lex->tos].fp = lex->fp;
 					lex->inc[lex->tos].name = lex->name;
 					lex->inc[lex->tos].line = lex->line_nbr+1;
 				}
 				lex->tos++;
 				lex->fp = incfp;
 				lex->name = strdup(arg);
 				lex->token = e_null;
 				lex->current = NULL;
 				printf("processing %s\n", arg);
 			}
 			else
 				error((t_parser*)lex->user, "Unknown preprocessor directive");
 			continue;
 		}


 		switch( lex->token ) {
 			case e_null:
 			if(isspace(ch))
 				continue;

 			if(isdigit(ch)) {
 				lex->token = e_number;
 				s[l++] = ch;
 				continue;
 			}
 			else if( isalpha(ch) ) {
 				lex->token = e_identifier;
 				s[l++] = ch;
 				continue;
 			}
 			else if( ch == '"' ) {
 				lex->token = e_string;
 				continue;
 			}
 			else if( ch == '=' )
 				lex->token = e_equal;
 			else if( ch == ';' )
 			lex->token = e_semicolon;
 			else if( ch == '{' )
 				lex->token = e_begin;
 			else if( ch == '}' )
 				lex->token = e_end;
 			else if( ch == '[' )
 				lex->token = e_open_bracket;
 				else if( ch == ']' )
 					lex->token = e_close_bracket;
 				else if( ch == '/' ) {
 					if( (ch = getc(lex->fp)) == '/' )
 						lex->token = e_comment;
 					else
 						ungetc(ch, lex->fp);
 					continue;
 				}
 				else
 					continue;

 				goto exit;

		    /* .......................................................... */
 				case e_number:
 				if( !isdigit(ch)) {
 					ungetc( ch, lex->fp );
 					s[l++] = '\0';

 					lex->current = strdup( s );
 					goto exit;
 				}
 				s[l++] = ch;
 				break;

			/* .......................................................... */
 				case e_identifier:
 				if( !(isalnum(ch) || ch == '_') ) {
 					ungetc( ch, lex->fp );
 					s[l++] = '\0';

 					lex->current = strdup( s );
 					goto exit;
 				}
 				s[l++] = ch;
 				break;

		    /* .......................................................... */
 				case e_string:
 				if( ch == '"' ) {
 					s[l++] ='\0';
 					lex->current = strdup(s);
 					goto exit;
 				}
 				s[l++] = ch;
 				break;

			/* .......................................................... */
 				case e_comment:
 				if( ch == '\n' )
 					lex->token = e_null;

 				default:
 				break;
 			}
 		}

 		lex->token = e_eof;

 		exit:
	//printf("next_token %d <%s>\n", lex->token, lex->current == NULL ? "": lex->current);
 		return lex->token;
 	}
