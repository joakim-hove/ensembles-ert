/*
   Copyright (C) 2015  Statoil ASA, Norway.

   The file 'python_plugin.h' is part of ERT - Ensemble based Reservoir Tool.

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

#ifndef __PYTHON_PLUGIN_H__
#define __PYTHON_PLUGIN_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <ert/util/type_macros.h>

  typedef struct python_plugin_struct python_plugin_type;

  python_plugin_type * python_plugin_alloc( const char * source_file );
  void                 python_plugin_free( python_plugin_type * plugin );
  UTIL_IS_INSTANCE_HEADER( python_plugin );

#ifdef __cplusplus
}
#endif
#endif
