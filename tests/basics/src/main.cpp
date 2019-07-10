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
#include <iostream>
#include <lia/defs.h>

using namespace std;
using namespace lia;

namespace {

// Test basic parameter passing & calling conventions & mapping from C++ to C
class IBasics {
public:
	typedef IBasics ThisType;

	void operator delete(void* p) {
		if (p != lia_NULLPTR) {
			static_cast<ThisType*>(p)->abiDestroy();
		}
	}

	/* vtable index  0 */ virtual void lia_CALL abiDestroy() lia_NOEXCEPT = 0;
	/* vtable index  1 */ virtual void lia_CALL abiTestInt8Int16(int8_t in8, int8_t& out8, int16_t in16, int16_t& out16) lia_NOEXCEPT = 0;
	/* vtable index  2 */ virtual void lia_CALL abiTestInt32Int64(int32_t in32, int32_t& out32, int64_t in64, int64_t& out64) lia_NOEXCEPT = 0;
	/* vtable index  3 */ virtual int8_t lia_CALL abiTestInt8Echo(int8_t in) lia_NOEXCEPT = 0;
	/* vtable index  4 */ virtual int16_t lia_CALL abiTestInt16Echo(int16_t in) lia_NOEXCEPT = 0;
	/* vtable index  5 */ virtual int32_t lia_CALL abiTestInt32Echo(int32_t in) lia_NOEXCEPT = 0;
	/* vtable index  6 */ virtual int64_t lia_CALL abiTestInt64Echo(int64_t in) lia_NOEXCEPT = 0;
	/* vtable index  7 */ virtual abi_size_t lia_CALL abiTestSizeEcho(abi_size_t in) lia_NOEXCEPT = 0;
	/* vtable index  8 */ virtual abi_bool_t lia_CALL abiTestBoolEcho(abi_bool_t in) lia_NOEXCEPT = 0;
	/* vtable index  9 */ virtual abi_ptrdiff_t lia_CALL abiTestPtrDiffEcho(abi_ptrdiff_t in) lia_NOEXCEPT = 0;
};

class Basics lia_FINAL: public IBasics {
public:
	Basics() {}
	virtual ~Basics() {}
	virtual void lia_CALL abiDestroy() lia_NOEXCEPT lia_OVERRIDE {
		::operator delete(this);
	}
	virtual void lia_CALL abiTestInt8Int16(int8_t in8, int8_t& out8, int16_t in16, int16_t& out16) lia_NOEXCEPT lia_OVERRIDE {
		out8 = in8;
		out16 = in16;
	}
	virtual void lia_CALL abiTestInt32Int64(int32_t in32, int32_t& out32, int64_t in64, int64_t& out64) lia_NOEXCEPT lia_OVERRIDE {
		out32 = in32;
		out64 = in64;
	}
	virtual int8_t lia_CALL abiTestInt8Echo(int8_t in) lia_NOEXCEPT lia_OVERRIDE {
		return in;
	}
	virtual int16_t lia_CALL abiTestInt16Echo(int16_t in) lia_NOEXCEPT lia_OVERRIDE {
		return in;
	}
	virtual int32_t lia_CALL abiTestInt32Echo(int32_t in) lia_NOEXCEPT lia_OVERRIDE {
		return in;
	}
	virtual int64_t lia_CALL abiTestInt64Echo(int64_t in) lia_NOEXCEPT lia_OVERRIDE {
		return in;
	}
	virtual abi_size_t lia_CALL abiTestSizeEcho(abi_size_t in) lia_NOEXCEPT lia_OVERRIDE {
		return in;
	}
	virtual abi_bool_t lia_CALL abiTestBoolEcho(abi_bool_t in) lia_NOEXCEPT lia_OVERRIDE {
		return in;
	}
	virtual abi_ptrdiff_t lia_CALL abiTestPtrDiffEcho(abi_ptrdiff_t in) lia_NOEXCEPT lia_OVERRIDE {
		return in;
	}
};

}

extern "C" {

// Same with "C" notation, to test C/C++ mapping done by the library
struct SBasics;

struct SBasicsVtable {
	/* vtable index  0 */ void (lia_CALL *abiDestroy)(SBasics* pThis);
	/* vtable index  1 */ void (lia_CALL *abiTestInt8Int16)(SBasics* pThis, int8_t in8, int8_t* out8, int16_t in16, int16_t* out16);
	/* vtable index  2 */ void (lia_CALL *abiTestInt32Int64)(SBasics* pThis, int32_t in8, int32_t* out8, int64_t in16, int64_t* out16);
	/* vtable index  3 */ int8_t (lia_CALL *abiTestInt8Echo)(SBasics* pThis, int8_t in);
	/* vtable index  4 */ int16_t (lia_CALL *abiTestInt16Echo)(SBasics* pThis, int16_t in);
	/* vtable index  5 */ int32_t (lia_CALL *abiTestInt32Echo)(SBasics* pThis, int32_t in);
	/* vtable index  6 */ int64_t (lia_CALL *abiTestInt64Echo)(SBasics* pThis, int64_t in);
	/* vtable index  7 */ abi_size_t (lia_CALL *abiTestSizeEcho)(SBasics* pThis, abi_size_t in);
	/* vtable index  8 */ abi_bool_t (lia_CALL *abiTestBoolEcho)(SBasics* pThis, abi_bool_t in);
	/* vtable index  9 */ abi_ptrdiff_t (lia_CALL *abiTestPtrDiffEcho)(SBasics* pThis, abi_ptrdiff_t in);
};

struct SBasics {
	SBasicsVtable* pVtable;
};

}

int main() {
	Basics b;
	SBasics* p = reinterpret_cast<SBasics*>(&b); // Access C++ obj with C struct
	{
		int8_t i8 = 0;
		int16_t i16 = 0;
		(*p->pVtable->abiTestInt8Int16)(p, 127, &i8, 32767, &i16);
		if (i8 != 127) {
			cerr << "Passing int8 arguments fails!" << endl;
			return 1;
		}
		if (i16 != 32767) {
			cerr << "Passing int16 arguments fails!" << endl;
			return 1;
		}
	}
	{
		int32_t i32 = 0;
		int64_t i64 = 0;
		(*p->pVtable->abiTestInt32Int64)(p, 2147483647, &i32, 9223372036854775807, &i64);
		if (i32 != 2147483647) {
			cerr << "Passing int32 arguments fails!" << endl;
			return 1;
		}
		if (i64 != 9223372036854775807) {
			cerr << "Passing int64 arguments fails!" << endl;
			return 1;
		}
	}
	if ((*p->pVtable->abiTestInt8Echo)(p, 127) != 127) {
		cerr << "Echoing int8 arguments fails!" << endl;
		return 1;
	}
	if ((*p->pVtable->abiTestInt16Echo)(p, 32767) != 32767) {
		cerr << "Echoing int16 arguments fails!" << endl;
		return 1;
	}
	if ((*p->pVtable->abiTestInt32Echo)(p, 2147483647) != 2147483647) {
		cerr << "Echoing int32 arguments fails!" << endl;
		return 1;
	}
	if ((*p->pVtable->abiTestInt64Echo)(p, 9223372036854775807) != 9223372036854775807) {
		cerr << "Echoing int64 arguments fails!" << endl;
		return 1;
	}
	std::cout << "Tests ran successfully!" << endl;
	return 0;
}
