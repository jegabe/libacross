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
#ifndef lia_ISharedPtr_h_INCLUDED
#define lia_ISharedPtr_h_INCLUDED

#include <lia/detail/SharedPtrApiMixin.h>
#include <lia/detail/PushWarnings.h>

#ifdef __cplusplus

namespace lia {

namespace detail {
	
typedef void* (*SharedPtrDerefFunction)(const void*);
typedef void  (*SharedPtrDestructFunction)(void*);
typedef void  (*SharedPtrCopyConstructFunction)(const void*, void*);
typedef void  (*SharedPtrMoveConstructFunction)(void*, void*);

template<int dummy>
void* emptySharedPtrDerefImpl(const void*) {
	return lia_NULLPTR;
}

template<int dummy>
void emptySharedPtrDestructImpl(void*) {
}

template<int dummy>
void emptySharedPtrCopyConstructImpl(const void*, void*) {
}

template<int dummy>
void emptySharedPtrMoveConstructImpl(const void*, void*) {
}

template<typename TSharedPtr>
void* sharedPtrDerefImpl(const void* pBuf) {
	const TSharedPtr& rSharedPtr = *reinterpret_cast<const TSharedPtr*>(pBuf);
	return rSharedPtr.get();
}

template<typename TSharedPtr>
void sharedPtrDestructImpl(void* pBuf) {
	TSharedPtr& rSharedPtr = *reinterpret_cast<TSharedPtr*>(pBuf);
	rSharedPtr.~TSharedPtr();
}

template<typename TSharedPtr>
void sharedPtrCopyConstructImpl(const void* pSrc, void* pDst) {
	const TSharedPtr& rSharedPtr = *reinterpret_cast<const TSharedPtr*>(pSrc);
	new(pDst) TSharedPtr(rSharedPtr);
}

template<typename TSharedPtr>
void sharedPtrMoveConstructImpl(void* pSrc, void* pDst) {
	TSharedPtr& rSharedPtr = *reinterpret_cast<TSharedPtr*>(pSrc);
#if lia_CPP11_API
	new(pDst) TSharedPtr(std::move(rSharedPtr));
#else
	new(pDst) TSharedPtr(rSharedPtr);
#endif
}

}

#define lia_ISharedPtr_BASE(T) lia::detail::SharedPtrApiMixin<T, \
                                                              lia::ISharedPtr<T> \
                                                              >

//! Interface version history:
//!
//! semver | notes
//! ------ | --------
//! 0.1    | Pre-release version
//!
template<typename T>
class ISharedPtr: public lia_ISharedPtr_BASE(T) {
public:

	typedef ISharedPtr<T> ThisType;

	void operator delete(void* p) {
		if (p != lia_NULLPTR) {
			static_cast<ThisType*>(p)->abiDestroy();
		}
	}

	ThisType& getAbi() lia_NOEXCEPT {
		return *this;
	}

	const ThisType& getAbi() const lia_NOEXCEPT {
		return *this;
	}

	/* vtable index 0 */ virtual void lia_CALL abiGetISharedPtrVersion(InterfaceVersion& v) const lia_NOEXCEPT = 0;
	/* vtable index 1 */ virtual void lia_CALL abiDestroy() lia_NOEXCEPT = 0;
	/* vtable index 2 */ virtual T*   lia_CALL abiDereference() const lia_NOEXCEPT = 0;
	/* vtable index 3 */ virtual void lia_CALL abiAssignToNewPtr(T* ptr) lia_NOEXCEPT = 0;
	/* vtable index 4 */ virtual void lia_CALL abiGetInternal(const void*& pPtr, lia::detail::SharedPtrDerefFunction& pDeref, lia::detail::SharedPtrDestructFunction& pDestruct, lia::detail::SharedPtrCopyConstructFunction& pCopyConstuct, lia::detail::SharedPtrMoveConstructFunction& pMove) const lia_NOEXCEPT = 0;

private:

	typedef lia_ISharedPtr_BASE(T) ApiBase;
};

#undef lia_ISharedPtr_BASE

lia_STATIC_ASSERT(sizeof(ISharedPtr<char>) == sizeof(void*), "Interface must be pure virtual")

template<typename T>
class SharedPtrRef lia_FINAL: public ISharedPtr<T> {
public:

	void operator delete(void* p) {
		::operator delete(p);
	}

	SharedPtrRef()
		:m_buf()
		,m_pDeref(&lia::detail::emptySharedPtrDerefImpl<T>)
		,m_pDestruct(&lia::detail::emptySharedPtrDestructImpl<0>)
		,m_pCopyConstruct(&lia::detail::emptySharedPtrCopyConstructImpl<0>)
		,m_pMoveConstruct(&lia::detail::emptySharedPtrMoveConstructImpl<0>)
	{
	}

	explicit SharedPtrRef(const std::shared_ptr<T>& p)
		:m_buf()
		,m_pDeref         (&lia::detail::sharedPtrDerefImpl<std::shared_ptr<T>>)
		,m_pDestruct      (&lia::detail::sharedPtrDestructImpl<std::shared_ptr<T>>)
		,m_pCopyConstruct (&lia::detail::sharedPtrCopyConstructImpl<std::shared_ptr<T>>)
		,m_pMoveConstruct (&lia::detail::sharedPtrMoveConstructImpl<std::shared_ptr<T>>)
	{
		static_assert(sizeof(std::shared_ptr<T>) <= lia::detail::kSharedPtrSize, "std::shared_ptr<T> is too big");
		new(m_buf.data) std::shared_ptr<T>(p);
	}


#ifdef lia_CPP11_API
	explicit SharedPtrRef(std::shared_ptr<T>&& p)
		:m_buf()
		,m_pDeref         (&lia::detail::sharedPtrDerefImpl<std::shared_ptr<T>>)
		,m_pDestruct      (&lia::detail::sharedPtrDestructImpl<std::shared_ptr<T>>)
		,m_pCopyConstruct (&lia::detail::sharedPtrCopyConstructImpl<std::shared_ptr<T>>)
		,m_pMoveConstruct (&lia::detail::sharedPtrMoveConstructImpl<std::shared_ptr<T>>)
	{
		static_assert(sizeof(std::shared_ptr<T>) <= lia::detail::kSharedPtrSize, "std::shared_ptr<T> is too big");
		new(m_buf.data) std::shared_ptr<T>(std::move(p));
	}
#endif

	explicit SharedPtrRef(const ISharedPtr<T>& other)
		:m_buf()
		,m_pDeref(lia_NULLPTR)
		,m_pDestruct(lia_NULLPTR)
		,m_pCopyConstruct(lia_NULLPTR)
		,m_pMoveConstruct(lia_NULLPTR)
	{
		const void* pSrc = lia_NULLPTR;
		other.abiGetInternal(pSrc, m_pDeref, m_pDestruct, m_pCopyConstruct, m_pMoveConstruct);
		(*m_pCopyConstruct)(pSrc, m_buf.data);
	}

#if lia_CPP11_API
	SharedPtrRef(ISharedPtr<T>&& other) noexcept
		:m_buf()
		,m_pDeref(lia_NULLPTR)
		,m_pDestruct(lia_NULLPTR)
		,m_pCopyConstruct(lia_NULLPTR)
		,m_pMoveConstruct(lia_NULLPTR)
	{
		const void* pSrc = lia_NULLPTR;
		other.abiGetInternal(pSrc, m_pDeref, m_pDestruct, m_pCopyConstruct, m_pMoveConstruct);
		(*m_pMoveConstruct)(const_cast<void*>(pSrc), m_buf.data);
	}
#endif

	SharedPtrRef(const SharedPtrRef& other)
		:m_buf()
		,m_pDeref(other.m_pDeref)
		,m_pDestruct(other.m_pDestruct)
		,m_pCopyConstruct(other.m_pCopyConstruct)
		,m_pMoveConstruct(other.m_pMoveConstruct)
	{
		(*m_pCopyConstruct)(other.m_buf.data, m_buf.data);
	}

#if lia_CPP11_API
	SharedPtrRef(SharedPtrRef&& other) noexcept
		:m_buf()
		,m_pDeref(other.m_pDeref)
		,m_pDestruct(other.m_pDestruct)
		,m_pCopyConstruct(other.m_pCopyConstruct)
		,m_pMoveConstruct(other.m_pMoveConstruct)
	{
		(*m_pMoveConstruct)(other.m_buf.data, m_buf.data);
	}
#endif

	SharedPtrRef& operator=(const SharedPtrRef& other) {
		if (&other != this) {
			(*m_pDestruct)(m_buf);
			m_pDeref = other.m_pDeref;
			m_pDestruct = other.m_pDestruct;
			m_pCopyConstruct = other.m_pCopyConstruct;
			m_pMoveConstruct = other.m_pMoveConstruct;
			(*m_pCopyConstruct)(other.m_buf.data, m_buf.data);
		}
		return *this;
	}

#if lia_CPP11_API
	SharedPtrRef& operator=(SharedPtrRef&& other) {
		(*m_pDestruct)(m_buf);
		m_pDeref = other.m_pDeref;
		m_pDestruct = other.m_pDestruct;
		m_pCopyConstruct = other.m_pCopyConstruct;
		m_pMoveConstruct = other.m_pMoveConstruct;
		(*m_pMoveConstruct)(other.m_buf.data, m_buf.data);
		return *this;
	}
#endif

	virtual ~SharedPtrRef()
	{
		(*m_pDestruct)(m_buf.data);
	}

	virtual void lia_CALL abiGetISharedPtrVersion(InterfaceVersion& v) const lia_NOEXCEPT lia_OVERRIDE {
		v.major = 0;
		v.minor = 1;
	}

	virtual void lia_CALL abiDestroy() lia_NOEXCEPT lia_OVERRIDE {
		delete this;
	}

	virtual T* lia_CALL abiDereference() const lia_NOEXCEPT lia_OVERRIDE {
		return reinterpret_cast<T*>((*m_pDeref)(m_buf.data));
	}

	virtual void lia_CALL abiAssignToNewPtr(T* ptr) lia_NOEXCEPT lia_OVERRIDE {
		(*m_pDestruct)(m_buf.data);
		static_assert(sizeof(std::shared_ptr<T>) <= lia::detail::kSharedPtrSize, "The type std::shared_ptr<T> is too big");
		new (m_buf.data) std::shared_ptr<T>(ptr);
		m_pDeref         = &lia::detail::sharedPtrDerefImpl<std::shared_ptr<T>>;
		m_pDestruct      = &lia::detail::sharedPtrDestructImpl<std::shared_ptr<T>>;
		m_pCopyConstruct = &lia::detail::sharedPtrCopyConstructImpl<std::shared_ptr<T>>;
		m_pMoveConstruct = &lia::detail::sharedPtrMoveConstructImpl<std::shared_ptr<T>>;
	}

	virtual void lia_CALL abiGetInternal(const void*& pPtr, lia::detail::SharedPtrDerefFunction& pDeref, lia::detail::SharedPtrDestructFunction& pDestruct, lia::detail::SharedPtrCopyConstructFunction& pCopyConstuct, lia::detail::SharedPtrMoveConstructFunction& pMoveConstruct) const lia_NOEXCEPT lia_OVERRIDE {
		pPtr = m_buf.data;
		pDeref = m_pDeref;
		pDestruct = m_pDestruct;
		pCopyConstuct = m_pCopyConstruct;
		pMoveConstruct = m_pMoveConstruct;
	}

private:

	union Data {
		memalign_t alignmentDummy;
		char data[lia::detail::kSharedPtrSize];
	} m_buf; // into this buffer, a std::shared_ptr can be constructed in

	lia::detail::SharedPtrDerefFunction         m_pDeref;
	lia::detail::SharedPtrDestructFunction      m_pDestruct;
	lia::detail::SharedPtrCopyConstructFunction m_pCopyConstruct;
	lia::detail::SharedPtrMoveConstructFunction m_pMoveConstruct;
};

#if 0
template<typename T, typename TSharedPtr>
class SharedPtrRef lia_FINAL: public ISharedPtr<T> {
public:

	typedef SharedPtrRef<T, TSharedPtr> ThisType;
	typedef typename lia::detail::RemoveReference<TSharedPtr>::type TSharedPtrValue;

	SharedPtrRef() lia_NOEXCEPT {}
	explicit SharedPtrRef(TSharedPtr& v) lia_NOEXCEPT :m_ptr(v) {}
	virtual ~SharedPtrRef() lia_NOEXCEPT {}

	void operator delete(void* p) {
		::operator delete(p);
	}

	virtual void lia_CALL abiGetISharedPtrVersion(InterfaceVersion& v) const lia_NOEXCEPT lia_OVERRIDE {
		v.major = 0;
		v.minor = 1;
	}

	virtual void lia_CALL abiDestroy() lia_NOEXCEPT lia_OVERRIDE {
		delete this;
	}

	virtual T* lia_CALL abiDereference() const lia_NOEXCEPT lia_OVERRIDE {
		return m_ptr.get();
	}

	virtual void lia_CALL abiAssignToNewPtr(T* ptr) lia_NOEXCEPT lia_OVERRIDE {
		m_ptr.reset(ptr);
	}

	virtual void lia_CALL abiGetInternal(const void*& pPtr, lia::detail::SharedPtrDerefFunction& pDeref, lia::detail::SharedPtrDestructFunction& pDestruct, lia::detail::SharedPtrCopyConstructFunction& pCopyConstuct, lia::detail::SharedPtrMoveConstructFunction& pMoveConstruct) const lia_NOEXCEPT lia_OVERRIDE {
		pPtr = &m_ptr;
		pDeref = &lia::detail::sharedPtrDerefImpl<TSharedPtrValue>;
		pDestruct = lia::detail::sharedPtrDestructImpl<TSharedPtrValue>;
		pCopyConstuct = &lia::detail::sharedPtrCopyConstructImpl<TSharedPtrValue>;
		pMoveConstruct = &lia::detail::sharedPtrMoveConstructImpl<TSharedPtrValue>;
	}

private:

	SharedPtrRef(const SharedPtrRef&); // forbidden
	SharedPtrRef& operator=(const SharedPtrRef&); // forbidden

	TSharedPtr m_ptr;
};
#endif

}

#else /* C compiler */

/* Not yet implemented */

#endif

#include <lia/detail/PopWarnings.h>

#endif
