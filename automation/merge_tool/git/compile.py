#!/usr/bin/python

import os
import commands
import sys

class Compile:

    def __init__(self, \
                 path_of_module, \
                 configuration_name, \
                 makefile_compile_rule, \
                 architecture_type = "arm", \
                 compiler_prefix = "arm-none-linux-gnueabi-", \
                 makefile_clean_rule = "distclean"):

        self.path_of_module = path_of_module
        self.configuration_name = configuration_name
        self.makefile_compile_rule = makefile_compile_rule

        self.architecture_type = architecture_type
        self.compiler_prefix = compiler_prefix
        self.makefile_clean_rule = makefile_clean_rule

        self.compiler_option = None
        self.architecture_option = None

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

    def runClean(self):
        """METHOD Compile.runClean
        Run clean process
        Input: None
        Output: None"""

        os.chdir(self.path_of_module)
        status, output = commands.getstatusoutput( \
                                                "make -j9 " + \
                                                self.architecture_option + \
                                                " " + \
                                                self.cross_compile_option + \
                                                " " + \
                                                self.makefile_clean_rule)
        if status:
            print "\nCannot clean, exit script!\n" + output + "\n"
            sys.exit()

    def runConfiguration(self):
        """METHOD Compile.runConfiguration
        Run configuration process
        Input: None
        Output: None"""

        os.chdir(self.path_of_module)
        status, output = commands.getstatusoutput(\
                                                  "make -j9 " + \
                                                  self.architecture_option + \
                                                  " " + \
                                                  self.cross_compile_option + \
                                                  " " + \
                                                  self.configuration_name)
        if status:
            print "\nCannot configure, exit script!\n" + output + "\n"
            sys.exit()

    def runBuild(self):
        """METHOD Compile.runBuild
        Run build process
        Input: None
        Output: None"""

        os.chdir(self.path_of_module)
        status, output = commands.getstatusoutput( \
                                                  "make -j9 " + \
                                                  self.architecture_option + \
                                                  " " + \
                                                  self.cross_compile_option + \
                                                  " " + \
                                                  self.makefile_compile_rule)
        if status:
            print "\nCannot compile, exit script!\n" + output + "\n"
            sys.exit()

    def runCompilation(self):
        """METHOD Compile.runCompilation
        Run the compilation of the module
        Input: none
        Output: Pass/Fail"""

        os.chdir(self.path_of_module)

        self.setArchitectureType(self.architecture_type)
        self.setCompilerPrefix(self.compiler_prefix)
        self.setConfigurationName(self.configuration_name)

        self.setMakefileCleanRule(self.makefile_clean_rule)
        self.setMakefileCompileRule(self.makefile_compile_rule)

        self.runClean()
        self.runConfiguration()
        self.runBuild()


if __name__ == '__main__':

        xloader = Compile( \
                           "/home/abraham/open.source/x-loader/", \
                           "omap4430sdp_config", \
                           "");
        xloader.runCompilation()

        uboot = Compile( \
                           "/home/abraham/open.source/u-boot/", \
                           "omap4430sdp_config", \
                           "");
        uboot.runCompilation()

        kernel = Compile( \
                         "/home/abraham/open.source/kernel-omap4-base/", \
                         "omap_4430sdp_defconfig", \
                         "uImage");
        kernel.runCompilation()
