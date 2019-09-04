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
#ifndef lia_BasicStringRef_h_INCLUDED
#define lia_BasicStringRef_h_INCLUDED

#include <lia/IBasicString.h>
#include <lia/detail/PushWarnings.h>

#ifdef __cplusplus

namespace lia {

template<typename T, typename TString>
class BasicStringRef lia_FINAL: public IBasicString<T> {
public:

	BasicStringRef() lia_NOEXCEPT {}
	explicit BasicStringRef(TString& s) lia_NOEXCEPT :m_string(s) {}
	virtual ~BasicStringRef() lia_NOEXCEPT {}

	void operator delete(void* p) {
		::operator delete(p);
	}

	virtual void lia_CALL abiGetIBasicStringVersion(InterfaceVersion& v) const lia_NOEXCEPT lia_OVERRIDE {
		v.major = 0;
		v.minor = 1;
	}

	virtual void lia_CALL abiDestroy() lia_NOEXCEPT lia_OVERRIDE {
		delete this;
	}

private:
	TString m_string; // either a reference or a type
};

namespace detail {

template<typename A, typename B, typename C>
struct MakeInterface< std::basic_string<A, B, C> > {
	typedef lia::IBasicString<A> Type;
};

template<typename A, typename B, typename C>
struct MakeRef< std::basic_string<A, B, C> > {
	typedef lia::BasicStringRef< A, std::basic_string<A, B, C>& > Type;
};

template<typename A, typename B, typename C>
typename MakeRef< std::basic_string<A, B, C> >::Type makeRef(std::basic_string<A, B, C>& s) {
	return typename MakeRef< std::basic_string<A, B, C> >::Type(s);
}

}

typedef lia::BasicStringRef<char, std::string&> StringRef;
#if lia_HAS_EXPECTED_WCHAR_T_SIZE
	typedef lia::BasicStringRef<wchar_t, std::wstring&> WstringRef;
#endif
#if lia_CPP11_API
	typedef lia::BasicStringRef<char16_t, std::u16string&> U16StringRef;
	typedef lia::BasicStringRef<char32_t, std::u32string&> U32StringRef;
#endif

}

#else /* C compiler */

/* Not yet implemented */

#endif

#include <lia/detail/PopWarnings.h>

#endif
