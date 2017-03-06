import shutil
import os
import os.path
import subprocess
import sys

class SplitRepo(object):

    def __init__(self, org_repo , new_repo):
        self._check_cwd( )
        self._clone( org_repo , new_repo )
        self._tool_path = os.path.dirname( os.path.abspath( __file__ ) )
        
        
    def _check_cwd(self):
        path_list = os.getcwd().split("/") 
        while len(path_list):
            git_path = os.path.join( "/".join( path_list ) , ".git")
            if os.path.isdir(git_path):
                sys.exit("Seems the script is invoked from inside a git repo - that is not allowed!")
        
            path_list.pop( )

        
    def filterBranch( self, path_list):
        rewrite = True
        if "ERT_REPOSPLIT_DRYRUN" in os.environ:
            rewrite = False

        for path in path_list:
            if rewrite:
                cmd = "git filter-branch --tree-filter 'rm -rf %s' --prune-empty -- --all" % path
                print cmd
                os.system( cmd )
            else:
                if os.path.isdir( path ):
                    shutil.rmtree( path )


                    
    def _clone(self, org_repo , new_repo):
        subprocess.check_call(["git", "clone" , org_repo , new_repo])
        os.chdir( new_repo )


    def copyFile(self , src_prefix, rel_path):
        src_file = os.path.join( self._tool_path , src_prefix, rel_path)
        target_file = rel_path

        if os.path.isfile( src_file ):
            shutil.copyfile( src_file , target_file)
        else:
            sys.exit("No such file: %s" % src_file)
