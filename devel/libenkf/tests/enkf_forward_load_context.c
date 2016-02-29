/*
   Copyright (C) 2016  Statoil ASA, Norway.

   The file 'forward_load_context.c' is part of ERT - Ensemble based Reservoir Tool.

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
#include <ert/util/test_util.h>
#include <ert/enkf/forward_load_context.h>


void test_create() {
  forward_load_context_type * load_context = forward_load_context_alloc( NULL , NULL , NULL );
  test_assert_true( forward_load_context_is_instance( load_context ));
  forward_laod_context_free( load_context );
}



int main(int argc , char ** argv) {
  test_create();
  exit(0);
}
