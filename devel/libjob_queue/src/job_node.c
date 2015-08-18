/*
   Copyright (C) 2015  Statoil ASA, Norway.

   The file 'job_node.c' is part of ERT - Ensemble based Reservoir Tool.

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

#define  _GNU_SOURCE   /* Must define this to get access to pthread_rwlock_t */
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include <ert/util/msg.h>
#include <ert/util/util.h>
#include <ert/util/thread_pool.h>
#include <ert/util/arg_pack.h>
#include <ert/util/type_macros.h>

#include <ert/job_queue/job_node.h>

#define JOB_QUEUE_NODE_TYPE_ID 3315299
#define INVALID_QUEUE_INDEX    -999

struct job_queue_node_struct {
  UTIL_TYPE_ID_DECLARATION;
  job_status_type        job_status;      /* The current status of the job. */
  int                    submit_attempt;  /* Which attempt is this ... */
  int                    num_cpu;         /* How many cpu's will this job need - the driver is free to ignore if not relevant. */
  int                    queue_index;
  char                  *run_cmd;         /* The path to the actual executable. */
  char                  *exit_file;       /* The queue will look for the occurence of this file to detect a failure. */
  char                  *ok_file;         /* The queue will look for this file to verify that the job was OK - can be NULL - in which case it is ignored. */
  char                  *job_name;        /* The name of the job. */
  char                  *run_path;        /* Where the job is run - absolute path. */
  /*-----------------------------------------------------------------*/
  char                  *failed_job;      /* Name of the job (in the chain) which has failed. */
  char                  *error_reason;    /* The error message from the failed job. */
  char                  *stderr_capture;
  char                  *stderr_file;     /* Name of the file containing stderr information. */
  /*-----------------------------------------------------------------*/
  pthread_mutex_t        data_mutex;      /* Protecting the access to the job_data pointer. */
  void                  *job_data;        /* Driver specific data about this job - fully handled by the driver. */
  int                    argc;            /* The number of commandline arguments to pass when starting the job. */
  char                 **argv;            /* The commandline arguments. */
  time_t                 submit_time;     /* When was the job added to job_queue - the FIRST TIME. */
  time_t                 sim_start;       /* When did the job change status -> RUNNING - the LAST TIME. */
  time_t                 sim_end ;        /* When did the job finish successfully */

  job_callback_ftype    *done_callback;
  job_callback_ftype    *retry_callback;  /* To determine if job can be retried */
  job_callback_ftype    *exit_callback;   /* Callback to perform any cleanup */
  void                  *callback_arg;
};



void job_queue_node_free_error_info( job_queue_node_type * node ) {
  util_safe_free(node->error_reason);
  util_safe_free(node->stderr_capture);
  util_safe_free(node->stderr_file);
  util_safe_free(node->failed_job);
}



/*
  When the job script has detected failure it will create a "EXIT"
  file in the runpath directory; this function will inspect the EXIT
  file and determine which job has failed, the reason the job script
  has given to fail the job (typically missing TARGET_FILE) and
  capture the stderr from the job.

  The file is XML formatted:

  ------------------------------------------------
  <error>
     <time>HH:MM:SS</time>
     <job> Name of job </job>
     <reason> Reason why the job failed </reason>
     <stderr>
        Capture of stderr from the job, can typically be
        a multiline string.
     </stderr>
  </error>
  ------------------------------------------------

  This format is written by the dump_EXIT_file() function in the
  job_dispatch.py script.
*/

/*
   This extremely half-assed XML "parsing" should of course be kept a
   secret...
*/

static char * __alloc_tag_content( const char * xml_buffer , const char * tag) {
  char * open_tag    = util_alloc_sprintf("<%s>"  , tag);
  char * close_tag   = util_alloc_sprintf("</%s>" , tag);

  char * start_ptr   = strstr( xml_buffer , open_tag );
  char * end_ptr     = strstr( xml_buffer , close_tag );
  char * tag_content = NULL;

  if ((start_ptr != NULL) && (end_ptr != NULL)) {
    int length;
    start_ptr += strlen(open_tag);

    length = end_ptr - start_ptr;
    tag_content = util_alloc_substring_copy( start_ptr , 0 , length );
  }

  free( open_tag );
  free( close_tag );
  return tag_content;
}




/**
   This code is meant to capture which of the jobs has failed; why it
   has failed and the stderr stream of the failing job. Depending on
   the failure circumstances the EXIT file might not be around.
*/

void job_queue_node_fscanf_EXIT( job_queue_node_type * node ) {
  job_queue_node_free_error_info( node );
  if (node->exit_file) {
    if (util_file_exists( node->exit_file )) {
      char * xml_buffer = util_fread_alloc_file_content( node->exit_file, NULL);

      node->failed_job     = __alloc_tag_content( xml_buffer , "job" );
      node->error_reason   = __alloc_tag_content( xml_buffer , "reason" );
      node->stderr_capture = __alloc_tag_content( xml_buffer , "stderr");
      node->stderr_file    = __alloc_tag_content( xml_buffer , "stderr_file");

      free( xml_buffer );
    } else
      node->failed_job = util_alloc_sprintf("EXIT file:%s not found - load failure?" , node->exit_file);
  }
}






UTIL_IS_INSTANCE_FUNCTION( job_queue_node , JOB_QUEUE_NODE_TYPE_ID )
UTIL_SAFE_CAST_FUNCTION( job_queue_node , JOB_QUEUE_NODE_TYPE_ID )



int job_queue_node_get_queue_index( const job_queue_node_type * node ) {
  if (node->queue_index == INVALID_QUEUE_INDEX)
    util_abort("%s: internal error: asked for not-yet-initialized node->queue_index\n",__func__);
  return node->queue_index;
}

void job_queue_node_set_queue_index( job_queue_node_type * node , int queue_index) {
  if (node->queue_index == INVALID_QUEUE_INDEX)
    node->queue_index = queue_index;
  else
    util_abort("%s: internal error: atteeempt to reset queue_index \n",__func__);
}


/*
 The error information is retained even after the job has completed
 completely, so that calling scope can ask for it - that is the
 reason there are separate free() and clear functions for the error related fields.
*/

void job_queue_node_free_data(job_queue_node_type * node) {
  util_safe_free( node->job_name );
  util_safe_free( node->exit_file );
  util_safe_free( node->ok_file );
  util_safe_free( node->run_cmd );
  util_free_stringlist( node->argv , node->argc );
  if (node->callback_arg) {
    arg_pack_free( node->callback_arg );
    node->callback_arg = NULL;
  }

  if (node->job_data != NULL)
    util_abort("%s: internal error - driver spesific job data has not been freed - will leak.\n",__func__);
}


void job_queue_node_free(job_queue_node_type * node) {
  job_queue_node_free_data(node);
  job_queue_node_free_error_info(node);
  util_safe_free(node->run_path);
  free(node);
}


job_status_type job_queue_node_get_status(const job_queue_node_type * node) {
  return node->job_status;
}

void job_queue_node_update_status(job_queue_node_type * node , job_status_type new_status) {
  if (new_status != node->job_status) {
    node->job_status = new_status;

    /*
       We record sim start when the node is in state JOB_QUEUE_WAITING
       to be sure that we do not miss the start time completely for
       very fast jobs which are registered in the state
       JOB_QUEUE_RUNNING.
    */
    if (new_status == JOB_QUEUE_WAITING)
      node->sim_start = time( NULL );

    if (new_status == JOB_QUEUE_RUNNING)
      node->sim_start = time( NULL );

    if (new_status == JOB_QUEUE_SUCCESS)
      node->sim_end = time( NULL );

    if (new_status == JOB_QUEUE_FAILED)
      job_queue_node_fscanf_EXIT( node );

  }
}

/******************************************************************/
/*
   These four functions all require that the caller has aquired the
   data lock before entering.
*/

void * job_queue_node_get_data(const job_queue_node_type * node) {
  return node->job_data;
}

void job_queue_node_free_driver_data( job_queue_node_type * node , queue_driver_type * driver) {
  if (node->job_data != NULL)
    queue_driver_free_job( driver , node->job_data );
  node->job_data = NULL;
}

void job_queue_node_driver_kill( job_queue_node_type * node , queue_driver_type * driver) {
  queue_driver_kill_job( driver , node->job_data );
  job_queue_node_free_driver_data( node , driver );
}

void job_queue_node_update_data( job_queue_node_type * node , void * data) {
  node->job_data = data;
}

/******************************************************************/


void job_queue_node_inc_submit_attempt( job_queue_node_type * node) {
  node->submit_attempt++;
}


void job_queue_node_reset_submit_attempt( job_queue_node_type * node) {
  node->submit_attempt = 0;
}

int job_queue_node_get_submit_attempt( const job_queue_node_type * node) {
  return node->submit_attempt;
}



void job_queue_node_set_num_cpu( job_queue_node_type * node , int num_cpu ) {
  node->num_cpu        = num_cpu;
}



void job_queue_node_init_status_files( job_queue_node_type * node , const char * ok_file , const char * exit_file) {
  if (ok_file)
    node->ok_file = util_alloc_filename(node->run_path , ok_file , NULL);

  if (exit_file)
    node->exit_file = util_alloc_filename(node->run_path , exit_file , NULL);

}


void job_queue_node_init_callbacks( job_queue_node_type * node ,
                                    job_callback_ftype * done_callback,
                                    job_callback_ftype * retry_callback,
                                    job_callback_ftype * exit_callback,
                                    void * callback_arg) {
  node->exit_callback  = exit_callback;
  node->retry_callback = retry_callback;
  node->done_callback  = done_callback;
  node->callback_arg   = callback_arg;
}


job_queue_node_type * job_queue_node_alloc( const char * job_name , const char * run_path , const char * run_cmd , int argc , const char ** argv) {
  if (util_is_directory( run_path )) {
    job_queue_node_type * node = util_malloc(sizeof * node );

    UTIL_TYPE_ID_INIT( node , JOB_QUEUE_NODE_TYPE_ID );

    node->job_name       = util_alloc_string_copy( job_name );
    {
    if (util_is_abs_path(run_path))
      node->run_path = util_alloc_string_copy( run_path );
    else
      node->run_path = util_alloc_realpath( run_path );

    if ( !util_is_directory(node->run_path) )
      util_abort("%s: the run_path: %s does not exist - aborting \n",__func__ , node->run_path);
    }
    node->run_cmd        = util_alloc_string_copy( run_cmd );
    node->argc           = argc;
    node->argv           = util_alloc_stringlist_copy( argv , argc );

    node->job_status     = JOB_QUEUE_NOT_ACTIVE;
    node->num_cpu        = 1;
    node->queue_index    = INVALID_QUEUE_INDEX;
    node->submit_attempt = 0;
    node->job_data       = NULL;                                    /* The allocation is run in single thread mode - we assume. */
    node->sim_start      = 0;
    node->sim_end        = 0;
    node->submit_time    = time( NULL );

    node->exit_callback  = NULL;
    node->retry_callback = NULL;
    node->done_callback  = NULL;
    node->callback_arg   = NULL;

    node->exit_file      = NULL;
    node->ok_file        = NULL;

    node->error_reason   = NULL;
    node->stderr_capture = NULL;
    node->stderr_file    = NULL;
    node->failed_job     = NULL;

    pthread_mutex_init( &node->data_mutex , NULL );
    return node;
  } else
    return NULL;
}



const char * job_queue_node_get_cmd( const job_queue_node_type * node) {
  return node->run_cmd;
}

const char * job_queue_node_get_error_reason( const job_queue_node_type * node) {
  return node->error_reason;
}

const char * job_queue_node_get_stderr_capture( const job_queue_node_type * node) {
  return node->stderr_capture;
}


const char * job_queue_node_get_stderr_file( const job_queue_node_type * node) {
  return node->stderr_file;
}


const char * job_queue_node_get_exit_file( const job_queue_node_type * node) {
  return node->exit_file;
}


const char * job_queue_node_get_ok_file( const job_queue_node_type * node) {
  return node->ok_file;
}



const char * job_queue_node_get_run_path( const job_queue_node_type * node) {
  return node->run_path;
}

const char * job_queue_node_get_name( const job_queue_node_type * node) {
  return node->job_name;
}

const char * job_queue_node_get_failed_job( const job_queue_node_type * node) {
  return node->failed_job;
}

int job_queue_node_get_argc( const job_queue_node_type * node) {
  return node->argc;
}

const char ** job_queue_node_get_argv( const job_queue_node_type * node) {
  return (const char **) node->argv;
}

int job_queue_node_get_num_cpu( const job_queue_node_type * node) {
  return node->num_cpu;
}

time_t job_queue_node_get_sim_start( const job_queue_node_type * node ) {
  return node->sim_start;
}


time_t job_queue_node_get_sim_end( const job_queue_node_type * node ) {
  return node->sim_end;
}

time_t job_queue_node_get_submit_time( const job_queue_node_type * node ) {
  return node->submit_time;
}


void job_queue_node_get_data_lock( job_queue_node_type * node) {
  pthread_mutex_lock( &node->data_mutex );
}


void job_queue_node_unlock( job_queue_node_type * node) {
  pthread_mutex_unlock( &node->data_mutex );
}


bool job_queue_node_run_DONE_callback( job_queue_node_type * node ) {
  bool OK = true;
  if (node->done_callback)
    OK = node->done_callback( node->callback_arg );

  return OK;
}


bool job_queue_node_run_RETRY_callback( job_queue_node_type * node ) {
  bool retry = false;
  if (node->retry_callback)
    retry = node->retry_callback( node->callback_arg );

  return retry;
}


void job_queue_node_run_EXIT_callback( job_queue_node_type * node ) {
  if (node->exit_callback)
    node->exit_callback( node->callback_arg );
}
