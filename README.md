# libacross

+-----------------------------------+
|                                   |
|                 A                 |
|                A A                |
|               A   A               |
|              A     A              |
|  #  AAAAAAAAAAAAAAAAAAAAAAAAA  #  |
|  ###       A         A       ###  |
|  ######   A           A    #####  |
|  ####### A             A #######  |
|                                   |
+-----------------------------------+

L I B A C R O S S - Using C++ containers
 across DLL- and ABI-stable boundaries

If you like this project, please refer to it with a link or
some other reference. You can use this ASCII art icon as well
as the supplied graphical icons for that purpose.

(c) Copyright 2019 Jens Ganter-Benzing

Licensed under the MIT license:

http://www.opensource.org/licenses/mit-license.php

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

---

libacross is a C++ header-only library that enables passing STL containers over
DLL and ABI-stable boundaries. DLLs built with different tool chains in different configurations (Debug vs. Release for example)
become interoperable with each other, which makes creation of C++ plug-ins or dynamic libraries that use C++ featurs a lot easier.

The C++ ABI, especially when it comes to C++ standard containers, differs much for each compiler vendor. That means, for example, if
you build your main application with mingw in release mode and use a Dll which is compiled with visual studio in debug mode, and use a
std::vector in the interface between the two, your code will crash because the memory layout and functionality of std::vector in the main
application and the Dll differ (different sets of standard header files and libc++ implementations are used together). Also, there are
two memory allocators and pools involved that aren't compatible.

The library bridges these differences (hence the bridge-shaped libacross logo) so that these problems are solved.

Bridging works by wrapping the C++ containers with wrapper classes that derive from interfaces that are guaranteed to be ABI stable
accross compilers and configurations. Such an interface is, for example, lia::IVector<T>. You can use that interface class in the boundary
between the main application and the Dll. Derived from lia::IVector is lia::VectorRef<T, ...>, which refers to, or wraps, a std::vector.

Code example (C++):

Interface between main app. and dll:

void doSth(lia::IVector<int32_t>& v); // This interface function is ABI stable!

Main app:

std::vector<int32_t> v { 1,2,3 };
doSth(lia::makeRef(v)) // makeRef() creates a reference to v, which derives from lia::IVector and passes it to the Dll

Dll:

void doSth(lia::IVector<int32_t>& v) {
  v.push_back(4); // this push_back call is mirrored into the main app via lia::IVector (the ABI stable interface)
                  // IVector provides the same public API as std::vector, so it can be accessed easily
}

More examples and a more extensive, more beatiful doc will be put under the doc/ folder.

Have fun with the library!
