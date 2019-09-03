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
class IVectorIterator;

template<typename T>
struct IsLiaInterface<IVectorIterator<T> > {
	static const bool value = true;
};

template<typename T>
class VectorIteratorHandle;

template<typename T>
class IVector;

template<typename T>
struct IsLiaInterface<IVector<T> > {
	static const bool value = true;
};

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
struct MakeTypes< const IVector<T> > {
	typedef VectorProxy<const T> Reference;
	typedef VectorProxy<const T> Pointer;
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

#define lia_IVectorIterator_BASE(T) lia::detail::VectorIteratorApiMixin<T, \
                                                                        lia::IVectorIterator<T>, \
                                                                        lia::IVectorIterator<T>, \
                                                                        typename lia::detail::MakeTypes<T>::Reference, \
                                                                        typename lia::detail::MakeTypes<T>::Pointer \
                                                                       >

//! Interface version history:
//!
//! semver | notes
//! ------ | --------
//! 0.1    | Pre-release version
//!
template<typename T>
class IVectorIterator: public lia_IVectorIterator_BASE(T)
{
public:

	typedef IVectorIterator<T> ThisType;

	IVectorIterator& getAbi() lia_NOEXCEPT {
		return *this;
	}

	const IVectorIterator& getAbi() const lia_NOEXCEPT {
		return *this;
	}

	/* vtable index  0 */ virtual void lia_CALL abiGetIVectorIteratorVersion(InterfaceVersion& v) const lia_NOEXCEPT = 0;
	/* vtable index  1 */ virtual void lia_CALL abiCloneTo(void* pBuf) const lia_NOEXCEPT = 0;
	/* vtable index  2 */ virtual void lia_CALL abiFinalize() lia_NOEXCEPT = 0;
	/* vtable index  3 */ virtual abi_ptrdiff_t abiGetDistance(const IVectorIterator<T>& other) const lia_NOEXCEPT = 0;
	/* vtable index  4 */ virtual void lia_CALL abiAdvance(abi_ptrdiff_t n) lia_NOEXCEPT = 0;
	/* vtable index  5 */ virtual void lia_CALL abiDereference(typename lia::detail::MakeTypes<T>::Pointer& pElem, abi_ptrdiff_t i) const lia_NOEXCEPT = 0;
private:

	typedef lia_IVectorIterator_BASE(T) ApiBase;
};

#undef lia_IVectorIterator_BASE

#define lia_VectorIteratorHandle_BASE(T) lia::detail::VectorIteratorApiMixin<T, \
                                                                             lia::VectorIteratorHandle<T>, \
                                                                             lia::IVectorIterator<T>, \
                                                                             typename lia::detail::MakeTypes<T>::Reference, \
                                                                             typename lia::detail::MakeTypes<T>::Pointer \
                                                                            >

template<typename T>
class VectorIteratorHandle: public lia_VectorIteratorHandle_BASE(T)
{
public:

	typedef VectorIteratorHandle<T> ThisType;

	const IVectorIterator<T>& getAbi() const lia_NOEXCEPT {
		return *reinterpret_cast<const IVectorIterator<T>*>(m_buf.data);
	}

	IVectorIterator<T>& getAbi() lia_NOEXCEPT {
		return *reinterpret_cast<IVectorIterator<T>*>(m_buf.data);
	}

	operator const IVectorIterator<T>&() const lia_NOEXCEPT {
		return getAbi();
	}

	operator IVectorIterator<T>&() lia_NOEXCEPT {
		return getAbi();
	}

	VectorIteratorHandle(): m_isConstructed(false), reserved__() {}

	VectorIteratorHandle(const VectorIteratorHandle& other): m_isConstructed(false), reserved__() {
		if (other.m_isConstructed) {
			other.getAbi().abiCloneTo(m_buf.data);
			m_isConstructed = true;
		}
	}

	VectorIteratorHandle& operator=(const VectorIteratorHandle& other) {
		if (&other != this) {
			detachImpl();
			if (other.m_isConstructed) {
				other.getAbi().abiCloneTo(m_buf.data);
				m_isConstructed = true;
			}
		}
		return *this;
	}

	~VectorIteratorHandle() {
		detachImpl();
	}

	void* getBuffer() lia_NOEXCEPT {
		return m_buf.data;
	}

	void setConstructed() lia_NOEXCEPT {
		m_isConstructed = true;
	}

private:

	void detachImpl() {
		if (m_isConstructed) {
			getAbi().abiFinalize();
			m_isConstructed = false;
		}
	}

	union Data {
		memalign_t alignmentDummy;
		char data[lia::detail::kIteratorBufSize];
	} m_buf;
	abi_bool_t m_isConstructed;
	uint8_t reserved__[sizeof(void*)-1];
};

#undef lia_VectorIteratorHandle_BASE

#define lia_IVector_BASE(T) lia::detail::VectorApiMixin<T, \
                                                        lia::IVector<T>, \
                                                        lia::IVector<T>, \
                                                        typename lia::detail::MakeTypes<T>::Reference, \
                                                        typename lia::detail::MakeTypes<T>::Pointer, \
                                                        typename lia::detail::MakeTypes<T>::ConstReference, \
                                                        typename lia::detail::MakeTypes<T>::ConstPointer, \
                                                        VectorIteratorHandle<T>, \
                                                        VectorIteratorHandle<const T> \
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

  lia_STATIC_ASSERT((!lia::IsSame<T, bool>::value),       "No std::vector<bool> support until now. Will follow in a future revision.")
  lia_STATIC_ASSERT((!lia::IsSame<T, const bool>::value), "No std::vector<bool> support until now. Will follow in a future revision.")

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

	/* vtable index  0  */ virtual void lia_CALL abiGetIVectorVersion(InterfaceVersion& v) const lia_NOEXCEPT = 0;
	/* vtable index  1  */ virtual void lia_CALL abiDestroy() lia_NOEXCEPT = 0;
	/* vtable index  2  */ virtual void lia_CALL abiClear() lia_NOEXCEPT = 0;
	/* vtable index  3  */ virtual abi_bool_t lia_CALL abiReserve(abi_size_t n) lia_NOEXCEPT = 0;
	/* vtable index  4  */ virtual abi_bool_t lia_CALL abiInsert(abi_size_t idx, typename lia::detail::MakeTypes<T>::ConstPointer& pElem) lia_NOEXCEPT = 0;
	/* vtable index  5  */ virtual abi_bool_t lia_CALL abiRemove(abi_size_t idx) lia_NOEXCEPT = 0;
	/* vtable index  6  */ virtual abi_size_t lia_CALL abiGetSize() const lia_NOEXCEPT = 0;
	/* vtable index  7  */ virtual abi_bool_t lia_CALL abiGetAt(abi_size_t idx, typename lia::detail::MakeTypes<T>::Pointer& pElem) lia_NOEXCEPT = 0;
	/* vtable index  8  */ virtual abi_bool_t lia_CALL abiGetAtConst(abi_size_t idx, typename lia::detail::MakeTypes<T>::ConstPointer& pElem) const lia_NOEXCEPT = 0;
	/* vtable index  9  */ virtual void lia_CALL abiConstructIterator(abi_bool_t atBegin, void* pBuf) lia_NOEXCEPT = 0;
	/* vtable index  10 */ virtual void lia_CALL abiConstructConstIterator(abi_bool_t atBegin, void* pBuf) const lia_NOEXCEPT = 0;

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
