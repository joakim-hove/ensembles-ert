#!/usr/bin/env python
import sys
import os
from split import SplitRepo

delete_paths = ["libecl",
                "libutil",
                "libecl_well",
                "libgeometry",
                "libeclxx",
                "libutilxx"]

org_repo = "https://github.com/Ensembles/ert"
new_repo = "libert"


        
if len(sys.argv) > 1:
    org_repo = sys.argv[1]

split = SplitRepo( org_repo , new_repo)
split.filterBranch( delete_paths )
split.copyFile( "cmake/ert" , "CMakeLists.txt" )
split.copyFile( "cmake/ert" , "libconfig/src/CMakeLists.txt")
split.copyFile( "cmake/ert" , "libsched/src/CMakeLists.txt")
split.copyFile( "cmake/ert" , "librms/src/CMakeLists.txt")
split.copyFile( "cmake/ert" , "libanalysis/src/CMakeLists.txt")
split.copyFile( "cmake/ert" , "libenkf/src/CMakeLists.txt")
split.copyFile( "cmake/ert" , "libjob_queue/src/CMakeLists.txt")

split.copyFile( "cmake/ert" , "libconfig/tests/CMakeLists.txt")
split.copyFile( "cmake/ert" , "libsched/tests/CMakeLists.txt")
split.copyFile( "cmake/ert" , "librms/tests/CMakeLists.txt")
split.copyFile( "cmake/ert" , "libanalysis/tests/CMakeLists.txt")
split.copyFile( "cmake/ert" , "libanalysis/modules/tests/CMakeLists.txt")
split.copyFile( "cmake/ert" , "libenkf/tests/CMakeLists.txt")
split.copyFile( "cmake/ert" , "libenkf/tests/tests.cmake")
split.copyFile( "cmake/ert" , "libenkf/tests/statoil_tests.cmake")
split.copyFile( "cmake/ert" , "libjob_queue/tests/CMakeLists.txt")
