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
#include <lia/testDllApi.h>
#include <lia/IVector.h>
#include <lia/ISharedPtr.h>
#include <lia/BasicStringRef.h>

using namespace lia;

namespace
{

class Test lia_FINAL: public ITest
{
public:
	Test() {}
	virtual ~Test() {}

	void operator delete(void* p) {
		::operator delete(p);
	}

	virtual void lia_CALL abiGetITestVersion(InterfaceVersion& v) const lia_NOEXCEPT lia_OVERRIDE {
		v.major = 0;
		v.minor = 1;
	}

	virtual void lia_CALL abiDestroy() lia_NOEXCEPT lia_OVERRIDE {
		delete this;
	}

	virtual const char* lia_CALL abiGetName() const lia_NOEXCEPT lia_OVERRIDE {
		return "Test";
	}

};

}

lia_TEST_DLL_API IVector<int32_t>* lia_CALL createInt32Vector() {
	return new VectorRef<int32_t, std::vector<int32_t> >();
}

lia_TEST_DLL_API IVector< IVector<int32_t> >* lia_CALL createInt32VectorVector() {
	return new VectorRef<IVector<int32_t>, std::vector< std::vector<int32_t> > >();
}

lia_TEST_DLL_API IString* lia_CALL createString() {
	return new BasicStringRef<char, std::string>();
}

lia_TEST_DLL_API ISharedPtr<ITest>* lia_CALL createSharedPtr() {
	return new SharedPtrRef<ITest>(std::make_shared<Test>());
}
