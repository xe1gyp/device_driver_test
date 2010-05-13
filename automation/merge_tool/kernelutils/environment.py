import os
import sys
import ConfigParser

class Environment:

        def __init__(self, configuration_section):

                self.configuration_section = configuration_section

                root = os.getenv('WORKING_DIR')
                if root == None:
                        sys.exit("Fatal: "+ root +" is not exported")

                env = os.getenv("PATH")
                if env == None:
                        sys.exit("Fatal: "+ env +" is not exported")

                repo = os.getenv('TDT')
                if repo == None:
                        sys.exit("Fatal: "+ repo +" is not exported")

                repo_info = os.getenv('REPO_INFO')
                if repo_info == None:
                        sys.exit("Fatal: Configuration file " + \
                        repo_info + " does not exist," + \
                        " impossible to continue!")

                os.chdir(root)
                config = ConfigParser.ConfigParser()
                config.read(repo_info)

                if not config.has_section(self.configuration_section):
                        print("\nERR: Please provide correct git Repo Identifier\n")
                        return
                else:
                        toolchain = config.get(self.configuration_section, 'toolchain')
                        defconfig = config.get(self.configuration_section, 'defconfig')
			url_http = config.get(self.configuration_section, 'url_http')
			url_git = config.get(self.configuration_section, 'url_git')
			branch = config.get(self.configuration_section, 'branch')
                        print("COMPILE configuration settings for: " + self.configuration_section)
                        print("TOOLCHAIN: " + toolchain)
                        print("DEFCONFIG: " + defconfig)
			print("URL_HTTP: " + url_http)
			print("URL_GIT: " + url_git)
			print("BRANCH: " + branch)

                os.putenv("PATH", toolchain+":"+env)
                self.git_base_url = os.path.join(root, repo)
                self.defconfig = defconfig
		self.toolchain = toolchain
		self.url_http = url_http
		self.url_git = url_git
		self.branch = branch
		self.root = root
		self.repo = repo
		self.images_dir = "images"

        def getConfigurationSection(self):
		"""METHOD Environment.getConfigurationSection
		Get the Section name that identify an
		specific release
		Input: None
		Output: self.configuration_section """
		return self.configuration_section
			
	def getGitBaseURL(self):
                """METHOD Environment.getGitBaseURL
                Get the current building directory.
                Input: None
                Output: Git Base URL"""
                return self.git_base_url

        def getDefConfig(self):
                """METHOD Environment.getDefConfig
                Get the default configuration file. 
                Input: None
                Output: configuration file"""
                return self.defconfig

	def getToolchain(self):
                """METHOD Environment.getToolchain
                Get the default toolchain. 
                Input: None
                Output: toolchain"""
                return self.toolchain

	def getUrlHttp(self):
                """METHOD Environment.getUrlHttp
                Get the http url to clone the repository 
                Input: None
                Output: http url repository"""
                return self.url_http

	def getUrlGIt(self):
                """METHOD Environment.getUrlGit
                Get the git url to clone the repository 
                Input: None
                Output: git url repository"""
                return self.utl_git

	def getBranch(self):
                """METHOD Environment.getBranch
                Get the working branch. 
                Input: None
                Output: branch"""
                return self.branch

	def getRepo(self):
                """METHOD Environment.getRepo
                Get the repo dir.
                Input: None
                Output: repo"""
                return self.repo

	def getRoot(self):
                """METHOD Environment.getRoot
                Get the root folder.
                Input: None
                Output: root folder"""
                return self.root

	def getImagesDir(self):
                """METHOD Environment.getImagesDir
                Get the images folder fro all the binaries.
                Input: None
                Output: images folder"""
                return self.images_dir

