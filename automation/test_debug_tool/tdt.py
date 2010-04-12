#!/usr/bin/python

import sys, os, commands
from bisect import Bisect
from initRepo import Init, Fetch
from compile import Compile
from binaryManager import BinaryManager
from environment import Environment

if __name__ == '__main__':

        if len(sys.argv) < 2:
                sys.exit("Usage: python tdt.py $configuration")
	
	configuration=sys.argv[1]
       # Initialize the repository by using a predifined enviroment    
#	myRepo = Init('WORKING_DIR')
#	myRepo.gitInit('WORKING_DIR', 'TDT')
#	newBranch = Fetch(configuration)
#	newBranch.gitFetch()
	
	#TODO: Create a module environment.
        environment = Environment(configuration)
        binaryManager = BinaryManager(environment)
#	kernel = Compile( \
#                         environment.getGitBaseURL(), \
#                         environment.getDefConfig(), \
#                         "uImage");
#        kernel.runCompilation()

	print "Functionality TDB"
