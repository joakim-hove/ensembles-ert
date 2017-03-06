#!/usr/bin/env python
import sys
import os
from split import SplitRepo

delete_paths = ["libconfig", "devel/libconfig",
                "libsched", "devel/libsched",
                "libanalysis", "devel/libanalysis",
                "libjob_queue" , "devel/libjob_queue",
                "libenkf" , "devel/libenkf"]

org_repo = "git@github.com:Ensembles/ert"
new_repo = "libecl"


        
if len(sys.argv) > 1:
    org_repo = sys.argv[1]

split = SplitRepo( org_repo , new_repo)
split.filterBranch( delete_paths )
split.updateCMake( "cmake/ecl/CMakeLists.txt" )
