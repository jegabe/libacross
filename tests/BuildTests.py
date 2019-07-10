#+-----------------------------------+
#|                                   |
#|                 A                 |
#|                A A                |
#|               A   A               |
#|              A     A              |
#|  #  AAAAAAAAAAAAAAAAAAAAAAAAA  #  |
#|  ###       A         A       ###  |
#|  ######   A           A    #####  |
#|  ####### A             A #######  |
#|                                   |
#+-----------------------------------+
#
#L I B A C R O S S - Using C++ containers
# across DLL- and ABI-stable boundaries
#
#If you like this project, please refer to it with a link or
#some other reference. You can use this ASCII art icon as well
#as the supplied graphical icons for that purpose.
#
#(c) Copyright 2019 Jens Ganter-Benzing
#
#Licensed under the MIT license:
#
#http://www.opensource.org/licenses/mit-license.php
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in
#all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#THE SOFTWARE.

import os
import subprocess
import sys
from colorama import Fore
from colorama import Style
from colorama import init
init()

print(Fore.GREEN)
print("+-----------------------------------+")
print("|                                   |")
print("|                 A                 |")
print("|                A A                |")
print("|               A   A               |")
print("|              A     A              |")
print("|  #  AAAAAAAAAAAAAAAAAAAAAAAAA  #  |")
print("|  ###       A         A       ###  |")
print("|  ######   A           A    #####  |")
print("|  ####### A             A #######  |")
print("|                                   |")
print("+-----------------------------------+")
print("")
print("L I B A C R O S S - Using C++ containers")
print(" across DLL- and ABI-stable boundaries")
print(Style.RESET_ALL)

def printSeparator(text):
	print("")
	print(Fore.CYAN)
	print("=============================================================")
	print(text)
	print("=============================================================")
	print("")
	print(Style.RESET_ALL)
	return
	
printSeparator("Building testing artefacts...")

def mkDirIfNotExists(path):
	"""Creates a directory, if it does not exist yet"""
	if not os.path.exists(path):
		os.mkdir(path)

def checkRetCode(completedProcessObj, expectedCode, errStr):
	"""Checks the return value of a subprocess.run() invocation and if the
	return code is not the expected one, it terminates the script with an error
	message"""
	if completedProcessObj.returncode != expectedCode:
		sys.exit("" + errStr)
	return

flavors = [
	("vs2017_win32_debug",                      "Visual Studio 15 2017",       "Debug",   []),
	("vs2017_win32_release",                    "Visual Studio 15 2017",       "Release", []),
	("vs2017_win64_debug",                      "Visual Studio 15 2017 Win64", "Debug",   []),
	("vs2017_win64_release",                    "Visual Studio 15 2017 Win64", "Release", []),
	("vs2008_win32_debug",                      "Visual Studio 9 2008",        "Debug",   []),
	("vs2008_win32_release",                    "Visual Studio 9 2008",        "Release", []),
	("mingw_win32_cpp98_debug",                 "MinGW Makefiles",             "",        ["-DGCC_CPP98=1", "-DGCC_DEBUG=1"]),
	("mingw_win32_cpp98_release",               "MinGW Makefiles",             "",        ["-DGCC_CPP98=1", "-DGCC_RELEASE=1"]),
	("mingw_win32_cpp98_release_no_exceptions", "MinGW Makefiles",             "",        ["-DGCC_CPP98=1", "-DGCC_RELEASE=1", "-DGCC_NO_EXCEPTIONS=1"]),
	("mingw_win32_cpp11_debug",                 "MinGW Makefiles",             "",        ["-DGCC_CPP11=1", "-DGCC_DEBUG=1"]),
	("mingw_win32_cpp11_release",               "MinGW Makefiles",             "",        ["-DGCC_CPP11=1", "-DGCC_RELEASE=1"]),
	("mingw_win32_cpp14_debug",                 "MinGW Makefiles",             "",        ["-DGCC_CPP14=1", "-DGCC_DEBUG=1"]),
	("mingw_win32_cpp14_release",               "MinGW Makefiles",             "",        ["-DGCC_CPP14=1", "-DGCC_RELEASE=1"]),
	("mingw_win32_cpp17_debug",                 "MinGW Makefiles",             "",        ["-DGCC_CPP17=1", "-DGCC_DEBUG=1"]),
	("mingw_win32_cpp17_release",               "MinGW Makefiles",             "",        ["-DGCC_CPP17=1", "-DGCC_RELEASE=1"]),
]

testLibs = [
	"basics",
	"libv",
]

for testLib in testLibs:
	mkDirIfNotExists(testLib + "/build")
	os.chdir(testLib + "/build")
	for flavor in flavors:
		subDir     = flavor[0]
		generator  = flavor[1]
		config     = flavor[2]
		switches   = flavor[3]
		printSeparator("Building lib '" + testLib + "' in flavor '" + subDir + "':")
		mkDirIfNotExists(subDir)
		os.chdir(subDir)
		checkRetCode(subprocess.run(["cmake", "../..", "-G", generator] + switches), 0, "Error while configuring for flavor " + subDir)
		if config != "":
			checkRetCode(subprocess.run(['cmake', '--build', '.', '--config', config] + switches), 0, "Error while building flavor " + subDir)
		else:
			checkRetCode(subprocess.run(['cmake', '--build', '.']), 0, "Error while building flavor " + subDir)
		os.chdir("..")
	os.chdir("../..")
	

printSeparator("... done.")
