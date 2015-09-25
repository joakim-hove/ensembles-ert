/*
   Copyright (C) 2015  Statoil ASA, Norway.

   The file 'enkf_python_plugin.c' is part of ERT - Ensemble based Reservoir Tool.

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

#include <ert/util/test_util.h>
#include <ert/enkf/python_plugin.h>


void test_create() {
  python_plugin_type * pp = python_plugin_alloc("file/does/not/exist");
  test_assert_true( python_plugin_is_instance( pp ));
  python_plugin_free( pp );
}


int main( int argc , char ** argv) {
  test_create();
}
