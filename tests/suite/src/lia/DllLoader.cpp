/*
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
*/
#include <lia/DllLoader.h>
#include <lia/defs.h>
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN
	#include <Windows.h>
#endif

using namespace std;

namespace {

#ifdef _WIN32
vector<HMODULE> g_Dlls;
#endif
	
}

namespace lia {
namespace dll_loader {

bool addDll(const string& path) {
	bool result = false;
#ifdef _WIN32
	const auto hDll = LoadLibraryA(path.c_str());
	if (hDll != lia_NULLPTR) {
		g_Dlls.push_back(hDll);
		result = true;
	}
#endif
	return result;
}

void unloadAllDlls() {
#ifdef _WIN32
	for (auto dll: g_Dlls) {
		FreeLibrary(dll);
	}
	g_Dlls.clear();
#endif
}

vector<FunctionPointer> getDllFunctions(const string& functionName) {
	vector<FunctionPointer> result;
	result.reserve(g_Dlls.size());
#ifdef _WIN32
	for (auto dll: g_Dlls) {
		const auto proc = GetProcAddress(dll, functionName.c_str());
		if (proc != lia_NULLPTR) {
			result.push_back((FunctionPointer)proc);
		}
	}
#endif
	return result;
}

}
}