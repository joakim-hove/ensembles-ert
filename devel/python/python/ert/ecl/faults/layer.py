#  Copyright (C) 2014  Statoil ASA, Norway. 
#   
#  The file 'layer.py' is part of ERT - Ensemble based Reservoir Tool. 
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

import ctypes
from ert.cwrap import BaseCClass, CWrapper
from ert.ecl import ECL_LIB

class Layer(BaseCClass):
    
    def __init__(self , nx , ny):
        c_pointer = self.cNamespace().alloc( nx , ny )
        if c_pointer:
            super( Layer , self).__init__(c_pointer)
        else:
            raise ValueError("Invalid input - no Layer object created")


    def __unpackIndex(self , index):
        try:
            (i,j) = index
        except TypeError:
            raise ValueError("Index:%s is invalid - must have two integers" % str(index))
        
        if i < 0 or i >= self.getNX():
            raise ValueError("Invalid layer i:%d" % i)

        if j < 0 or j >= self.getNY():
            raise ValueError("Invalid layer j:%d" % j)
        
        return (i,j)



    def __setitem__(self , index , value):
        (i,j) = self.__unpackIndex(index)
        self.cNamespace().set_cell(self , i , j , value )
        
            
    def __getitem__(self , index):
        (i,j) = self.__unpackIndex(index)
        return self.cNamespace().get_cell(self , i , j)
    
        
    def getNX(self):
        return self.cNamespace().get_nx(self)

    def getNY(self):
        return self.cNamespace().get_ny(self)

    def free(self):
        self.cNamespace().free(self)

    def cellContact(self , p1 , p2):
        i1,j1 = p1
        i2,j2 = p2

        if not 0 <= i1 < self.getNX():
            raise IndexError("Invalid i1:%d" % i1)

        if not 0 <= i2 < self.getNX():
            raise IndexError("Invalid i2:%d" % i2)

        if not 0 <= j1 < self.getNY():
            raise IndexError("Invalid i1:%d" % j1)

        if not 0 <= j2 < self.getNY():
            raise IndexError("Invalid i2:%d" % j2)

        return self.cNamespace().cell_contact(self , i1,j1,i2,j2)
        

    def addInterpBarrier(self , c1 , c2):
        self.cNamespace().add_interp_barrier( self , c1 , c2 )


    
    def addFaultBarrier(self , fault , K , link_segments = True ):
        fault_layer = fault[K]
        num_lines = len(fault_layer)
        print "DEBUG: num_lines: %d fault:%s" % (num_lines , fault.getName())
        for index , fault_line in enumerate(fault_layer):
            for segment in fault_line:
                c1 , c2 = segment.getCorners()
                self.cNamespace().add_barrier(self , c1 , c2)

            if index < num_lines - 1:
                next_line = fault_layer[index + 1]
                next_segment = next_line[0]
                next_c1 , next_c2 = next_segment.getCorners()

                if link_segments:
                    self.addInterpBarrier( c2 , next_c1 )
                    



cwrapper = CWrapper(ECL_LIB)
CWrapper.registerObjectType("layer", Layer)
Layer.cNamespace().alloc        = cwrapper.prototype("c_void_p  layer_alloc(int,  int)")
Layer.cNamespace().free         = cwrapper.prototype("void      layer_free(layer)")
Layer.cNamespace().get_nx       = cwrapper.prototype("int       layer_get_nx(layer)")
Layer.cNamespace().get_ny       = cwrapper.prototype("int       layer_get_ny(layer)")
Layer.cNamespace().set_cell     = cwrapper.prototype("void      layer_iset_cell_value(layer , int , int , int)")
Layer.cNamespace().get_cell     = cwrapper.prototype("int       layer_iget_cell_value(layer , int , int )")
Layer.cNamespace().cell_contact = cwrapper.prototype("bool      layer_cell_contact(layer , int , int , int , int)")
Layer.cNamespace().add_barrier  = cwrapper.prototype("void      layer_add_barrier(layer , int , int)")
Layer.cNamespace().add_interp_barrier = cwrapper.prototype("void  layer_add_interp_barrier(layer , int , int)")
