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

project(libacross_test_dll) 
cmake_minimum_required(VERSION 3.10) 

if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake) 
	include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake) 
	conan_basic_setup(TARGETS) 
else() 
	message(FATAL_ERROR "Run 'conan install' first") 
endif() 

set (Sources
	src/lia/testDllApi.h
	src/lia/testDllApi.cpp
) 

set(target_name liatestlib) 
add_library(${target_name} SHARED) 
target_sources(${target_name} PRIVATE ${Sources}) 
target_include_directories(${target_name} PUBLIC ${PROJECT_SOURCE_DIR}/src) 
target_compile_definitions(${target_name} PRIVATE lia_INSIDE_TEST_DLL) # Switch to export DLL functions
target_link_libraries(${target_name} PUBLIC CONAN_PKG::libacross) 
set_target_properties(${target_name} PROPERTIES 
	POSITION_INDEPENDENT_CODE 1         # pass -fPIC under linux, when not already done 
	C_VISIBILITY_PRESET   hidden        # pass -fvisibility=hidden under linux for C 
	CXX_VISIBILITY_PRESET hidden        # pass -fvisibility=hidden under linux for C++ 
	VISIBILITY_INLINES_HIDDEN 1         # No public visibility for inlines as well 
)
 
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${Sources})
