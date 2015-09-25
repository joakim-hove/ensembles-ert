/*
   Copyright (C) 2015  Statoil ASA, Norway.

   The file 'python_plugin.c' is part of ERT - Ensemble based Reservoir Tool.

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
#include <stdlib.h>

#include <ert/enkf/python_plugin.h>

#define PYTHON_PLUGIN_TYPE_ID 753937007

struct python_plugin_struct {
  UTIL_TYPE_ID_DECLARATION;
  char * source_file;
};


UTIL_IS_INSTANCE_FUNCTION( python_plugin , PYTHON_PLUGIN_TYPE_ID )


python_plugin_type * python_plugin_alloc( const char * source_file ) {
  python_plugin_type * plugin = util_malloc( sizeof * plugin );
  UTIL_TYPE_ID_INIT( plugin , PYTHON_PLUGIN_TYPE_ID);
  plugin->source_file = util_alloc_string_copy( source_file );
  return plugin;
}



void python_plugin_free( python_plugin_type * plugin) {
  free( plugin->source_file );
  free( plugin );
}
