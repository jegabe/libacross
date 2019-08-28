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

project(libacross) 
cmake_minimum_required(VERSION 3.10) 

if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake) 
	include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake) 
	conan_basic_setup(TARGETS) 
else() 
	message(FATAL_ERROR "Run 'conan install' first") 
endif() 

set (Sources
	src/lia/detail/BasicStringApiMixin.h
	src/lia/detail/PopWarnings.h
	src/lia/detail/PushWarnings.h
	src/lia/detail/VectorApiMixin.h
	src/lia/detail/VectorProxy.h
	src/lia/BasicStringRef.h
	src/lia/defs.h
	src/lia/IBasicString.h
	src/lia/IVector.h
	src/lia/VectorRef.h
	src/DummyMain.cpp
) 

set(target_name libacross) 
add_library(${target_name} INTERFACE) # Header only libraries 
target_sources(${target_name} INTERFACE ${Sources}) 
target_include_directories(${target_name} INTERFACE ${PROJECT_SOURCE_DIR}/src) 

if(WIN32 OR UNIX)
	# Project in order to create .vcproj to be able to edit lib with IDE
	add_executable(DummyForIde ${Sources})
	target_include_directories(DummyForIde PRIVATE ${PROJECT_SOURCE_DIR}/src) 
endif()

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${Sources})
