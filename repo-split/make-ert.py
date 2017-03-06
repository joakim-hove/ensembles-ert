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

org_repo = "git@github.com:Ensembles/ert"
new_repo = "libert"


        
if len(sys.argv) > 1:
    org_repo = sys.argv[1]

split = SplitRepo( org_repo , new_repo)
split.filterBranch( delete_paths )
split.updateCMake( "cmake/ert/CMakeLists.txt" )
