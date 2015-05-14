/****************************************************************************
 *  $Id$
 *
 *  Aug 29, 2008 6:43:00 PM
 *  Copyright (C) 2008 Michael Erdmann
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
#ifndef SCANNER_H_
#define SCANNER_H_

#include <stdio.h>

typedef enum { e_null,
	e_identifier, e_equal, e_semicolon, e_number, e_string,
	e_begin, e_end, e_open_bracket, e_close_bracket, e_eof, e_comment } t_token;

typedef struct {
	char *name;
	FILE *fp;
	unsigned line;
} t_include_file;

#define MAX_INCLUDE_DEPTH 20

typedef struct {
	void *user;
	FILE *fp;
	char *name;				// file name
	t_token token;			// current token
	char *current;			// current string assoc with the token
	unsigned line_nbr;		// line number
	unsigned char_pos;		// char pos. in lines
	unsigned tos;
	t_include_file inc[MAX_INCLUDE_DEPTH];
} t_lexer;

/** this function creates a lexical scanner instance
 *  @param name name of the file to be scanned.
 */
t_lexer *create_lexer( char *name, void *user );

/** delete the lexer which has been created by means of create_lexer.
 *  @param lexer pointer to the lexical scanner.
 */
void delete_lexer( t_lexer *lex);
/** get the current line numer
 *  @param lex  pointer to the lexical scanner
 */
unsigned line_number( t_lexer *lex);

/** get the next token from the scanner.
 *  @param lex pointer to the lexical scanner.
 */
t_token next_token( t_lexer *lex);

char *glyph( t_lexer *lex );

void include_file_path(char *path);

#endif /* SCANNER_H_ */
