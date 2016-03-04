/*
   Copyright (C) 2016  Statoil ASA, Norway.

   The file 'forward_load_context.h' is part of ERT - Ensemble based Reservoir Tool.

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

#ifndef __FORWARD_LOAD_CONTEXT_H__
#define __FORWARD_LOAD_CONTEXT_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <ert/util/type_macros.h>

#include <ert/ecl/ecl_sum.h>
#include <ert/ecl/ecl_file.h>

  //#include <ert/enkf/enkf_fs.h>
  //#include <ert/enkf/run_arg.h>

  typedef struct enkf_fs_struct enkf_fs_type; // Forward declaration to avoid circular dependencies.
  typedef struct run_arg_struct run_arg_type; // Forward declaration to avoid circular dependencies.
  typedef struct forward_load_context_struct forward_load_context_type;

  forward_load_context_type * forward_load_context_alloc( const run_arg_type * run_arg , const ecl_sum_type * ecl_sum );
  void                        forward_load_context_free( forward_load_context_type * load_context );
  const ecl_sum_type        * forward_load_context_get_ecl_sum( const forward_load_context_type * load_context);
  const ecl_file_type       * forward_load_context_get_restart_file( const forward_load_context_type * load_context);
  int                         forward_load_context_get_report_step( const forward_load_context_type * load_context);
  int                         forward_load_context_get_iens( const forward_load_context_type * load_context);
  const run_arg_type        * forward_load_context_get_run_arg( const forward_load_context_type * load_context );
  const char                * forward_load_context_get_run_path( const forward_load_context_type * load_context );
  int                         forward_load_context_get_load_step(const forward_load_context_type * load_context);
  enkf_fs_type              * forward_load_context_get_result_fs( const forward_load_context_type * load_context );

  bool                        forward_load_context_load_restart_file( forward_load_context_type * load_context , 
								      const char * ecl_base , 
								      bool fmt_file , 
								      int report_step);

  UTIL_IS_INSTANCE_HEADER( forward_load_context );

#ifdef __cplusplus
}
#endif
#endif
