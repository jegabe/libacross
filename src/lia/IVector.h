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
class IVector;

template<typename T>
class VectorIteratorHandle;

template<typename T, typename TVector>
class VectorRef;

namespace detail {

template<typename T>
struct IsLiaInterface<lia::IVectorIterator<T> > {
	static const bool value = true;
};

template<typename T>
struct IsLiaInterface<lia::IVector<T> > {
	static const bool value = true;
};

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

	/* vtable index  0 */ virtual void          lia_CALL abiGetIVectorIteratorVersion(InterfaceVersion& v) const lia_NOEXCEPT = 0;
	/* vtable index  1 */ virtual void          lia_CALL abiCloneTo(void* pBuf) const lia_NOEXCEPT = 0;
	/* vtable index  2 */ virtual void          lia_CALL abiMoveTo(void* pBuf) lia_NOEXCEPT = 0;
	/* vtable index  3 */ virtual void          lia_CALL abiFinalize() lia_NOEXCEPT = 0;
	/* vtable index  4 */ virtual abi_ptrdiff_t lia_CALL abiGetDistance(const IVectorIterator<T>& other) const lia_NOEXCEPT = 0;
	/* vtable index  5 */ virtual void          lia_CALL abiAdvance(abi_ptrdiff_t n) lia_NOEXCEPT = 0;
	/* vtable index  6 */ virtual void          lia_CALL abiDereference(typename lia::detail::MakeTypes<T>::Pointer& pElem, abi_ptrdiff_t i) const lia_NOEXCEPT = 0;
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

#if lia_CPP11_API

	VectorIteratorHandle(VectorIteratorHandle&& other) lia_NOEXCEPT: m_isConstructed(false), reserved__() {
		if (other.m_isConstructed) {
			other.getAbi().abiMoveTo(m_buf.data);
			m_isConstructed = true;
		}
	}

#endif

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

#if lia_CPP11_API

	VectorIteratorHandle& operator=(VectorIteratorHandle&& other) lia_NOEXCEPT {
		detachImpl();
		if (other.m_isConstructed) {
			other.getAbi().abiMoveTo(m_buf.data);
			m_isConstructed = true;
		}
		return *this;
	}

#endif

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

  lia_STATIC_ASSERT((!lia::detail::IsSame<T, bool>::value),       "No std::vector<bool> support until now. Will follow in a future revision.")
  lia_STATIC_ASSERT((!lia::detail::IsSame<T, const bool>::value), "No std::vector<bool> support until now. Will follow in a future revision.")

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

	IVector& operator=(const ThisType& u) {
		(void)this->ApiBase::operator=(u);
		return *this;
	}

	template<typename U>
	IVector& operator=(const U& u) {
		(void)this->ApiBase::operator=(u);
		return *this;
	}

	/* vtable index  0  */ virtual void       lia_CALL abiGetIVectorVersion(InterfaceVersion& v) const lia_NOEXCEPT = 0;
	/* vtable index  1  */ virtual void       lia_CALL abiDestroy() lia_NOEXCEPT = 0;
	/* vtable index  2  */ virtual void       lia_CALL abiClear() lia_NOEXCEPT = 0;
	/* vtable index  3  */ virtual abi_bool_t lia_CALL abiReserve(abi_size_t n) lia_NOEXCEPT = 0;
	/* vtable index  4  */ virtual abi_bool_t lia_CALL abiInsert(abi_size_t idx, typename lia::detail::MakeTypes<T>::ConstPointer& pElem) lia_NOEXCEPT = 0;
	/* vtable index  5  */ virtual abi_bool_t lia_CALL abiRemove(abi_size_t idx) lia_NOEXCEPT = 0;
	/* vtable index  6  */ virtual abi_size_t lia_CALL abiGetSize() const lia_NOEXCEPT = 0;
	/* vtable index  7  */ virtual abi_bool_t lia_CALL abiGetAt(abi_size_t idx, typename lia::detail::MakeTypes<T>::Pointer& pElem) lia_NOEXCEPT = 0;
	/* vtable index  8  */ virtual abi_bool_t lia_CALL abiGetAtConst(abi_size_t idx, typename lia::detail::MakeTypes<T>::ConstPointer& pElem) const lia_NOEXCEPT = 0;
	/* vtable index  9  */ virtual void       lia_CALL abiConstructIterator(abi_bool_t atBegin, void* pBuf) lia_NOEXCEPT = 0;
	/* vtable index  10 */ virtual void       lia_CALL abiConstructConstIterator(abi_bool_t atBegin, void* pBuf) const lia_NOEXCEPT = 0;

private:

	typedef lia_IVector_BASE(T) ApiBase;
};

#undef lia_IVector_BASE

lia_STATIC_ASSERT(sizeof(IVector<int>) == sizeof(void*), "Interface must be pure virtual")

namespace detail {

#define lia_VectorProxy_BASE(T) VectorApiMixin<T, \
                                               VectorProxy<T>, \
                                               lia::IVector<T>, \
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

	VectorProxy<T>* operator->() lia_NOEXCEPT {
		return this;
	}

	const VectorProxy<T>* operator->() const lia_NOEXCEPT {
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

template<typename T, typename TIterator>
class VectorIteratorRef lia_FINAL: public IVectorIterator<T> {
public:

	typedef VectorIteratorRef<T, TIterator> ThisType;

	VectorIteratorRef() lia_NOEXCEPT: m_iter() {}
	explicit VectorIteratorRef(const TIterator& i) lia_NOEXCEPT :m_iter(i) {}
#if lia_CPP11_API
	explicit VectorIteratorRef(TIterator&& i) lia_NOEXCEPT :m_iter(std::forward<TIterator>(i)) {}
#endif

	virtual ~VectorIteratorRef() lia_NOEXCEPT {}

	virtual void lia_CALL abiGetIVectorIteratorVersion(InterfaceVersion& v) const lia_NOEXCEPT lia_OVERRIDE {
		v.major = 0;
		v.minor = 1;
	}

	virtual void lia_CALL abiCloneTo(void* pBuf) const lia_NOEXCEPT lia_OVERRIDE {
		new(pBuf) ThisType(m_iter);
	}

	virtual void lia_CALL abiMoveTo(void* pBuf) lia_NOEXCEPT lia_OVERRIDE {
		new(pBuf) ThisType(
#if lia_CPP11_API
			std::move(m_iter)
#else
			m_iter
#endif
		);
	}

	virtual void lia_CALL abiFinalize() lia_NOEXCEPT lia_OVERRIDE {
		m_iter.~TIterator();
	}

	virtual abi_ptrdiff_t lia_CALL abiGetDistance(const IVectorIterator<T>& other_) const lia_NOEXCEPT lia_OVERRIDE {
		const ThisType& other = static_cast<const ThisType&>(other_);
		return static_cast<abi_ptrdiff_t>(m_iter - other.m_iter);
	}

	virtual void lia_CALL abiAdvance(abi_ptrdiff_t n) lia_NOEXCEPT lia_OVERRIDE {
		m_iter += static_cast<std::ptrdiff_t>(n);
	}

	virtual void lia_CALL abiDereference(typename lia::detail::MakeTypes<T>::Pointer& pElem, abi_ptrdiff_t i) const lia_NOEXCEPT lia_OVERRIDE {
		lia::detail::assignElemPtr(pElem, m_iter[static_cast<std::ptrdiff_t>(i)]);
	}

private:
	TIterator  m_iter;
};

template<typename T, typename TVector>
class VectorRef lia_FINAL: public IVector<T> {
public:

	typedef VectorRef<T, TVector> ThisType;

	VectorRef() lia_NOEXCEPT {}
	explicit VectorRef(TVector& v) lia_NOEXCEPT :m_vector(v) {}
	virtual ~VectorRef() lia_NOEXCEPT {}

	void operator delete(void* p) {
		::operator delete(p);
	}

	virtual void lia_CALL abiGetIVectorVersion(InterfaceVersion& v) const lia_NOEXCEPT lia_OVERRIDE {
		v.major = 0;
		v.minor = 1;
	}

	virtual void lia_CALL abiDestroy() lia_NOEXCEPT lia_OVERRIDE {
		delete this;
	}

	virtual void lia_CALL abiClear() lia_NOEXCEPT lia_OVERRIDE {
		m_vector.clear();
	}

	virtual abi_bool_t lia_CALL abiReserve(abi_size_t n) lia_NOEXCEPT lia_OVERRIDE {
		lia_TRY
			m_vector.reserve(static_cast<std::size_t>(n));
		lia_CATCHALL(return abi_false)
		return abi_true;
	}

	virtual abi_bool_t lia_CALL abiInsert(abi_size_t idx, typename lia::detail::MakeTypes<T>::ConstPointer& pElem) lia_NOEXCEPT lia_OVERRIDE {
		const std::size_t i = static_cast<std::size_t>(idx);
		lia_TRY
			if (i <= m_vector.size()) {
				m_vector.insert(m_vector.begin() + i, lia::detail::derefElemPtr(pElem));
			}
			else {
				return abi_false;
			}
		lia_CATCHALL(return abi_false)
		return abi_true;
	}

	virtual abi_bool_t lia_CALL abiRemove(abi_size_t idx) lia_NOEXCEPT lia_OVERRIDE {
		const std::size_t i = static_cast<std::size_t>(idx);
		if (i >= m_vector.size()) {
			return abi_false;
		}
		lia_TRY
				m_vector.erase(m_vector.begin() + i);
		lia_CATCHALL(return abi_false)
		return abi_true;
	}

	virtual abi_size_t lia_CALL abiGetSize() const lia_NOEXCEPT lia_OVERRIDE {
		return static_cast<abi_size_t>(m_vector.size());
	}

	virtual abi_bool_t lia_CALL abiGetAt(abi_size_t idx, typename lia::detail::MakeTypes<T>::Pointer& pElem) lia_NOEXCEPT lia_OVERRIDE {
		const std::size_t i = static_cast<std::size_t>(idx);
		if (i < m_vector.size()) {
			lia::detail::assignElemPtr(pElem, m_vector[i]);
			return abi_true;
		}
		return abi_false;
	}

	virtual abi_bool_t lia_CALL abiGetAtConst(abi_size_t idx, typename lia::detail::MakeTypes<T>::ConstPointer& pElem) const lia_NOEXCEPT lia_OVERRIDE {
		const std::size_t i = static_cast<std::size_t>(idx);
		if (i < m_vector.size()) {
			lia::detail::assignElemPtr(pElem, m_vector[i]);
			return abi_true;
		}
		return abi_false;
	}

	virtual void lia_CALL abiConstructIterator(abi_bool_t atBegin, void* pBuf) lia_NOEXCEPT lia_OVERRIDE {
		typedef typename lia::detail::RemoveReference<TVector>::type::iterator TIterator;
		new (pBuf) VectorIteratorRef<T, TIterator>(atBegin ? m_vector.begin() : m_vector.end());
	}

	virtual void lia_CALL abiConstructConstIterator(abi_bool_t atBegin, void* pBuf) const lia_NOEXCEPT lia_OVERRIDE {
		typedef typename lia::detail::RemoveReference<TVector>::type::const_iterator TIterator;
		new (pBuf) VectorIteratorRef<const T, TIterator>(atBegin ? m_vector.begin() : m_vector.end());
	}

private:
	TVector m_vector; // either a reference or a type
};

namespace detail {

template<typename A, typename B>
struct MakeInterface< std::vector<A, B> > {
	typedef lia::IVector<typename MakeInterface<A>::Type> Type;
};

template<typename A, typename B>
struct MakeRef< std::vector<A, B> > {
	typedef lia::VectorRef< typename lia::detail::MakeInterface<A>::Type, std::vector<A, B>& > Type;
};

template<typename A, typename B>
typename MakeRef< std::vector<A, B> >::Type makeRef(std::vector<A, B>& v) {
	return typename MakeRef< std::vector<A, B> >::Type(v);
}

}

}

#else /* C compiler */

/* Not yet implemented */

#endif

#include <lia/detail/PopWarnings.h>

#endif
