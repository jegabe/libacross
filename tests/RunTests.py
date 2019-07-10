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

def checkRetCode(completedProcessObj, expectedCode, errStr):
	"""Checks the return value of a subprocess.run() invocation and if the
	return code is not the expected one, it terminates the script with an error
	message"""
	if completedProcessObj.returncode != expectedCode:
		sys.exit("" + errStr)
	return
	
printSeparator("Running test executables...")

basicTestExecutables = [
	"basics/build/vs2017_win32_debug/Debug/basics.exe",
	"basics/build/vs2017_win32_release/Release/basics.exe",
	"basics/build/vs2017_win64_debug/Debug/basics.exe",
	"basics/build/vs2017_win64_release/Release/basics.exe",
	"basics/build/vs2008_win32_debug/Debug/basics.exe",
	"basics/build/vs2008_win32_release/Release/basics.exe",
	"basics/build/mingw_win32_cpp98_debug/basics.exe",
	"basics/build/mingw_win32_cpp98_release/basics.exe",
	"basics/build/mingw_win32_cpp98_release_no_exceptions/basics.exe",
	"basics/build/mingw_win32_cpp11_debug/basics.exe",
	"basics/build/mingw_win32_cpp11_release/basics.exe",
	"basics/build/mingw_win32_cpp14_debug/basics.exe",
	"basics/build/mingw_win32_cpp14_release/basics.exe",
	"basics/build/mingw_win32_cpp17_debug/basics.exe",
	"basics/build/mingw_win32_cpp17_release/basics.exe",
]

for basicTestExecutable in basicTestExecutables:
	printSeparator("Running basic tests '" + basicTestExecutable + "':")
	checkRetCode(subprocess.run([basicTestExecutable]), 0, "Error!")

printSeparator("... done.")
