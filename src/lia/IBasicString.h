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
#ifndef lia_IBasicString_h_INCLUDED
#define lia_IBasicString_h_INCLUDED

#include <lia/detail/BasicStringApiMixin.h>
#include <lia/detail/PushWarnings.h>

#ifdef __cplusplus

namespace lia {

template<typename T>
class IBasicString;

#define lia_IBasicString_BASE(T) lia::detail::BasicStringApiMixin<T, \
                                                                  lia::IBasicString<T> \
                                                                 >

//! Interface version history:
//!
//! semver | notes
//! ------ | --------
//! 0.1    | Pre-release version
//!
template<typename T>
class IBasicString: public lia_IBasicString_BASE(T) {
public:

	#if (!lia_HAS_EXPECTED_WCHAR_T_SIZE)
		lia_STATIC_ASSERT((!lia::IsSame<T, wchar_t>::value), "IBasicString can't be instantiated on systems with non-standard wchar_t size. You may be able to fix that with compiler switches that influence the size of size_t. Under Windows, it should be 16 bits wide and under linux, it should be 32 bits wide. See definition of lia_HAS_EXPECTED_WCHAR_T_SIZE.")
	#endif

	typedef IBasicString<T> ThisType;

	void operator delete(void* p) {
		if (p != lia_NULLPTR) {
			static_cast<ThisType*>(p)->abiDestroy();
		}
	}

	IBasicString& getAbi() lia_NOEXCEPT {
		return *this;
	}

	const IBasicString& getAbi() const lia_NOEXCEPT {
		return *this;
	}

#if 0
	template<typename U>
	IBasicString& operator=(const U& u) {
		(void)this->ApiBase::operator=(u);
		return *this;
	}
#endif

	/* vtable index  0 */ virtual void lia_CALL abiGetIBasicStringVersion(InterfaceVersion& v) const lia_NOEXCEPT = 0;
	/* vtable index  1 */ virtual void lia_CALL abiDestroy() lia_NOEXCEPT = 0;

private:

	typedef lia_IBasicString_BASE(T) ApiBase;
};

#undef lia_IBasicString_BASE

lia_STATIC_ASSERT(sizeof(IBasicString<char>) == sizeof(void*), "Interface must be pure virtual")

typedef lia::IBasicString<char> IString;
#if lia_HAS_EXPECTED_WCHAR_T_SIZE
	typedef lia::IBasicString<wchar_t> IWstring;
#endif
#if lia_CPP11_API
	typedef lia::IBasicString<char16_t> IU16String;
	typedef lia::IBasicString<char32_t> IU32String;
#endif

}

#else /* C compiler */

/* Not yet implemented */

#endif

#include <lia/detail/PopWarnings.h>

#endif
