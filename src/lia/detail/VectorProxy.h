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
#ifndef lia_detail_VectorProxy_h_INCLUDED
#define lia_detail_VectorProxy_h_INCLUDED

#include <lia/IVector.h>
#include <lia/detail/PushWarnings.h>

#ifdef __cplusplus

#include <new> // for placement new

namespace lia {

template<typename T, typename TVector>
class VectorRef;

namespace detail {

#define lia_VectorProxy_BASE(T) VectorApiMixin<T, \
                                               VectorProxy<T>, \
                                               typename MakeTypes<T>::Reference, \
                                               typename MakeTypes<T>::Pointer, \
                                               typename MakeTypes<T>::ConstReference, \
                                               typename MakeTypes<T>::ConstPointer, \
                                               VectorIteratorHandle<T>, \
                                               VectorIteratorHandle<const T> \
                                              >

// Helper to transport any IVector<T> derivate (let's call it TClass)
// over ABI/DLL borders
// without need of dynamic memory allocation or yet another pointer indirection.
// This is used primarily to transport VectorRef<T> instances over DLL boundaries
// Precondition for usage with TClass is:
// - TClass is derived by single derivation from IVector<T>
// - TClass is trivially constructable by just memcpy-ing it from another TClass instance
// - TClass can be destroyed without invoking any destructor (TClass must not have non-POD members except
//   for the vtable pointer)
// - A TClass instance can be moved around in memory (std::memmove) without loosing integrity
// - The maximum size of a TClass instance is lia::kProxySize bytes
// This works by constructing and holding the TClass instance inside an aligned in-object buffer
// of size lia::kProxySize and only accessing it via its base class IVector<T>.
// This is a helper class, so don't use it directly but only over functions that generate instances
// of it.
template<typename T>
class VectorProxy: public lia_VectorProxy_BASE(T) {
public:

	VectorProxy<T>* operator->() const lia_NOEXCEPT {
		return this;
	}

	IVector<T>& getAbi() lia_NOEXCEPT {
		return *reinterpret_cast<IVector<T>*>(m_buf.data);
	}

	const IVector<T>& getAbi() const lia_NOEXCEPT {
		return *reinterpret_cast<const IVector<T>*>(m_buf.data);
	}

	operator IVector<T>&() lia_NOEXCEPT {
		return getAbi();
	}

	operator const IVector<T>&() const lia_NOEXCEPT {
		return getAbi();
	}

	VectorProxy() lia_NOEXCEPT {}

	template<typename IVectorDerivate, typename TParam0>
	void construct(TParam0& p0) lia_NOEXCEPT {
		lia_STATIC_ASSERT(sizeof(IVectorDerivate) <= sizeof(m_buf), "IVectorDerivate instance can't be placed into internal buffer")
		new(m_buf.data) IVectorDerivate(p0);
	}

	template<typename IVectorDerivate, typename TParam0>
	void construct(const TParam0& p0) lia_NOEXCEPT {
		lia_STATIC_ASSERT(sizeof(IVectorDerivate) <= sizeof(m_buf), "IVectorDerivate instance can't be placed into internal buffer")
		new(m_buf.data) IVectorDerivate(p0);
	}

	template<typename U>
	VectorProxy& operator=(const U& u) {
		(void)this->ApiBase::operator=(u);
		return *this;
	}

private:

	typedef lia_VectorProxy_BASE(T) ApiBase;

	union Data {
		memalign_t alignmentDummy;
		char data[kProxySize];
	} m_buf;
};

#undef lia_VectorProxy_BASE

lia_STATIC_ASSERT(sizeof(VectorProxy<int>) == kProxySize, "Class must only contain the m_buf member")

template<typename T>
VectorProxy<T> derefElemPtr(VectorProxy<T>& p) {
	return p;
}

template<typename T>
VectorProxy<const T> derefElemPtr(VectorProxy<const T>& p) {
	return p;
}

template<typename T, typename U, typename V>
void assignElemPtr(VectorProxy<T>& p, std::vector<U, V>& u) {
	p.template construct< lia::VectorRef< T, std::vector<U, V>& > >(u);
}

template<typename T, typename U, typename V>
void assignElemPtr(VectorProxy<const T>& p, const std::vector<U, V>& u) {
	p.template construct< lia::VectorRef< const T, std::vector<U, V>& > >(const_cast<std::vector<U, V>&>(u));
}

}
}

#else /* C compiler */

/* Not yet implemented */

#endif

#include <lia/detail/PopWarnings.h>

#endif
