#!/usr/bin/python

import os
import sys
import time
import commands

from git import *

class Compile:

	def __init__(self, \
		path_of_module, \
		configuration_name, \
		makefile_compile_rule, \
		architecture_type = "arm", \
		compiler_prefix = "arm-none-linux-gnueabi-", \
		makefile_clean_rule = "distclean", \
		clean_action = True):

		self.path_of_module = path_of_module
		self.configuration_name = configuration_name
		self.makefile_compile_rule = makefile_compile_rule
	
		self.architecture_type = architecture_type
		self.compiler_prefix = compiler_prefix
		self.makefile_clean_rule = makefile_clean_rule        

		self.compiler_option = None
		self.architecture_option = None

		self.clean_action = clean_action
		self.list_of_warnings = []

		return None

	def setPath(self, path_of_module):
		"""METHOD Compile.setPath
		Set path of module to compile
		Input: Path of module
		Output: None"""
		self.path_of_module = path_of_module
		return None

	def getPath(self):
		"""METHOD Compile.getPath
		Get path of module to compile
		Input: None
		Output: Path of module"""
		return self.path_of_module

	def setArchitectureType(self, architecture_type):
		"""METHOD Compile.setArchitectureType
		Set architecture type
		Input: Architecture type
		Output: None"""
		self.architecture_type = architecture_type
		self.architecture_option = "ARCH=" + self.architecture_type
		return None

	def getArchitectureType(self):
		"""METHOD Compile.getArchitectureType
		Get architecture type
		Input: None
		Output: Architecture type"""
		return self.architecture_type

	def setCompilerPrefix(self, compiler_prefix):
		"""METHOD Compile.setCompilerPrefix
		Set compiler prefix
		Input: Compiler prefix
		Output: None"""
		self.compiler_prefix = compiler_prefix
		self.cross_compile_option = "CROSS_COMPILE=" + self.compiler_prefix
		return None

	def getCompilerPrefix(self):
		"""METHOD Compile.getCompilerPrefix
		Get compiler prefix
		Input: None
		Output: Compiler prefix"""
		return self.compiler_prefix


	def setConfigurationName(self, configuration_name):
		"""METHOD Compile.setConfigurationName
		Set configuration name (e.g. defconfig)
		Input: Configuration Name
		Output: None"""
		self.configuration_name = configuration_name
		return None

	def getConfigurationName(self):
		"""METHOD Compile.getConfigurationName
		Get configuration name
		Input: None
		Output: Configuration name"""
		return self.configuration_name

	def setMakefileCompileRule(self, makefile_compile_rule):
		"""METHOD Compile.setMakefileCompileRule
		Set Makefile rule to compile
		Input: Makefile compile rule
		Output: None"""
		self.makefile_compile_rule = makefile_compile_rule
		return None

	def getMakefileCompileRule(self):
		"""METHOD Compile.getMakefileCompileRule
		Get Makefile rule to compile
		Input: None
		Output: Makefile compile rule"""
		return self.makefile_compile_rule
            
	def setMakefileCleanRule(self, makefile_clean_rule):
		"""METHOD Compile.setMakefileCleanRule
		Set Makefile rule to clean (e.g distclean)
		Input: Makefile clean rule
		Output: None"""
		self.makefile_clean_rule = makefile_clean_rule
		return None

	def getMakefileCleanRule(self):
		"""METHOD Compile.getMakefileCleanRule
		Get Makefile rule to clean
		Input: None
		Output: Makefile clean rule"""
		return self.makefile_clean_rule

	def setWarningsList(self, warnings):
		"""METHOD Compile.setWarningsList
		Set the list of warnings
		Input: List of warnings
		Output: None"""
		self.list_of_warnings = warnings
		return None

	def getWarningsList(self):
		"""METHOD Compile.getWarningsList
		Get the list of warnings
		Input: None
		Output: List of warnings"""
		return self.list_of_warnings

	def runClean(self):
		"""METHOD Compile.runClean
		Run clean process
		Input: None
		Output: None"""
        
		print "        clean      : " + self.makefile_clean_rule
		os.chdir(self.path_of_module)
		status, output = commands.getstatusoutput( \
							"make -j9 " + \
							self.architecture_option + \
							" " + \
							self.cross_compile_option + \
							" " + \
							self.makefile_clean_rule)
		if status:
			print "\n        Cannot clean, exit script!\n" + output + "\n"
			sys.exit()

	def runConfiguration(self):
		"""METHOD Compile.runConfiguration
		Run configuration process
		Input: None
		Output: None"""

		print "        configure  : " + self.configuration_name
		os.chdir(self.path_of_module)
		status, output = commands.getstatusoutput(\
							"make -j10 " + \
							self.architecture_option + \
							" " + \
							self.cross_compile_option + \
							" " + \
							self.configuration_name)
		if status:
			print "\n        Cannot configure, exit script!\n" + output + "\n"
			sys.exit()

	def runBuild(self):
		"""METHOD Compile.runBuild
		Run build process
		Input: None
		Output: None"""

		print "        build      : " + self.makefile_compile_rule
		os.chdir(self.path_of_module)
		status, output = commands.getstatusoutput( \
							"make -j9 " + \
							self.architecture_option + \
							" " + \
							self.cross_compile_option + \
							" " + \
							self.makefile_compile_rule)

		myfile = open('warnings.output', 'w')
		myfile.writelines(output)
		myfile.close()

		if status:
			print "\n        Cannot compile, exit script!\n"
			self.runWarnings()
			sys.exit()

        

	def runWarnings(self):
		"""METHOD TICompile.removeNonWarnings
		Remove Non warnings
		Input: None
		Output: None"""

		print "        parse      : warnings"
		os.chdir(self.path_of_module)
		myfile = open('warnings.output', 'r')
		fileList = myfile.readlines()
		warnings = []
		print "\n        [ warnings ] \n"
		for lineStr in fileList:
			if lineStr[0] != ' ':
				print "        " + lineStr.rstrip('\n')
				warnings.append(lineStr)
				lineStr = lineStr.rstrip('\n')
		self.setWarningsList(warnings)
		outfile = open('warnings.only', 'w')
		outfile.writelines(warnings)
		outfile.close()
		myfile.close()
		print ""
		return None


	def runCompilation(self):
		"""METHOD Compile.runCompilation
		Run the compilation of the module
		Input: none
		Output: Pass/Fail"""

		print "\n        [ compile ]\n"
		os.chdir(self.path_of_module)

		self.setArchitectureType(self.architecture_type)
		self.setCompilerPrefix(self.compiler_prefix)
		self.setConfigurationName(self.configuration_name)

		self.setMakefileCleanRule(self.makefile_clean_rule)
		self.setMakefileCompileRule(self.makefile_compile_rule)

		if self.clean_action:
			self.runClean()
			self.runConfiguration()
			self.runBuild()
			self.runWarnings()
        


class Branch:

	def __init__(self, url, name, local = True):

		self.url = url
		self.name = name
		self.local = local

		if self.local:
			self.repo = Repo(url);
			if self.repo.is_dirty:
				print "\ndo not know what to do... so please manually save or discard your changes!\n"
				sys.exit(0)
		return None

	def setTree(self, nameTree):
		self.url = nameTree

	def getTree(self):
		return self.url

	def setBranch(self, nameBranch):
		self.name = nameBranch

	def getBranch(self):
		return self.name

	def printHead(self):
		if self.local:
			commits = self.repo.commits(self.name, max_count=1)
			head = commits[0]
			print "path    : " + self.url
			print "branch  : " + self.name
			print "commit  : " + head.id
			print "author  : " + head.author.name + " | " + head.author.email
			print "date    : " + time.strftime("%a, %d %b %Y %H:%M", head.committed_date)
			#print "message : " + head.message
		else:
			print "\nbranch in remote tree, cannot get information!"
		return None

	def checkout(self, source_branch):
		if self.local:
			os.chdir(self.url)
			#print "\nactive branch  : " + self.repo.active_branch
			status, output = commands.getstatusoutput("git checkout -b " + self.name + " " + source_branch)
			if status:
				print "\ncannot create branch, exit script!\n" + output + "\n"
				sys.exit()
			#print "working branch : " + self.repo.active_branch + "\n"
		else:
			print "\nbranch in remote tree, cannot checkout"
		return None

	def pullRemote(self, branch):
		remote_tree = branch.getTree()
		remote_branch = branch.getBranch()
		print "\n  tree     : " + remote_tree
		print "  branch   : " + remote_branch
		os.chdir(self.url)
		status, output = commands.getstatusoutput("git pull " + remote_tree  + " " + remote_branch)
		if status:
			print "    error  : could not merge branch"
			print "\n[ conflicts ] \n"
			print output
#			writer.write(output)
#			writer.close()
			self.git_reset();
		else:
			print "    done   : remote branch has been merged"
		return status

	def git_reset(self):
		if self.local:
			print "\n    reset  : Reset to HEAD to continue"
			status, output = commands.getstatusoutput("git reset --hard HEAD")
			if status:
				print "      error: could not reset, cannot continue!"
				sys.exit()
			else:
				print "             " + output
		else:
			print "\nbranch in remote tree, cannot reset"
		return None

class LogWriter: 
	def __init__(self, stdout, filename): 
		self.stdout = stdout 
		self.logfile = file(filename, 'a') 
	def write(self, text): 
		self.stdout.write(text) 
		self.logfile.write(text) 
	def close(self): 
		self.stdout.close() 
		self.logfile.close() 
writer = LogWriter(sys.stdout, 'L24xFailures.log') 
#sys.stdout = writer 
#print 'test'

# To Do
# check if path exist

def branch_name_create():
	# Create a branch with given name
	date = time.strftime("%Y.%m.%d.%A")
	hour = time.strftime("%H.%M.%S")
	return date + ".daily.build." + hour

def printMergeBanner(banner):
	print "\n\n"
	print "  [[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
	print banner
	print "  ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"

if __name__ == '__main__':

	print "\nExperimental Automatic Merge Tool\n"

	# TODO: Give the git_baseport_url as an argument to don't have it hard coded
	# git_baseport_url = sys.argv[1]
	# git_baseport_branch = "origin/omap4_next"

	git_baseport_url = "/home/richo/kernel-omap4-base"
	git_baseport_branch = "origin/omap4_next"
	print "\ntree     : git://dev.omapzoom.org/pub/scm/santosh/kernel-omap4-base.git\n"
	# Create the name of the new working branch
	working_branch = branch_name_create()

	branch_baseport  = Branch(git_baseport_url, \
				working_branch, local = True)

	branch_tiler_1   = Branch("git://dev.omapzoom.org/pub/scm/dsin/tiler-omap4.git", \
				"dmm_til_1.0", local = False)
#	branch_tiler_2   = Branch("git://dev.omapzoom.org/pub/scm/dsin/tiler-omap4.git", \
#				"tiler_omap4_next", local = False)

	branch_display   = Branch("git://dev.omapzoom.org/pub/scm/axelcx/kernel-display.git", \
				"display-next", local = False)

	branch_syslink_1 = Branch("git://dev.omapzoom.org/pub/scm/tisyslink/kernel-syslink.git", \
				"syslink_next", local = False)
#	branch_syslink_2 = Branch("git://dev.omapzoom.org/pub/scm/tisyslink/kernel-syslink.git", \
#				"tesla-v2.6.33-rc2_24_4", local = False)

	branch_audio     = Branch("git://dev.omapzoom.org/pub/scm/misael/kernel-audio.git", \
				"audio_next", local = False)

	branch_baseport.checkout(git_baseport_branch)
	branch_baseport.printHead()

	kernel = Compile( \
			git_baseport_url, \
			"omap_4430sdp_defconfig", \
			"uImage");
	kernel.runCompilation()

        # tiler tree
	printMergeBanner("  [[[  merge tiler, 1 branch     ]]]")
	status = branch_baseport.pullRemote(branch_tiler_1)
	if not status:
		kernel.runCompilation()
#       status = branch_baseport.pullRemote(branch_tiler_2)
#       if not status:
#               kernel.runCompilation()

        # display tree
	printMergeBanner("  [[[  merge display, 1 branch   ]]]")
	status = branch_baseport.pullRemote(branch_display)
	if not status:
		kernel.runCompilation()

        # syslink tree
	printMergeBanner("  [[[  merge syslink, 1 branch   ]]]")
	status = branch_baseport.pullRemote(branch_syslink_1)
	if not status:
		kernel.runCompilation()
#       status = branch_baseport.pullRemote(branch_syslink_2)
#       if not status:
#               kernel.runCompilation()

	# audio tree
	printMergeBanner("  [[[   merge audio, 1 branch    ]]]")
	status = branch_baseport.pullRemote(branch_audio)
	if not status:
		kernel.runCompilation()

	print "\n"

# End of file
