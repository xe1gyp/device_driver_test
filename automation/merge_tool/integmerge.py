#!/usr/bin/python

import os
import sys
import time
import ConfigParser

from git import *
from kernelutils import compile

#Pass this option when git commands are expected to fail
kwargs_expect_errors = { 'with_extended_output': True, 
			 'with_exceptions' : False }

class LogWriter: 
	def __init__(self, stdout, filename): 
		self.stdout = stdout 
		self.logfile = file(filename, 'w') 
        def write(self, text): 
		self.stdout.write(text)
		self.logfile.write(text) 
	def close(self):
		self.stdout.close() 
		self.logfile.close() 

#Logging object. This is global
logging = LogWriter(sys.stdout, 'L24xFailures.log')

#This is needed to indirectly collect build output to 
#the main log file
#TODO fix the compile.py to do configurable logging
def append_build_logs(bdir):
	os.chdir(bdir)
	l = open('warnings.output')
	for x in l.readlines():
		logging.write(x)

def branch_name_create():
	# Create a branch with given name
	date = time.strftime("%Y.%m.%d.%A")
	hour = time.strftime("%H.%M.%S")
	return "daily.build." + date + '.' + hour

def printMergeBanner(banner):
	logging.write("\n\n")
	logging.write("  [[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[\n")
	logging.write('\t' + banner + '\n')
	logging.write("  ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]\n")

def printBuildBanner(banner):
	logging.write("\n\n")
	logging.write("  ******************************\n")
	logging.write('\t' + banner + '\n')
	logging.write("  ******************************\n")

def get_url(mcon, gspec):
	return mcon.get(gspec, 'git_url')

def get_brname(mcon, gspec):
	return mcon.get(gspec, 'git_branch')

def pull_a_remote(onrepo, mcon, gspec):
	git_returns = ()
	r_url = get_url(mcon, gspec)
	r_br = get_brname(mcon, gspec)

	printMergeBanner(r_url + ' : ' + r_br)
	if not onrepo.is_dirty:
		gobj = onrepo.git
		git_returns = gobj.pull(r_url, r_br, \
				**kwargs_expect_errors);
	else:
		git_returns = ( -1, "Error", "Working tree dirty")

	return git_returns

def clean_working_copy(repo):
	gobj = repo.git
	gobj.merge('--abort', **kwargs_expect_errors)
	gobj.reset('--hard', **kwargs_expect_errors)
	gobj.clean('-d', '-f', **kwargs_expect_errors)


def execute_mergeconf(wrepo, mcon):
	gobj = wrepo.git
	if wrepo.is_dirty:
		print "Working tree dirty! provide a clean workspace for merging.."
		exit
	else:
		gobj.pull()

	#Remote branch name base
	rname = 'origin/' + get_brname(mcon, mcon.get('MERGEMANAGER', 'starting_base'))
	
	# Create the name of the new working branch
	working_branch = branch_name_create()
	#Checkout the starting base branch
	pull_results = gobj.checkout(rname, '-b', working_branch, \
			 **kwargs_expect_errors)
	if pull_results[0] == 0:
		pull_results = (0, 'Checkedout out ' + rname + ' as ' + working_branch, ' ')
	yield pull_results

	#Merge list of remote repositories
	for x in mcon.get('MERGEMANAGER', 'merge_targets').split(','):
		yield pull_a_remote(wrepo, mcon, x)

def merge_and_build(config):
        base_dir = config.get('MERGEMANAGER', 'working_dir')
	logging.write('base dir =' + base_dir + '\n')
	wrepo = Repo(base_dir)
	compobjs = []

	#Get ready to build
	if config.get('MERGEMANAGER', 'build_test') == '1':
		print 'reading buildconf'
		for x in config.get('MERGEMANAGER','build_conf').split(','):
			compobjs.append(compile.Compile(base_dir,\
					config.get(x,'buildconfig'),\
					config.get(x,'build_target')))
	stop_on_error = config.get('MERGEMANAGER', 'stop_on_error')
	for pullres in execute_mergeconf(wrepo, config):
		if pullres[0] == 0:
			logging.write('***merge succeeded\n')
			logging.write(pullres[1] + '\n')
			for bld in compobjs:
				printBuildBanner("build: " + bld.getConfigurationName())
				bld.runCompilation()
				append_build_logs(base_dir)
		else:
			logging.write('***Merge failed\n')
			logging.write(pullres[1] + '\n')
			logging.write(pullres[2] + '\n')
			if stop_on_error == '1':
				break
			else:
				clean_working_copy()
				continue
			

if __name__ == '__main__':
	config = ConfigParser.ConfigParser()
	config.read(sys.argv[1])
	merge_and_build(config)
	logging.close()
	print 'Done everything!!'

