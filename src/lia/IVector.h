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
#ifndef lia_IVector_h_INCLUDED
#define lia_IVector_h_INCLUDED

#include <lia/detail/VectorApiMixin.h>
#include <lia/detail/PushWarnings.h>

#ifdef __cplusplus

namespace lia {

template<typename T>
class IVector;

namespace detail {

template<typename T>
class VectorProxy;

template<typename T>
struct MakeTypes< IVector<T> > {
	typedef VectorProxy<T>       Reference;
	typedef VectorProxy<T>       Pointer;
	typedef VectorProxy<const T> ConstReference;
	typedef VectorProxy<const T> ConstPointer;
};

template<typename T>
struct MakeTypes< IVector<const T> > {
	typedef VectorProxy<const T> Reference;
	typedef VectorProxy<const T> Pointer;
	typedef VectorProxy<const T> ConstReference;
	typedef VectorProxy<const T> ConstPointer;
};

}

#define lia_IVector_BASE(T) lia::detail::VectorApiMixin<T, \
                                                        lia::IVector<T>, \
                                                        typename lia::detail::MakeTypes<T>::Reference, \
                                                        typename lia::detail::MakeTypes<T>::Pointer, \
                                                        typename lia::detail::MakeTypes<T>::ConstReference, \
                                                        typename lia::detail::MakeTypes<T>::ConstPointer \
                                                       >

//! Interface version history:
//!
//! semver | notes
//! ------ | --------
//! 0.1    | Pre-release version
//!
template<typename T>
class IVector: public lia_IVector_BASE(T) {
public:

	typedef IVector<T> ThisType;

	void operator delete(void* p) {
		if (p != lia_NULLPTR) {
			static_cast<ThisType*>(p)->abiDestroy();
		}
	}

	IVector& getAbi() lia_NOEXCEPT {
		return *this;
	}

	const IVector& getAbi() const lia_NOEXCEPT {
		return *this;
	}

	template<typename U>
	IVector& operator=(const U& u) {
		(void)this->ApiBase::operator=(u);
		return *this;
	}

	/* vtable index  0 */ virtual void lia_CALL abiGetIVectorVersion(InterfaceVersion& v) const lia_NOEXCEPT = 0;
	/* vtable index  1 */ virtual void lia_CALL abiDestroy() lia_NOEXCEPT = 0;
	/* vtable index  2 */ virtual void lia_CALL abiClear() lia_NOEXCEPT = 0;
	/* vtable index  3 */ virtual abi_bool_t lia_CALL abiReserve(abi_size_t n) lia_NOEXCEPT = 0;
	/* vtable index  4 */ virtual abi_bool_t lia_CALL abiInsert(abi_size_t idx, typename lia::detail::MakeTypes<T>::ConstPointer& pElem) lia_NOEXCEPT = 0;
	/* vtable index  5 */ virtual abi_bool_t lia_CALL abiRemove(abi_size_t idx) lia_NOEXCEPT = 0;
	/* vtable index  6 */ virtual abi_size_t lia_CALL abiGetSize() const lia_NOEXCEPT = 0;
	/* vtable index  7 */ virtual abi_bool_t lia_CALL abiGetAt(abi_size_t idx, typename lia::detail::MakeTypes<T>::Pointer& pElem) lia_NOEXCEPT = 0;
	/* vtable index  8 */ virtual abi_bool_t lia_CALL abiGetAtConst(abi_size_t idx, typename lia::detail::MakeTypes<T>::ConstPointer& pElem) const lia_NOEXCEPT = 0;

private:

	typedef lia_IVector_BASE(T) ApiBase;
};

#undef lia_IVector_BASE

lia_STATIC_ASSERT(sizeof(IVector<int>) == sizeof(void*), "Interface must be pure virtual")

}

#else /* C compiler */

/* Not yet implemented */

#endif

#include <lia/detail/PopWarnings.h>

#endif
