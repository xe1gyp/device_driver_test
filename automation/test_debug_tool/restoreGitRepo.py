#!/usr/bin/python

# setRepo.py
# Copyright (C) 2010 Leed Aguilar (leed.aguilar@ti.com) and contributors
# This module is part of the Test Debug Tool (TDT) and is released under
# the BSD License: http://www.opensource.org/licenses/bsd-license.php

import commands

class Restore:
	"""
	This class sets the master branch in the git repository
	of the TDT (Test Debug Tool) project as a workaround for
	the following case:

	- When a configuration is intialized by fetching a
	  remote branch and then checking out the same one for
	  building purposes and considering that that this branch
	  remains as the current HEAD, a second initialization for
	  a diferent configuration but that requires  to fetch the
	  same git branch will hang.
	
	- This class provides a simple workaround to checkout the
	  master branch.

	- This module should be called only after the building
	  process is complete.
	"""
	def __init__(self):
		
		self.masterBranch = "master"
		self.masterRefs = "refs/heads/master"
		self.currentHead = "cat .git/HEAD | awk '{print$2}'"

	def initRestore(self):
		
		# Check if current HEAD is master
		if self.currentHead == self.masterRefs:
			print "\n::: we are currently in master branch\n"
		elif self.currentHead == None:
			print "\n::: We are detached from HEAD\n"
			print "\n:::Checking out master branch ....\n"
			status, output = commands.getstatusoutput('git checkout ' + self.masterBranch)
			print output
			print ""
		else:
                        status, output = commands.getstatusoutput('git checkout ' + self.masterBranch)
			print ""
                        print output
                        print ""

if __name__=='__main__':
	
	restore = Restore()
	restore.initRestore()
		
