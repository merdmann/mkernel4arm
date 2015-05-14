/***************************************************************************
 *  $Id: oil.c 219 2008-10-12 17:23:57Z merdmann $
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

/** @file
 * OIL Compiler for Mkernel
 *
 * @mainpage OIL compiler
 *
 * @section oil_intro Introduction
 *
 * This command is used to generate a system out of a set of system specifications,
 * which means for a given application mode a set of init and include files is
 * generated.
 *
 * The generator takes oil files as input and creates the following files
 * out of it:
 *
 * <prefix>_init.c
 * <prefix>_defs.h
 *
 * The _init.c file provides an initialization procedure which can be called
 * after the application has determined the application mode. Naming convention
 * is:
 *     void Init_Mode(void)
 *
 * The file _defs.h contains all definitions which are needed by the application
 * code:
 *
 * Task_ID_<taskname>  - ID of the task with taskname
 *
 * @section oil_syntax Command line syntax
 *
 * The syntax of the command line interface is:
 *
 * oil [args] file(s)
 *
 * allowed arguments
 *
 * @arg -a name - Application mode to be gnerated
 * @arg -o pref - a prefix to be added to each generated file.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "scanner.h"
#include "parser.h"
#include "scanner.h"
#include "sysgen.h"

#include "oil.h"

int opt_quiet = FALSE;
int opt_verbose = FALSE;

unsigned max_tasks = 4;
unsigned max_alarms = 4;
unsigned event_bits = 32;

void report(char *s) {
	if( opt_verbose )
		printf("%s\n", s);
}

void alert(char *s) {
	fprintf(stderr,"Warning: %s\n", s);
}

void info(char *s) {
	if(opt_quiet)
		return;
	printf("%s\n", s);
}

int main(int argc,char *argv[]) {
	int opt, i;
	char *target_name = NULL;
	char *application_mode = NULL;
	int rc = OIL_OK;

	application_mode = NULL;

	include_file_path( ".");
	include_file_path( "./include");

	while( (opt=getopt(argc,argv,"qvo:a:t:I:e:")) != -1) {
		switch(opt) {
			case 'q':
				opt_quiet = TRUE;
				break;

			case 't':
				max_tasks = atoi( optarg );
				break;

			case 'b':
				max_alarms = atoi( optarg );
				break;

			case 'a':
				application_mode = strdup( optarg );
				break;

			case 'o':
				target_name = strdup( optarg );
				break;

			case 'I':
				include_file_path( optarg );
				break;

			case 'v':
				opt_verbose = TRUE;
				break;

			case 'e':
				event_bits = atoi( optarg );
				break;

			default:
				break;
		}
	}
	if( opt_quiet )
		opt_verbose = FALSE;
	else {
	    fprintf(stderr,"OIL Mkernel System Builder; Version %d.%d.%d\n",
			VERSION_MAJOR, VERSION_MINOR, VERSION_PATCHLEVEL );
	    fprintf(stderr,"Copyright (C) 2008 Michael Erdmann <michael.erdmann@snafu.de>\n");
	    fprintf(stderr,"\n");
	}


	/* load all definition files given in the coomand line */
	rc = OIL_NOTHING;

	for ( i = optind; i < argc; ++i ) {
	    t_parser *p = NULL; 

	    if((p=create_parser( argv[i] ))!= NULL ) {
	    	rc = parse_file(p);
		delete_parser(p);	
		rc = OIL_OK;		
	    }
	    else {
	    	char msg[1024];
	    	sprintf(msg, "File %s not found\n", argv[i]); 
	    	alert(msg);
	    	rc = OIL_ERROR;
	    	break;
	    }
	}
	
	if( rc == OIL_NOTHING ) {
	    alert("Nothing to generate!");
	    rc = OIL_ERROR;	
	}

	if( rc == OIL_OK ) {
	     if( application_mode == NULL ) {
		alert("Application name must be set");
		return OIL_ERROR;
	     }

	     rc = sysgen_generate(target_name, application_mode);
	     info("done.");
	}
	return rc;
}
