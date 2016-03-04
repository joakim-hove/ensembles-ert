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
 
 
  /* The variables below are updated during the load process. */
  int load_step;
  int load_result;
};

UTIL_IS_INSTANCE_FUNCTION( forward_load_context , FORWARD_LOAD_CONTEXT_TYPE_ID)

forward_load_context_type * forward_load_context_alloc( const run_arg_type * run_arg , const ecl_sum_type * ecl_sum) {
  forward_load_context_type * load_context = util_malloc( sizeof * load_context );
  UTIL_TYPE_ID_INIT( load_context , FORWARD_LOAD_CONTEXT_TYPE_ID );

  load_context->ecl_sum = ecl_sum; // Could maybe let the load context load the eclipse files?
  load_context->restart_file = NULL;
  load_context->run_arg = run_arg;

  return load_context;
}

void forward_load_context_free( forward_load_context_type * load_context ) {
  free( load_context );
}


void forward_load_context_set_restart_file( forward_load_context_type * load_context , ecl_file_type * restart_file ) {
  load_context->restart_file = restart_file;
}

void forward_load_context_clear_restart_file( forward_load_context_type * load_context ) {
  load_context->restart_file = NULL;
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
  return run_arg_get_result_fs( load_context );
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




