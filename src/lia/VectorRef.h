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

template<typename T, typename TVector>
class VectorRef lia_FINAL: public IVector<T> {
public:

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

	virtual abi_bool_t lia_CALL abiInsert(abi_size_t idx, typename lia::detail::MakeTypes<T>::ConstPointer pElem) lia_NOEXCEPT lia_OVERRIDE {
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
