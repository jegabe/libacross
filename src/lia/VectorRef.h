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
#ifndef lia_VectorRef_h_INCLUDED
#define lia_VectorRef_h_INCLUDED

#include <lia/IVector.h>
#include <lia/detail/VectorProxy.h>
#include <lia/detail/PushWarnings.h>

#ifdef __cplusplus

namespace lia {

template<typename T, typename TIterator>
class VectorIteratorRef lia_FINAL: public IVectorIterator<T> {
public:

	typedef VectorIteratorRef<T, TIterator> ThisType;

	VectorIteratorRef() lia_NOEXCEPT: m_iter(), m_isConst(true) {}
	explicit VectorIteratorRef(const TIterator& i, abi_bool_t isConst) lia_NOEXCEPT :m_iter(i), m_isConst(isConst) {}
	virtual ~VectorIteratorRef() lia_NOEXCEPT {}

	virtual void lia_CALL abiGetIVectorIteratorVersion(InterfaceVersion& v) const lia_NOEXCEPT lia_OVERRIDE {
		v.major = 0;
		v.minor = 1;
	}

	virtual void lia_CALL abiCloneTo(void* pBuf) lia_NOEXCEPT lia_OVERRIDE {
		new(pBuf) ThisType(m_iter, m_isConst);
	}

	virtual void lia_CALL abiFinalize() lia_NOEXCEPT lia_OVERRIDE {
		m_iter.~TIterator();
	}

	virtual abi_ptrdiff_t abiGetDistance(const IVectorIterator<T>& other_) const lia_NOEXCEPT lia_OVERRIDE {
		const ThisType& other = static_cast<const ThisType&>(other_);
		return static_cast<abi_ptrdiff_t>(m_iter - other.m_iter);
	}

	virtual void lia_CALL abiAdvance(abi_ptrdiff_t n) lia_NOEXCEPT lia_OVERRIDE {
		m_iter += static_cast<std::ptrdiff_t>(n);
	}

	virtual abi_bool_t lia_CALL abiDereference(typename lia::detail::MakeTypes<T>::Pointer& pElem) lia_NOEXCEPT lia_OVERRIDE {
		if (m_isConst) {
			return false; // Attempt to deref a const iterator in non-const "mode"
		}
		lia::detail::assignElemPtr(pElem, *m_iter);
		return abi_true;
	}

	virtual void lia_CALL abiDereferenceConst(typename lia::detail::MakeTypes<T>::ConstPointer& pElem) const lia_NOEXCEPT lia_OVERRIDE {
		lia::detail::assignElemPtr(pElem, *m_iter);
	}

private:
	TIterator  m_iter;
	abi_bool_t m_isConst;
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

	virtual void lia_CALL abiConstructIterator(abi_bool_t atBegin, abi_bool_t asConstIterator, void* pBuf) const lia_NOEXCEPT lia_OVERRIDE {
		typedef typename lia::RemoveReference<TVector>::type::iterator TIterator;
		ThisType& rThis = const_cast<ThisType&>(*this);
		new (pBuf) VectorIteratorRef<T, TIterator>(atBegin ? rThis.m_vector.begin() : rThis.m_vector.end(), asConstIterator);
	}

private:
	TVector m_vector; // either a reference or a type
};

namespace detail {

template<typename A, typename B>
struct MakeInterface< std::vector<A, B> > {
	typedef lia::IVector<typename MakeInterface<A>::Type> Type;
};

}

template<typename A, typename B>
struct MakeRef< std::vector<A, B> > {
	typedef lia::VectorRef< typename lia::detail::MakeInterface<A>::Type, std::vector<A, B>& > Type;
};

template<typename A, typename B>
typename MakeRef< std::vector<A, B> >::Type makeRef(std::vector<A, B>& v) {
	return typename MakeRef< std::vector<A, B> >::Type(v);
}

}

#else /* C compiler */

/* Not yet implemented */

#endif

#include <lia/detail/PopWarnings.h>

#endif
