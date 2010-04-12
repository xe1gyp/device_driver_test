#!/usr/bin/python

# setRepo.py
# Copyright (C) 2010 Leed Aguilar (leed.aguilar@ti.com) and contributors
# This module is part of the Test Debug Tool (TDT) and is released under
# the BSD License: http://www.opensource.org/licenses/bsd-license.php

import ConfigParser
import commands
import os
import sys
import time

class Init:
	"""
	This class initialize a new git repository
	in a predifined working directory. If the
	repository already exist just return/exit
	"""
	def __init__(self, workingPath):
		"""
		Initialize a newly instanced Init
	
		`workingPath`
		     is the absolute path where the repository
		     will be initialized 
		"""
		
		self.workingPath = os.getenv(workingPath)

		if self.workingPath == None:	
			sys.exit("\nFATAL: "+ self.workingPath +" is not exported\n")

		os.chdir(self.workingPath)

		print("\nMoving the working directory: " + self.workingPath  + " \n")		

	def gitInit(self, workingPath, gitPath):
		"""
		Creates a initial git repository in gitPath. If gitPath
		exists then don't do anything since it assume that .git/
		already is present.
	
		`gitPath`
		    is the relative path to $workingPath where the git
		    repository is initialized
		"""
		filename = "README"
		
		path = os.getenv(gitPath)

		if path == None:
			sys.exit("\nFATAL: "+ gitPath +" is not exported\n")

		dir =  os.path.join(self.workingPath, path)

 		if os.path.exists(dir):
			print("\nGit repository has been already initialized ... skipping\n")
			"""
                	REVISIT: currently we are asumming the "dir" is a clean repository
        	        which can not be true and hence face some problems.
	                """
			os.chdir(dir)
			return
		else:
			try:
				os.makedirs(dir)
			except OSError:
    				pass

	                """
                        REVISIT: currently we are asumming the "dir" is a clean repository
                        which can not be true and hence face some problems.
                        """
			chdir = os.chdir(dir)
			print("\nMoving the workign directory: ")
			print  chdir

			""" Initializing the git repository """
			status, output = commands.getstatusoutput('git init')

			print("\n" + output  + "\n")

			""" Initialize an empty README file to create a master branch """
			FILE = open(filename,"w")
			FILE.close()

			""" Generate the first commit object and hence the master branch """
			commands.getstatusoutput('git add ' + filename)
			commands.getstatusoutput('git commit -m "initial commit" -a')

			"""
			REVISIT: do we want to check that master branch is created?
			'git branch | awk '{print$2}'' will do the trick			
			"""
			print("\nSUCESS: Repository has been initialized\n")

class Fetch:
	"""
	This class will fetch a remote branch
	into a particular git repository
	"""
	def __init__(self, repoIdentifier):
		
                """
                This class reads from a file a particulary git information
                and fetch a predifined remote branch into the repository

                `repoInfo`
                   It is a file which contains the git information of multiple
                   repositories. Each repository is identified by an unique ID

                `repoIdentifier`
                   unique ID (section) for a specific git repository.
                """
		self.repoIdentifier = repoIdentifier

	def gitFetch(self):
		"""
		Fetch a git branch from a remote repository. A configuration
		file must be giving/exported with all the information about
		the remotes repositories. REPO_INFO contains the path to the
		configuration file
		"""
		repoInfo = os.getenv('REPO_INFO')
                if repoInfo == None:
                        sys.exit("\nFATAL: "+ repoInfo +" is not exported\n")

		config = ConfigParser.ConfigParser()
		config.read(repoInfo)

		if not config.has_section(self.repoIdentifier):
			print("\nERR: Please provide correct git Repo Identifier\n")		
			return
		else:
			gitdir = config.get(self.repoIdentifier, 'url_git')
			branch = config.get(self.repoIdentifier, 'branch')
			print("Repo configuration settings for: " + self.repoIdentifier)
			print("URL: " + gitdir)
			print("BRANCH: " + branch)
		
		# Fetch the branch from a given configuration
		commands.getstatusoutput('git fetch ' + gitdir + ' ' +  branch + ':' + branch)
		commands.getstatusoutput('git checkout ' + branch)
	
		
if __name__ == '__main__':
		
		# Initialize the repository by using a predifined enviroment	
		myRepo = Init('WORKING_DIR')
		myRepo.gitInit('WORKING_DIR', 'TDT')
		
		newBranch = Fetch(sys.argv[1])
		newBranch.gitFetch()

