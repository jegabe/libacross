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
#ifndef lia_defs_h_INCLUDED
#define lia_defs_h_INCLUDED

#define lia_MAJOR_VERSION 0
#define lia_MINOR_VERSION 0
#define lia_PATCH_VERSION 1
#define lia_LABEL_VERSION "pre"

#define lia_STRINGIFY_(x) #x
#define lia_STRINGIFY(x) lia_STRINGIFY_(x)

#define lia_VERSION_STRING lia_STRINGIFY(lia_MAJOR_VERSION) "." lia_STRINGIFY(lia_MINOR_VERSION) "." lia_STRINGIFY(lia_PATCH_VERSION) "-" lia_LABEL_VERSION

#ifdef _MSC_VER /* Visual Studio Compiler */
	#define lia_EXPORT __declspec(dllexport)
	#define lia_IMPORT __declspec(dllimport)
	#if defined(_WIN64)
		#define lia_WORD_WIDTH 64
		#define lia_CALL
	#else
		#define lia_WORD_WIDTH 32
		#define lia_CALL __cdecl
	#endif
	#ifdef __cplusplus
		#if ((_MSC_VER >= 1800) || defined(__RESHARPER__)) /* VS 2013 */
			#define lia_CPP11_API 1
		#else
			#define lia_CPP11_API 0
		#endif
		#if ((_MSC_VER >= 1900)  || defined(__RESHARPER__))/* VS 2015 */
			#define lia_CPP14_API 1
		#else
			#define lia_CPP14_API 0
		#endif
		#if ((_MSC_VER >= 1910)  || defined(__RESHARPER__)) /* VS 2017 */
			#define lia_CPP17_API 1
		#else
			#define lia_CPP17_API 0
		#endif
	#endif
#elif defined(__GNUC__) /* GCC */
	#if defined (_WIN32) /* defined for 32 and 64 bit Windows */
		#define lia_EXPORT __attribute__((dllexport))
		#define lia_IMPORT __attribute__((dllimport))
	#else
		#define lia_EXPORT __attribute__((visibility("default")))
		#define lia_IMPORT
	#endif
	#ifdef __cplusplus
		#if (__cplusplus >= 201103L)
			#define lia_CPP11_API 1
		#else
			#define lia_CPP11_API 0
		#endif
		#if (__cplusplus >= 201402L)
			#define lia_CPP14_API 1
		#else
			#define lia_CPP14_API 0
		#endif
		#if (__cplusplus >= 201703L)
			#define lia_CPP17_API 1
		#else
			#define lia_CPP17_API 0
		#endif
	#endif
	#if (defined(__LP64__) || defined(_LP64))
		#define lia_WORD_WIDTH 64
		#define lia_CALL
	#else
		#define lia_WORD_WIDTH 32
		#if (defined(__i386) || defined(__i386__))
			#define lia_CALL __attribute__((cdecl))
		#else
			#define lia_CALL
		#endif
	#endif
#else
	#error "Compiler not supported yet"
#endif

#ifdef __cplusplus

#include <cstddef> // for std::size_t
#include <cwchar>  // for WCHAR_MIN and WCHAR_MAX

#define lia_EXTERN_C extern "C"

#ifdef _MSC_VER // Visual C++
	#if (_MSC_VER >= 1600) // >= Visual C++ 2010
		#include <cstdint>
		namespace lia {
			using ::std::int8_t;
			using ::std::uint8_t;
			using ::std::int16_t;
			using ::std::uint16_t;
			using ::std::int32_t;
			using ::std::uint32_t;
			using ::std::int64_t;
			using ::std::uint64_t;
		}
	#else
		// no <cstdint> available, but compiler has type support:
		namespace lia {
			typedef signed char      int8_t;
			typedef unsigned char    uint8_t;
			typedef signed   __int16 int16_t;
			typedef unsigned __int16 uint16_t;
			typedef signed   __int32 int32_t;
			typedef unsigned __int32 uint32_t;
			typedef signed   __int64 int64_t;
			typedef unsigned __int64 uint64_t;
		}
	#endif
#elif defined(__GNUC__) /* GCC */
	#if lia_CPP11_API
		#include <cstdint>
		namespace lia {
				using ::std::int8_t;
				using ::std::uint8_t;
				using ::std::int16_t;
				using ::std::uint16_t;
				using ::std::int32_t;
				using ::std::uint32_t;
				using ::std::int64_t;
				using ::std::uint64_t;
		}
	#else
		// <cstdint> not available, so we have to define the types manually.
        // One way is to inspect the type limits and decide by that.
		#include <climits>
		namespace lia {
				#if (UCHAR_MAX == 255)
					typedef signed char int8_t;
					typedef unsigned char uint8_t;
				#else
					#error "Can't deduce 8 bit integer types"
				#endif
				#if (USHRT_MAX == 65535)
					typedef signed short int16_t;
					typedef unsigned short uint16_t;
				#elif (UINT_MAX == 65535)
					typedef signed int int16_t;
					typedef unsigned int uint16_t;
				#else
					#error "Can't deduce 16 bit integer types"
				#endif
				#if (UINT_MAX == 4294967295ull)
					typedef signed int int32_t;
					typedef unsigned int uint32_t;
				#elif (ULONG_MAX == 4294967295ull)
					typedef signed long int32_t;
					typedef unsigned long uint32_t;
				#else
					#error "Can't deduce 32 bit integer types"
				#endif
				#if (ULONG_MAX == 18446744073709551615ull)
					typedef signed long int64_t;
					typedef unsigned long uint64_t;
				#elif (ULLONG_MAX == 18446744073709551615ull)
					typedef signed long long int64_t;
					typedef unsigned long long uint64_t;
				#else
					#error "Can't deduce 64 bit integer types"
				#endif
			}
	#endif
#else
	#error "Compiler not supported yet"
#endif

#ifdef lia_DISABLE_EXCEPTIONS
	#include <cstdlib> // for std::abort
	#include <cstdio>  // for std::fprintf
#endif

namespace lia {

#if lia_CPP11_API
	#define lia_NOEXCEPT              noexcept
	#define lia_OVERRIDE              override
	#define lia_NULLPTR               nullptr
	#define lia_FINAL                 final
	#define lia_STATIC_ASSERT(x, msg) static_assert(x, msg);
#else
	#define lia_NOEXCEPT              throw()
	#define lia_OVERRIDE
	#define lia_NULLPTR               0
	#define lia_FINAL
	#define lia_STATIC_ASSERT(x, msg)
#endif

#if (lia_WORD_WIDTH == 32)
	typedef uint32_t abi_size_t;
	typedef int32_t  abi_ptrdiff_t;
#elif (lia_WORD_WIDTH == 64)
	typedef uint64_t abi_size_t;
	typedef int64_t  abi_ptrdiff_t;
#else
	#error "Word width not supported yet"
#endif

typedef double memalign_t;
lia_STATIC_ASSERT(sizeof(void*) == sizeof(abi_size_t),    "abi_size_t should match the size of a pointer")
lia_STATIC_ASSERT(sizeof(void*) == sizeof(abi_ptrdiff_t), "abi_ptrdiff_t should match the size of a pointer")

typedef bool abi_bool_t;
lia_STATIC_ASSERT(sizeof(abi_bool_t) == 1, "Please rewrite the code to choose another 1 byte type for abibool_t")
const abi_bool_t abi_true  = true;
const abi_bool_t abi_false = false;

namespace detail {

const std::size_t kProxySize       = sizeof(void*)*4u;
const std::size_t kIteratorBufSize = sizeof(void*)*10u;

template<typename T>
T& derefElemPtr(T* ptr) {
	return *ptr;
}

template<typename T>
const T& derefElemPtr(const T* ptr) {
	return *ptr;
}

template<typename T, typename U>
void assignElemPtr(T*& ptr, U& u) {
	ptr = &u;
}

template<typename T, typename U>
void assignElemPtr(const T*& ptr, const U& u) {
	ptr = &u;
}

template<typename T>
struct MakeTypes {
	typedef T&       Reference;
	typedef T*       Pointer;
	typedef const T& ConstReference;
	typedef const T* ConstPointer;
};

template<typename T>
struct MakeInterface {
	typedef T Type;
};

}

template<typename T>
struct MakeRef {
	typedef T Type;
};

#ifdef lia_DISABLE_EXCEPTIONS
	#ifndef lia_THROW0 // you might as well override this...
		inline void exitProgram(const char* pType) {
			std::fprintf(stderr, "Error of type '%s' occured. Terminating.", pType);
			std::fflush(stderr);
			std::abort();
		}
		inline void exitProgram(const char* pType, const char* pParam0) {
			std::fprintf(stderr, "Error of type '%s' occured: '%s'. Terminating.", pType, pParam0);
			std::fflush(stderr);
			std::abort();
		}
		#define lia_THROW0(type)         lia::exitProgram(#type)
		#define lia_THROW1(type, param0) lia::exitProgram(#type, param0)
		#define lia_TRY {
		#define lia_CATCHALL(action) }
	#endif
#else
	#define lia_THROW0(type) throw type()
	#define lia_THROW1(type, param0) throw type(param0)
	#define lia_TRY try {
	#define lia_CATCHALL(action) } catch(...) { action; }
#endif

//! For semver-style versioning of interfaces
//! The "bugfix" tag is left out because interfaces can't have
//! bugs fixed without changing the ABI
struct InterfaceVersion {
	uint32_t major; //!< Is incremented when the ABI breaks and a completely new version is added
	uint32_t minor; //!< Is incremented when a new function is appended to the interface
};

#define lia_ABI (downCast().getAbi())

//! \def lia_HAS_EXPECTED_WCHAR_T_SIZE
//! \hideinitializer
//! Is defined to one (1) when wchar_t is 16 bits under windows and 32 bits under linux (unsigned) for the currently used build environment,
//! which is the default or expected size for those platforms.
//! If that's not the case, it is defined to be (0) and as a consequence, no lia::IString<wchar_t> support will be compiled, in order
//! to avoid errors during function calls between modules built with different compilers
#ifdef _WIN32
	#if ((WCHAR_MIN == 0) && (WCHAR_MAX == 0xFFFF))
		#define lia_HAS_EXPECTED_WCHAR_T_SIZE 1
	#endif
#elif (defined (__linux__) || defined(__gnu_linux__))
	#if ((WCHAR_MIN == 0) && (WCHAR_MAX == 0xFFFFFFFF))
		#define lia_HAS_EXPECTED_WCHAR_T_SIZE 1
	#endif
#endif
#ifndef lia_HAS_EXPECTED_WCHAR_T_SIZE
	#define lia_HAS_EXPECTED_WCHAR_T_SIZE 0
#endif

// Re-implementation of std::is_same because this lib also supports old C++ (98) standard
template<typename T, typename U>
struct IsSame {
  static const bool value = false;
};

template<typename T>
struct IsSame<T, T> {
  static const bool value = true;
};

template<typename T>
struct RemoveReference
{
	typedef T type;
};

template<typename T>
struct RemoveReference<T&>
{
	typedef T type;
};

template<typename T>
struct RemoveReference<const T&>
{
	typedef T type;
};

template<bool b, class T=void>
struct EnableIf;

template<class T>
struct EnableIf<true, T> {
	typedef T type;
};

}

#else /* C compiler */

#include <stdint.h>

#define lia_EXTERN_C

#endif

#endif
