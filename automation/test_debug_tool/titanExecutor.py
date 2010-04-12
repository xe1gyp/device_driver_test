#!/usr/bin/python
import os
import sys

class TitanExecutor:
    "TItan Executor class"
    mode = ""
    testcases = ""

    def __init__(self, mode, testcases):
        self.mode=mode
        self.testcases=testcases

    def execute(self):
	root = os.getenv("CIT_DIR")
	os.chdir(root+"/Titan-build-0008/")
        os.system("./start.sh "+self.mode+" "+self.testcases)
               
if len(sys.argv) < 3:
    sys.exit("Usage: python titanExecutor.py $mode $testcases")

executor = TitanExecutor(sys.argv[1], sys.argv[2])
executor.execute()


#print sys.argv
