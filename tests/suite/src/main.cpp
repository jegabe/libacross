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
#include <string>
#include <cstring>
#include <iostream>
#include <gtest/gtest.h>
#include <lia/DllLoader.h>

using namespace std;

namespace {

bool startsWith(const string& s, const char* p) {
	const size_t len = strlen(p);
	return (len <= s.length()) && (memcmp(s.c_str(), p, len) == 0);
}

#define STATIC_STRLEN(x) static_cast<std::size_t>((sizeof(x)/sizeof(x[0])) - 1u)

}

int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc, argv);
	bool dllSpecified = false;
	for (int i=1; i<argc; ++i) {
		const string arg = argv[i];
		const char kBegin[] = "--dll=";
		if (startsWith(arg, kBegin)) {
			dllSpecified = true;
			const string dllPath = arg.substr(STATIC_STRLEN(kBegin));
			const bool couldAdd = lia::dll_loader::addDll(dllPath);
			if (!couldAdd) {
				cerr << "Could not load Dll '" << dllPath << "'!" << endl;
				lia::dll_loader::unloadAllDlls();
				return 1;
			}
		}
	}
	if (!dllSpecified) {
		cerr << "Minimum one Dll path needs to be specified with '--dll=<path>'!" << endl;
		return 1;
	}
	const auto result = RUN_ALL_TESTS();
	lia::dll_loader::unloadAllDlls();
	return result;
}
