#!/usr/bin/python

# setRevision.py
# Copyright (C) 2010 Leed Aguilar (leed.aguilar@ti.com) and contributors
# This module is part of the Test Debug Tool (TDT) and is released under
# the BSD License: http://www.opensource.org/licenses/bsd-license.php

import os
import commands
import sys

class Rev:
	"""
	This class knows how to detached the current HEAD to checkout
	a valid commit id. Notice that you are no longer on any branch
	"""
	def __init__(self, revision):
	
		self.revision = revision
	
	def setRev(self):

		print "Detaching current HEAD and checking out the following version: " + self.revision
		status, output = commands.getstatusoutput('git checkout ' + self.revision)
		print output
		if status:
			sys.exit("ERROR: Please provid a valid commit ID")

if __name__ == '__main__':

	if len(sys.argv) < 2:
		sys.exit("USAGE: python setRev.py <commit_id>")

	setMyRev = Rev(sys.argv[1])
	setMyRev.setRev()
	
