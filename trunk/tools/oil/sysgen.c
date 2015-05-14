/***************************************************************************
 *  $Id$
 *
 *  Aug 31, 2008 9:36:08 AM
 *  Copyright 2008 Michael Erdmann
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
  * This is the system generator
  *
  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "bucket.h"
#include "objects.h"
#include "object_names.h"
#include "oil.h"

static const char *id="$id$";

static FILE *cf;
static FILE *hf;
static int alarm_used = FALSE;
static jmp_buf on_error;

static char use_timers = 0;
static unsigned kstack_size = 120;

static void fatal(char *msg) {
	alert(msg);

	longjmp( on_error, 1 );
}

static char *get_attribute(t_object *o, char *name ) {
	t_object *val;

	if((val = object_get_attribute(o,name))==NULL)
		return NULL;

	return val->name;
}

static int is_contained(t_object *o, char *name, char *value ) {
	t_object *attr;

	attr = object_get_attribute( o, name );

	while( attr != NULL) {
		if( !strcmp( attr->name, value ))
			return TRUE;

		attr = object_next_object(attr);
	}

	return FALSE;
}

static int is_true(t_object *o, char *name) {
	char *value;

	if( (value=get_attribute(o,name))==NULL)
		return FALSE;

	return( !strcmp(value,"TRUE") );
}

static t_object *get_object(t_object *app, char *class, char *item) {
	t_object_query *query;
	t_object *result;

	result = NULL;

	if( (query = object_query_create( app, class )) != NULL ) {
		t_object *o;

		while( (o=object_query_next(query))!=NULL ) {
			if( !strcmp( o->name, item ) ) {
				result = o;
				break;
			}
		}
	}
	object_query_delete(query);

	return result;
}


static char *oil_attribute( t_object *o, char *name, char *def) {
	char *value;

	if( (value=get_attribute(o, name)) == NULL )
		return def;
	return value;
}

static t_object *get_task(t_object *app, char *name ) {
	return get_object(app, OBJ_NAME_TASK, name );
}


static t_object *get_event(t_object *app, char *name) {
	return get_object(app, OBJ_NAME_EVENT, name );
}


static int get_counter(t_object *app, char *name, char **base, char  **maxvalue) {
	t_object *result;

	if( (result = get_object(app, OBJ_NAME_COUNTER, name))!= NULL ) {
		*base = get_attribute(result,"TICKSPERBASE");
		*maxvalue = get_attribute(result,"MAXALLOWEDVALUE");
		return 0;
	}
	return -1;
}

static void *sysgen_counter_tables(t_object *app) {
	t_object_query *query;

	fprintf(cf, "/*** all counters ****/\n");
	if( (query = object_query_create( app, OBJ_NAME_COUNTER )) != NULL ) {
		t_object *o;

		while( (o=object_query_next(query))!=NULL ) {
			fprintf( cf, "static CODE AlarmBaseType %s_base = {", o->name );
			fprintf( cf, "0,");
			fprintf( cf, "%s,", oil_attribute(o, "TICKSPERBASE", "0"));
			fprintf( cf, "%s};\n", oil_attribute(o, "MINCYCLE", "0"));

			fprintf( cf, "extern t_counter %s;\n", o->name );
		}
	}
	object_query_delete(query);


	fprintf(cf, "\n");
}


static void *sysgen_counter_init(t_object *app) {
	t_object_query *query;

	fprintf(cf, "/*** all alarms ****/\n");
	if( (query = object_query_create( app, OBJ_NAME_COUNTER )) != NULL ) {
		t_object *o;

		while( (o=object_query_next(query))!=NULL ) {
			use_timers++;
			fprintf( cf, "%s.base=&%s_base;\n", o->name, o->name );
			fprintf( cf, "%s.value=0;\n", o->name);
		}
	}
	object_query_delete(query);

	fprintf(cf, "\n");
}

static void sysgen_prelude( void ) {
	fprintf(cf, "/* DO NOT EDIT */\n");
	fprintf(cf, "#include \"typedefs.h\"\n");
	fprintf(cf, "#include \"kernel.h\"\n");
	fprintf(cf, "#include \"proc.h\"\n");
	fprintf(cf, "#include \"timer.h\"\n");
	fprintf(cf, "#include \"idle.h\"\n");
	fprintf(cf, "#include \"execution.h\"\n");

	fprintf(cf, "\n\n");

	fprintf(cf, "void _oil_device_init(void);\n");
	fprintf(cf, "void _oil_device_intr(void);\n");

	fprintf(cf, "\n\n");
}

/*
 * Locate the first CPU with this application name. Since we do not
 * support multiple CPU's.
 */
static t_object *locate_cpu(char *name) {
	t_object *cpu;

	if( (cpu = object_get_attribute( document, OBJ_NAME_CPU ))==NULL ) {
		alert("No CPU definition in document");
	}

	while(cpu!=NULL) {
		if( is_contained(cpu, OBJ_NAME_APPMODE, name) )
			return cpu;

		cpu = object_next_object(cpu);
	}

	return NULL;
}

static int sysgen_events(t_object *app) {
	t_object_query *query;
	t_object *o;
	char msg[MSG_SIZE];
	long defined_events = 0;

	if( (query = object_query_create( app, OBJ_NAME_EVENT )) == NULL ) {
		return 0;
	}

	while((o=object_query_next(query))!=NULL) {
		char *mask;
		long value;

		mask = get_attribute(o, "MASK");

		/* allocate an event bit dynamically */
		if(!strcmp(mask,"AUTO") ) {
			int i;

			value = 1;
			for(i=0; i<event_bits && (defined_events & (1<<i)) != 0; ++i )
				value = value << 1;

			if(!value)
				fatal("to many events");

		} else
			value = atoi(mask);

		if( !value ) {
			sprintf(msg, "mask value in %s should be greater then 0.", o->name );
			fatal(msg);
		}

		if( value & defined_events ) {
			sprintf(msg, "event value 0x%x already in use for %s", value, o->name);
			fatal(msg);
		}
		defined_events |= value;

		//fprintf(hf,"#define %s %d\n", o->name, value );
		fprintf(cf,"const EventMaskType %s = %d;\n", o->name, value);

	}

	object_query_delete( query );

	return 0;
}

typedef struct {
	char *name;
	char *activations;
	int priority;
	char *autostart;
	char *schedule;
	unsigned stacksize;
} t_task_info;

static int byPrio(const t_task_info *p1, const t_task_info *p2) {
	return p1->priority > p2->priority ? 1 : -1 ;
}

static int sysgen_task_tables(t_object *app) {
	char msg[MSG_SIZE];
	t_object_query *query;
	t_object *o;
	int task_id;
	int i;
	t_task_info t[200];

	if( (query = object_query_create( app, OBJ_NAME_TASK )) == NULL )
		fatal("No tasks defined.");

	for(task_id = 0; (o=object_query_next(query))!=NULL; ++task_id) {
		t[task_id].name = o->name;
		t[task_id].activations = oil_attribute(o,"ACTIVATIONS", "1");
		t[task_id].priority = atoi(oil_attribute(o, "PRIORITY", "55"));
		t[task_id].autostart = oil_attribute(o,"AUTOSTART", "TRUE");
		t[task_id].schedule  = oil_attribute(o, "SCHEDULE", "FULL");
		t[task_id].stacksize = atoi(oil_attribute(o, "STACKSIZE", "96"));

		if( task_id+1 == max_tasks )
			fatal("error to many tasks");
	}
	object_query_delete( query );

	qsort( t, task_id, sizeof(t_task_info), &byPrio);

	for(i=0; i<task_id;++i) {
		//fprintf(cf, "#pragma udata userbank stack_%s\n", t[i].name );
		fprintf(cf, "static dword stack_%s[%d];\n", t[i].name, t[i].stacksize );


		fprintf(cf, "static CODE t_task_descriptor %s_descriptor= {\n", t[i].name );
		fprintf(cf, "\t%s, // activations\n", t[i].activations );
		fprintf(cf, "\t%d, // priority\n", t[i].priority );
		fprintf(cf, "\t%s, // schedule\n", t[i].schedule);
		fprintf(cf, "\t&%s,  // entry\n", t[i].name);
		fprintf(cf, "\t(POINTER)&stack_%s[%d]\n", t[i].name, t[i].stacksize-1);
		fprintf(cf, "};\n");

		fprintf(hf, "#define TASK_ID_%s %i\n", t[i].name, i);
	}

	//fprintf(cf, "#pragma udata userbank idle_stack\n");
	fprintf(cf, "static dword idle_stack[STACK_SIZE_IDLE_TASK];\n");
	fprintf(hf, "#define TASK_ID_IDLE %d\n", task_id);

	fprintf(cf, "CODE t_task_descriptor _os_idle_task_descriptor = {\n");
	fprintf(cf, "	1, 		 				// max_activations\n");
	fprintf(cf, "	99, 					// priority;\n");
	fprintf(cf, "	FULL,					// schedule;\n");
	fprintf(cf, "	Idle_Task,				// entry;\n");
	fprintf(cf, "	(POINTER)&idle_stack[STACK_SIZE_IDLE_TASK-1]\n");
	fprintf(cf, "};\n");

	fprintf(cf, "const int _oil_max_tcb = %d;\n", task_id+1);
    fprintf(cf, "t_tcb _TCB[%d];\n", task_id+1);
	return 0;
}

static int sysgen_task_init(t_object *app) {
	char msg[MSG_SIZE];
	t_object_query *query;
	t_object *o;
	int task_id;

	if( (query = object_query_create( app, OBJ_NAME_TASK )) == NULL )
		fatal("No tasks defined.");

	for(task_id = 0; (o=object_query_next(query))!=NULL; ++task_id) {
		t_object *event;
		char *prio;
		char *schedule;
		char *stacksize;

		if( task_id+1 == max_tasks )
			fatal("error to many tasks");

		fprintf(cf,"/* Task %s */\n", o->name);

		/* startup sequence for each task */
		fprintf(cf,"_os_task_create( TASK_ID_%s, &%s_descriptor);\n",o->name, o->name);

		if( is_true(o, "AUTOSTART") ) {
			fprintf(cf, "ActivateTask(TASK_ID_%s);\n", o->name );
		}

		//fprintf(hf, "#define TASK_ID_%s %d\n", o->name, task_id);
		fprintf(hf, "extern void %s(TASK_ARGUMENT args);\n", o->name);

		/* perform some consitency checks */
		event = object_get_attribute(o, "EVENT");
		while( event !=NULL ) {
			if( get_event(app, event->name) == NULL) {
				sprintf(msg,"undefined event %s in task %s\n", event->name, o->name );
				fatal(msg);
			}
			event = object_next_object(event);
		}
	}
	/* create the idle task */
	fprintf(cf, "_os_task_create( TASK_ID_IDLE, &_os_idle_task_descriptor);\n");
	fprintf(cf, "ActivateTask(TASK_ID_IDLE);\n" );

	object_query_delete( query );

	return 0;
}

static int sysgen_resource(t_object *app) {
	t_object_query *query;
	t_object *o;

	if( (query = object_query_create( app, OBJ_NAME_RESOURCE )) == NULL ) {
		info("No resources  defined.");
		return -1;
	}

	while((o=object_query_next(query))!=NULL) {
		fprintf(cf,"ResourceInstance __near %s = { NO_OWNER, (HANDLE)0 };\n", o->name);
	}

	object_query_delete( query );
}
static int sysgen_os(t_object *app) {
	t_object_query *query;
	t_object *o, *attr;

	if( (query = object_query_create( app, OBJ_NAME_OS )) == NULL ) {
		info("No OS defined.");
		return -1;
	}
	o = object_query_next(query);

	if( is_true(o, "PRETASKHOOK") )
		fprintf(cf,"extern void TaskBeginHook(t_task_id id);\n");
	else
		fprintf(cf,"void TaskBeginHook(t_task_id id) { t_task_id __attribute__((unused)) myId = id; }\n");

	if( is_true(o, "POSTTASKHOOK") )
		fprintf(cf,"extern void TaskBeginHook(t_task_id id);\n");
	else
		fprintf(cf,"void TaskEndHook(t_task_id id) { t_task_id __attribute__((unused)) myId = id;}\n");

	if( !is_true(o, "STARTUPHOOK") )
		fprintf(cf,"void StartupHook(void) { }\n");

	if( !is_true(o, "SHUTDOWNHOOK") )
		fprintf(cf,"void ShutdownHook(void) { }\n");

	if (!strcmp(oil_attribute(o, "SCHEDULERTYPE", "PREEMPTIVE" ), "PREEMPTIVE") )
		fprintf(cf,"BOOL _oil_use_preemption = TRUE;\n");
	else
		fprintf(cf,"BOOL _oil_use_preemption = FALSE;\n");

	kstack_size = atoi( oil_attribute(o,"STACKSIZE", "160") );
	fprintf(cf, "static dword kernel_stack[%d];\n",kstack_size );
	fprintf(cf, "POINTER _os_kernel_stack;\n");

	object_query_delete( query );
}

static int sysgen_device_tables(t_object *app) {
	t_object_query *query;
	t_object *o, *attr;

	if( (query = object_query_create( app, OBJ_NAME_OS )) == NULL ) {
		info("No OS defined.");
		return -1;
	}
	o = object_query_next(query);

	fprintf(cf, "/* list all devices */\n");
	if( (attr= object_get_attribute(o,"DEVICE"))!=NULL ) {
		while( attr != NULL) {
			char *name = attr->name;

			fprintf(cf, "extern CODE BOOL _device_init_%s_handler();\n", name);
			fprintf(cf, "extern CODE BOOL _device_intr_%s_handler();\n", name);

			attr = object_next_object(attr);
		}
	}

	object_query_delete( query );
}

static int sysgen_device_intr(t_object *app) {
	t_object_query *query;
	t_object *o, *attr;

	if( (query = object_query_create( app, OBJ_NAME_OS )) == NULL ) {
		info("No OS defined.");
		return -1;
	}


	fprintf(cf, "void _oil_device_intr(void) {\n");

	if( (attr= object_get_attribute(o,"DEVICE"))!=NULL ) {
		while( attr != NULL) {
			char *name = attr->name;

			fprintf(cf, "    if( _device_intr_%s_handler() ) return;\n", name);

			attr = object_next_object(attr);
		}
	}
	fprintf(cf,"}\n");

	object_query_delete(query);
}

/* generate the device init code */
static int sysgen_device_init(t_object *app) {
	t_object_query *query;
	t_object *o, *attr;

	if( (query = object_query_create( app, OBJ_NAME_OS )) == NULL ) {
		info("No OS defined.");
		return -1;
	}

	o = object_query_next(query);

	fprintf(cf, "void _oil_device_init(void) {\n");

	if( (attr= object_get_attribute(o,"DEVICE"))!=NULL ) {
		while( attr != NULL) {
			char *name = attr->name;

			fprintf(cf, "    _device_init_%s_handler();\n", name);

			attr = object_next_object(attr);
		}
	}
	fprintf(cf,"}\n");

	object_query_delete(query);
}

static void sysgen_alarm_tables(t_object *app) {
	t_object_query *query;
	t_object *o;

	fprintf(cf, "/* alarms */\n");
	if( (query = object_query_create( app, OBJ_NAME_ALARM )) == NULL )
		return;

	while((o=object_query_next(query))!=NULL) {
		t_object *counter = object_get_attribute(o, "COUNTER" );

		fprintf(cf, "t_alarm %s_impl = { 0,0,0,&%s};\n",o->name, counter->name );

		fprintf(hf, "extern t_alarm %s_impl;\n", o->name);
		fprintf(hf, "#define %s &%s_impl\n", o->name, o->name );

	}

	object_query_delete( query );

	fprintf(cf,"\n");
}

static void sysgen_alarm_action(t_object *app, t_object *o) {
	t_object *action;

	if((action = object_get_attribute(o,"ACTION"))==NULL) {
		char msg[MSG_SIZE];

		sprintf(msg, "Action in alarm %s missing; ignored", o->name);
		fatal(msg);
	}
	if( !strcmp(action->name, "SETEVENT"))  {
		char *task_name;
		char *event_name;
		char msg[MSG_SIZE];

		if((task_name = get_attribute(action, "TASK"))==NULL) {
			sprintf(msg,"Task not defined in alarm %s ", o->name);
			fatal(msg);
		}

		if(get_object(app, OBJ_NAME_TASK, task_name)==NULL)  {
			sprintf(msg,"Task %s used in alarm %s not defined", task_name, o->name);
			fatal(msg);
		}

		event_name = get_attribute(action,"EVENT");

		fprintf(cf,"    SetEvent( TASK_ID_%s, %s);\n", task_name, event_name);
	}
	else if(!strcmp( action->name, "ACTIVATETASK")) {
		char *task_name;

		if( (task_name = get_attribute(action,"TASK")) == NULL)
			fatal("task attribute missing in ACTIVATETASK");

		fprintf(cf, "    ActivateTask(TASK_ID_%s);\n", task_name);

	}
	else if(!strcmp( action->name, "ALARMCALLBACK")) {
		char *cb_name;

		if((cb_name=get_attribute(action, "ALARMCALLBACKNAME"))==NULL) {
			char msg[MSG_SIZE];

			sprintf(msg, "call back name in alarm %s missing.", o->name);
			fatal(msg);
		}
		fprintf(hf,"extern void %s(AlarmType AlarmID);\n", cb_name);
		fprintf(cf,"        %s(&%s_impl);\n", cb_name, o->name );
	}
}

static void sysgen_alarm_dispatch(t_object *app, char *counter) {
	t_object_query *query;
	t_object *o;

	if( (query = object_query_create( app, OBJ_NAME_ALARM )) == NULL )
		return;

	while((o=object_query_next(query))!=NULL) {
		char alarm[MSG_SIZE];

		if( strcmp(oil_attribute(o,"COUNTER", ""),counter) )
				continue;

		sprintf(alarm,"%s_impl", o->name );

		/*
		 * The code to be generated to each timer:
		 *
		 * If active and timer.value == timer.expiery then
		 *    ... Action ....
		 *    if cycle /= 0 then
		 *       timer.expiery += timer.cycle;
		 *    else
		 *       deactivate timer;
		 *    end if;
		 * fi
		 */
		fprintf(cf,"           if(%s.active && %s.expire==%s.value) {\n", alarm, alarm, counter );
		fprintf(cf,"              if( %s.cycle != 0 ) \n", alarm );
		fprintf(cf,"                 %s.expire += %s.cycle;\n", alarm);
		fprintf(cf,"              else\n");
		fprintf(cf,"                 %s.active=0;\n", alarm);
		sysgen_alarm_action(app, o);
		fprintf(cf,"           }\n");
	}

	object_query_delete( query );
}


/*
 * Genereate the code the handle alarms
 */
static int sysgen_alarm_handlers(t_object *app) {
	t_object_query *query;
	t_object *o;

	if( (query = object_query_create( app, OBJ_NAME_COUNTER  )) == NULL )
		return 0;

	while((o=object_query_next(query))!=NULL) {
		char *maxvalue = oil_attribute(o, "MAXALLOWEDVALUE", "32000");
		char *tickbase = oil_attribute(o, "TICKSPERBASE", "1");
		char *mincycle = oil_attribute(o, "MINCYCLE", "10");

		t_object_query *alarm_query;

		/*
		 * if counter.ticks+-- == 0 then
		 *      counter.ticks = counter base;
		 *
		 *      if counter.value == counter.maxvalue then
		 *      	counter.value  = 0;
		 *
		 *      	counter.expire = 0;
		 *      end if;
		 *
		 *      dispatch actions;
		 *
		 *      ++counter.value;
		 * end if;
		 */
		fprintf(hf,"void %s_Handler(void);", o->name );
		fprintf(cf,"void %s_Handler(void) {\n", o->name );

		fprintf(cf,"    if( %s.ticks-- ==0 ) {\n", o->name);
		fprintf(cf,"        %s.ticks = %s;\n", o->name, tickbase );
		fprintf(cf,"        if( %s.value == %s ) {\n", o->name, maxvalue );
		fprintf(cf,"           %s.value = 0;\n", o->name );

		/**
		 * Reset for each alarm which refer to this counter
		 */
		if( (alarm_query = object_query_create( app, OBJ_NAME_ALARM )) != NULL ) {
			t_object *o_alarm;
			
			while((o_alarm=object_query_next(alarm_query))!=NULL) {

				if( strcmp(oil_attribute(o_alarm,"COUNTER", ""), o->name) )
					continue;

				fprintf(cf, "           %s_impl.expire = 0;\n", o_alarm->name );
			}
		}

		fprintf(cf,"        }\n");
		sysgen_alarm_dispatch(app, o->name);
		fprintf(cf,"        ++%s.value;\n", o->name );
		fprintf(cf,"    }\n");
		fprintf(cf,"}\n");
	}
	object_query_delete( query );

}


/*
 * Genereate the code the handle alarms
 */
static int sysgen_alarm_list(t_object *app) {
	t_object_query *query;
	t_object *o;

	if( (query = object_query_create( app, OBJ_NAME_COUNTER  )) == NULL )
		return 0;

	fprintf(cf, "t_alarm_handler _oil_alarm_handlers[] = { ");
	while((o=object_query_next(query))!=NULL) 
		fprintf(cf,"%s_Handler,", o->name );

	fprintf( cf, "NULL };");

	object_query_delete( query );
}

/*
 * Genereate the code the handle alarms
 */
static int sysgen_alarm_autostart(t_object *app) {
	t_object_query *query;
	t_object *o;

	fprintf(cf,"/** start all autostart alarms **/\n");
	if( (query = object_query_create( app, OBJ_NAME_ALARM  )) == NULL )
		return 0;

	while((o=object_query_next(query))!=NULL) {
		t_object *autostart;
		char *start, *cycle;

		if( (autostart = object_get_attribute(o,"AUTOSTART"))==NULL )
			continue;

		if(!strcmp(autostart->name,"FALSE"))
			continue;

		start = get_attribute( autostart, "ALARMTIME" );
		cycle = get_attribute( autostart, "CYCLETIME" );


		if( start == NULL  ||  cycle == NULL) {
			char msg[MSG_SIZE];

			sprintf(msg, "ALRMTIME/CYCLETIME missing in alarm %s", o->name );
			fatal(msg);
		}

		fprintf(cf,"%s_impl.cycle=%s;\n", o->name, cycle);
		fprintf(cf,"%s_impl.expire=%s;\n", o->name, start);
		fprintf(cf,"%s_impl.active=TRUE;\n", o->name );
	}

	object_query_delete( query );
}

/*
 * Main entry of the system generator
 */
int sysgen_generate(char *target, char *name) {
	t_object *appl ;
	char *init_file_name;
	char *include_file_name;
	char msg[MSG_SIZE];

	if((appl = locate_cpu(name))==NULL){
		sprintf( msg, "**** ERROR: application mode %s not defined for any CPU object.", name);
		alert(msg);
		return OIL_ERROR;
	}

	sprintf(msg, "%s_init.c", target);
	init_file_name = strdup(msg);

	sprintf(msg, "%s_defs.h", target);
	include_file_name = strdup(msg);

	sprintf( msg, "creating %s and %s.", init_file_name, include_file_name);
	info(msg);

	hf = fopen(include_file_name, "w");
	cf = fopen(init_file_name, "w");

	sysgen_prelude();

	fprintf(cf, "#include \"%s\"\n", include_file_name );
	fprintf(cf, "\n");

	if(setjmp( on_error ) ) {
		fclose(cf);
		fclose(hf);

		unlink(include_file_name );
		unlink(init_file_name);
		return OIL_ERROR;
	}

	sysgen_counter_tables(appl);
	sysgen_alarm_tables(appl);
	sysgen_resource(appl);
	sysgen_events(appl);

	sysgen_device_tables(appl);

	sysgen_task_tables(appl);
	fprintf(cf,"\n");
	sysgen_os(appl);
	fprintf(cf,"\n");

	sysgen_device_init(appl);
	sysgen_device_intr(appl);

	fprintf(hf, "/* Application Modes */\n");
	fprintf(hf, "#define Application_Mode_%s 1\n", name );

	fprintf(cf, "/* Alarmhandlers */\n");
	sysgen_alarm_handlers(appl);
	fprintf(cf, "\n");
	sysgen_alarm_list(appl);	
	fprintf(cf,"\n");
	fprintf(cf, "void _oil_init_application_mode(AppModeType mode) {\n");
        fprintf(cf, "AppModeType myMode __attribute__((unused)) = mode;\n");

	// allocate the kernel stack
    	fprintf(cf, "_os_kernel_stack = (POINTER) &kernel_stack[%d];\n", kstack_size-1 );

	fprintf(cf,"\n");
	sysgen_task_init(appl);

	fprintf(cf,"\n");
	sysgen_counter_init(appl);
	sysgen_alarm_autostart(appl);
	fprintf(cf,"\n");

	if( use_timers )
		fprintf(cf, "_os_initialize_alarm();\n");

	fprintf(cf, "}");

	fprintf(cf,"\n");
	fprintf(hf,"\n");

	fclose(cf);
	fclose(hf);

	free( init_file_name );
	free( include_file_name );

	return 0;
}
