#!/usr/bin/python
import sys
import os
import commands

class Bisect:
	rev_list="git rev-list --parents --pretty=oneline --first-parent --boundary "
	awk="awk '{print$1}'"
	
	def __init__(self, good_commit, bad_commit, repository):
		git_rev_command=self.rev_list+good_commit+".."+bad_commit+" | "+self.awk

		if not os.path.exists(repository):
			sys.exit("Fatal:  " + repository + " does not exist," + \
			" impossible to continue!")
		os.chdir(repository)
		status, output = commands.getstatusoutput(git_rev_command)
		self.commits = output.split("\n")
		self.commits.reverse()

		length=self.getLength()
		if length == 0:
			sys.exit("Error: list is empty commits were not found")

		self.good=0
		self.number=length-2
		self.bad=self.number+1

	def getPointOfTest(self):
		return self.good+self.delta+1
		
	def getLength(self):
		return len(self.commits)

	def getCommit(self, index):
		return self.commits[index]

	def isEven(self):
		return self.number % 2 == 0
			
	def searchExhausted(self):
		return self.calculateRevisions() <= 1

	def calculateRevisions(self):
		return self.bad-self.good-1

	def calculateInterval(self):
		if self.isEven():
			delta = (self.bad-self.good-3)/2
		else:
			delta = (self.bad-self.good-2)/2

		return delta

	def execute(self):
		if not self.searchExhausted():
			self.delta = self.calculateInterval()
			point_of_test=self.getPointOfTest()
			print self.getCommit(point_of_test)
		else:
			if self.calculateRevisions() == 0:
				print "The bad revision is : " + self.getCommit(self.bad)
			else:
				self.point_of_test=self.bad-1
				print "The last revision to test: "+self.getCommit(self.point_of_test)



if __name__ == '__main__': 
        	if len(sys.argv) < 3:
	                sys.exit("Usage: python bisect.py $good_commit $bad_commit")

		root = os.getenv("CIT_DIR")
		if root == None:
			sys.exit("Fatal: CIT_DIR is not exported")
		repository=root+"/repository"
	
		bisector = Bisect(sys.argv[1], sys.argv[2], repository)
		bisector.execute()

