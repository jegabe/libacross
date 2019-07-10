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

cmake_minimum_required(VERSION 3.14)
project(libv)

set(Sources
	src/libv/IApi.h
	src/libv/libv.h
	src/libv/libv.cpp
)

set(TargetName libv)

add_library(${TargetName} SHARED ${Sources})

target_include_directories(${TargetName} PRIVATE ../../src)
target_include_directories(${TargetName} PUBLIC ./src)

target_compile_options(${TargetName} PRIVATE -Dlibv_INSIDE_LIBRARY)

set_target_properties(${TargetName} PROPERTIES 
    POSITION_INDEPENDENT_CODE 1         # pass -fPIC under linux, when not already done 
    C_VISIBILITY_PRESET   hidden        # pass -fvisibility=hidden under linux for C 
    CXX_VISIBILITY_PRESET hidden        # pass -fvisibility=hidden under linux for C++ 
    VISIBILITY_INLINES_HIDDEN 1         # No public visibility for inlines as well 
)

if(MSVC)
  target_compile_options(${TargetName} PRIVATE /W4 /WX)
else()
  target_compile_options(${TargetName} PRIVATE -Wall -Wextra -Werror)
endif()

if(${GCC_CPP98})
	message("Building with C++98 standard")
	target_compile_options(${TargetName} PRIVATE -std=gnu++98)
endif()
if(${GCC_CPP11})
	message("Building with C++11 standard")
	target_compile_options(${TargetName} PRIVATE -std=gnu++11)
endif()
if(${GCC_CPP14})
	message("Building with C++14 standard")
	target_compile_options(${TargetName} PRIVATE -std=gnu++14)
endif()
if(${GCC_CPP17})
	message("Building with C++17 standard")
	target_compile_options(${TargetName} PRIVATE -std=gnu++17)
endif()
if(${GCC_DEBUG})
	message("Building with DEBUG configuration")
	target_compile_options(${TargetName} PRIVATE -O0 -g)
endif()
if(${GCC_RELEASE})
	message("Building with RELEASE configuration")
	target_compile_options(${TargetName} PRIVATE -O3 -DNDEBUG)
	target_link_options(${TargetName} PRIVATE -s)
endif()
if(${GCC_NO_EXCEPTIONS})
	target_compile_options(${TargetName} PRIVATE -fno-exceptions -Dlia_DISABLE_EXCEPTIONS)
endif()
if(${GCC_NO_EXCEPTIONS})
	target_compile_options(${TargetName} PRIVATE -fno-exceptions -Dlia_DISABLE_EXCEPTIONS)
endif()

source_group(TREE ${PROJECT_SOURCE_DIR}/../.. FILES ${Sources})
