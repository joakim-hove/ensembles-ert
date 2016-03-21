/*
   Copyright (C) 2016  Statoil ASA, Norway.

   The file 'forward_load_context.c.h' is part of ERT - Ensemble based Reservoir Tool.

   ERT is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   ERT is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.

   See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html>
   for more details.
*/

#include <ert/util/type_macros.h>
#include <ert/util/stringlist.h>

#include <ert/enkf/forward_load_context.h>
#include <ert/enkf/run_arg.h>


#define FORWARD_LOAD_CONTEXT_TYPE_ID 644239127

struct forward_load_context_struct {
  UTIL_TYPE_ID_DECLARATION;
  const ecl_sum_type  * ecl_sum;
  ecl_file_type       * restart_file;  // This is set and cleared by calling scope.
  const run_arg_type * run_arg;
  int step1;
  int step2;
  stringlist_type * messages;          // This is managed by external scope - can be NULL


  /* The variables below are updated during the load process. */
  int load_step;
  int load_result;
};

UTIL_IS_INSTANCE_FUNCTION( forward_load_context , FORWARD_LOAD_CONTEXT_TYPE_ID)

forward_load_context_type * forward_load_context_alloc( const run_arg_type * run_arg , const ecl_sum_type * ecl_sum, stringlist_type * messages) {
  forward_load_context_type * load_context = util_malloc( sizeof * load_context );
  UTIL_TYPE_ID_INIT( load_context , FORWARD_LOAD_CONTEXT_TYPE_ID );

  load_context->ecl_sum = ecl_sum; // Could maybe let the load context load the eclipse files?
  load_context->restart_file = NULL;
  load_context->run_arg = run_arg;
  load_context->load_result = 0;
  load_context->messages = messages;

  return load_context;
}



bool forward_load_context_accept_messages( const forward_load_context_type * load_context ) {
  if (load_context->messages)
    return true;
  else
    return false;
}


/*
  The messages can be NULL; in which case the message is completely ignored.
*/

void forward_load_context_add_message( forward_load_context_type * load_context , const char * message ) {
  if (load_context->messages)
    stringlist_append_copy( load_context->messages , message );
}


int forward_load_context_get_result( const forward_load_context_type * load_context ) {
  return load_context->load_result;
}

void forward_load_context_update_result( forward_load_context_type * load_context , int flags) {
  load_context->load_result |= flags;
}


void forward_load_context_free( forward_load_context_type * load_context ) {
  if (load_context->restart_file)
    ecl_file_close( load_context->restart_file );
  free( load_context );
}

bool forward_load_context_load_restart_file( forward_load_context_type * load_context ,
					     const char * ecl_base ,
					     bool fmt_file ,
					     int report_step) {

  char * filename = ecl_util_alloc_exfilename( run_arg_get_runpath(load_context->run_arg) ,
					       ecl_base ,
					       ECL_RESTART_FILE ,
					       fmt_file ,
					       report_step);

  if (load_context->restart_file)
    ecl_file_close( load_context->restart_file );
  load_context->restart_file = NULL;

  if (filename) {
    load_context->restart_file = ecl_file_open( filename , 0 );
    free(filename);
  }

  if (load_context->restart_file)
    return true;
  else
    return false;
}




const ecl_sum_type * forward_load_context_get_ecl_sum( const forward_load_context_type * load_context) {
  return load_context->ecl_sum;
}

const ecl_file_type * forward_load_context_get_restart_file( const forward_load_context_type * load_context) {
  return load_context->restart_file;
}

const run_arg_type * forward_load_context_get_run_arg( const forward_load_context_type * load_context ) {
  return load_context->run_arg;
}

const char * forward_load_context_get_run_path( const forward_load_context_type * load_context ) {
  return run_arg_get_runpath( load_context->run_arg );
}


enkf_fs_type * forward_load_context_get_result_fs( const forward_load_context_type * load_context ) {
  return run_arg_get_result_fs( load_context->run_arg );
}


void forward_load_context_select_step( forward_load_context_type * load_context , int report_step) {
  load_context->load_step = report_step;
  if (load_context->restart_file) {
    // Select block
  }
}

int forward_load_context_get_load_step(const forward_load_context_type * load_context) {
  return load_context->load_step;
}




