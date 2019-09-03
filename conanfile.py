# +-----------------------------------+
# |                                   |
# |                 A                 |
# |                A A                |
# |               A   A               |
# |              A     A              |
# |  #  AAAAAAAAAAAAAAAAAAAAAAAAA  #  |
# |  ###       A         A       ###  |
# |  ######   A           A    #####  |
# |  ####### A             A #######  |
# |                                   |
# +-----------------------------------+
# 
# L I B A C R O S S - Using C++ containers
#  across DLL- and ABI-stable boundaries
# 
# If you like this project, please refer to it with a link or
# some other reference. You can use this ASCII art icon as well
# as the supplied graphical icons for that purpose.
# 
# (c) Copyright 2019 Jens Ganter-Benzing
# 
# Licensed under the MIT license:
# 
# http://www.opensource.org/licenses/mit-license.php
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

from conans import ConanFile, CMake

class Libacross(ConanFile):
	name = "libacross"
	version = "0.1.0"
	url = "https://github.com/jegabe/libacross.git"
	license = "MIT"
	description = "See README.md"
	# No settings required: This is a header only library
	exports_sources = "src/*"
	no_copy_source = True
	
	def package(self):
		self.copy("*.h", dst="include", src="src")
			
	def package_info(self):
		if not self.in_local_cache: # when package is in editable mode: The include dir is src/
			self.cpp_info.includedirs = ["src"]
	
	def package_id(self):
		self.info.header_only()
