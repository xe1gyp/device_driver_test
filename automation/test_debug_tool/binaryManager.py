#!/usr/bin/python
import os
from environment import Environment

class BinaryManager:
	def __init__(self, environment, commitId):
		imagesDir=environment.getImagesDir()
		configurationSection=environment.getConfigurationSection()
		configurationImages=imagesDir+"/"+configurationSection

		if not os.path.exists(configurationImages):
			print configurationImages+" directory does not exist" \
			"it will be created"
			try:
				os.makedirs(configurationImages)
			except OSError:
				pass
		self.configurationImages=configurationImages
		self.commitId=commitId

	def searchBinary(self):
		binaryFile=os.path.join(self.configurationImages, self.commitId)
		return os.path.exists(binaryFile)
