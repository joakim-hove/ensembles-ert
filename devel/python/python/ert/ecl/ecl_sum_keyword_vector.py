#  Copyright (C) 2011  Statoil ASA, Norway.
#
#  The file 'ecl_sum_keyword_vector.py' is part of ERT - Ensemble based Reservoir Tool.
#
#  ERT is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  ERT is distributed in the hope that it will be useful, but WITHOUT ANY
#  WARRANTY; without even the implied warranty of MERCHANTABILITY or
#  FITNESS FOR A PARTICULAR PURPOSE.
#
#  See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html>
#  for more details.


import numpy
import datetime

# Observe that there is some convention conflict with the C code
# regarding order of arguments: The C code generally takes the time
# index as the first argument and the key/key_index as second
# argument. In the python code this order has been reversed.
from ert.cwrap import BaseCClass, CWrapper
#from ert.util import StringList, CTime, DoubleVector, TimeVector, IntVector
from ert.ecl import ECL_LIB
#from ert.ecl.ecl_sum import EclSum



class EclSumKeyWordVector(BaseCClass):


    def __init__(self, ecl_sum):
        c_pointer = EclSumKeyWordVector.cNamespace().alloc(ecl_sum)
        if c_pointer is None:
            raise AssertionError("Failed to create summary keyword vector")
        else:
            super(EclSumKeyWordVector, self).__init__(c_pointer)

    def free(self):
        EclSumKeyWordVector.cNamespace().free(self)

    def add_keyword(self, keyword):
        EclSumKeyWordVector.cNamespace().add(self, keyword)

    def add_keywords(self, keyword):
        EclSumKeyWordVector.cNamespace().add_multiple(self, keyword)

    def get_size(self):
        return EclSumKeyWordVector.cNamespace().get_size(self)

    def get_node_index(self, index):
        return EclSumKeyWordVector.cNamespace().get_node_index(self, index)

    def get_is_rate(self, index):
        return EclSumKeyWordVector.cNamespace().get_is_rate(self, index)



cwrapper = CWrapper(ECL_LIB)
cwrapper.registerType( "ecl_sum_vector" , EclSumKeyWordVector )
cwrapper.registerType( "ecl_sum_vector_obj" , EclSumKeyWordVector.createPythonObject )
cwrapper.registerType( "ecl_sum_vector_ref" , EclSumKeyWordVector.createCReference )

EclSumKeyWordVector.cNamespace().alloc                  = cwrapper.prototype("c_void_p ecl_sum_vector_alloc(ecl_sum )")
EclSumKeyWordVector.cNamespace().free                   = cwrapper.prototype("void ecl_sum_vector_free(ecl_sum_vector )")
EclSumKeyWordVector.cNamespace().add                    = cwrapper.prototype("void ecl_sum_vector_add_key( ecl_sum_vector ,  char* )")
EclSumKeyWordVector.cNamespace().add_multiple           = cwrapper.prototype("void ecl_sum_vector_add_keys( ecl_sum_vector ,  char* )")
EclSumKeyWordVector.cNamespace().get_size               = cwrapper.prototype("int ecl_sum_vector_get_size( ecl_sum_vector )")
EclSumKeyWordVector.cNamespace().get_node_index         = cwrapper.prototype("int ecl_sum_vector_get_node_index( ecl_sum_vector , int)")
EclSumKeyWordVector.cNamespace().get_is_rate         = cwrapper.prototype("bool ecl_sum_vector_get_is_rate( ecl_sum_vector , int)")