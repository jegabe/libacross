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
#ifndef lia_detail_BasicStringApiMixin_h_INCLUDED
#define lia_detail_BasicStringApiMixin_h_INCLUDED

#include <lia/defs.h>
#include <lia/detail/PushWarnings.h>

#ifdef __cplusplus

#include <new>       // for std::bad_alloc
#include <stdexcept> // for std::out_of_range
#include <string>

namespace lia {
namespace detail {

// Mix-in class for adding public std::basic_string compatible API into sub-class.
// The class TSubClass that derives from this mixin is required to implement
// a public function getAbi() that returns a reference to some type that's
// equal or API compatible to IBasicString
template<typename T,
         typename TSubClass>
class BasicStringApiMixin {

public:

#if 0
	template<typename U, typename V>
	VectorApiMixin& operator=(const std::vector<U, V>& v) {
		lia_ABI.abiClear();
		if (!lia_ABI.abiReserve(static_cast<abi_size_t>(v.size()))) {
			lia_THROW0(std::bad_alloc);
		}
		abi_size_t newPos = 0;
		typename lia::detail::MakeTypes<T>::ConstPointer pElem;
		for (typename std::vector<U, V>::const_iterator iter = v.begin(); iter != v.end(); ++iter) {
			assignElemPtr(pElem, *iter);
			if (!lia_ABI.abiInsert(newPos, pElem)) {
				lia_ABI.abiClear();
				lia_THROW0(std::bad_alloc);
			}
			++newPos;
		}
		return *this;
	}

	template<typename U>
	void assign(std::size_t count, const U& value) {
		lia_ABI.abiClear();
		if (!lia_ABI.abiReserve(static_cast<abi_size_t>(count))) {
			lia_THROW0(std::bad_alloc);
		}
		typename lia::detail::MakeTypes<T>::ConstPointer pElem;
		assignElemPtr(pElem, value);
		abi_size_t newPos = 0;
		for (std::size_t i=0; i<count; ++i) {
			if (!lia_ABI.abiInsert(newPos, pElem)) {
				lia_ABI.abiClear();
				lia_THROW0(std::bad_alloc);
			}
			++newPos;
		}
	}

	template<class InputIt>
	void assign(InputIt first, InputIt last) {
		lia_ABI.abiClear();
		typename lia::detail::MakeTypes<T>::ConstPointer pElem;
		abi_size_t newPos = 0;
		for (InputIt iter=first; iter != last; ++iter) {
			assignElemPtr(pElem, *iter);
			if (!lia_ABI.abiInsert(newPos, pElem)) {
				lia_ABI.abiClear();
				lia_THROW0(std::bad_alloc);
			}
			++newPos;
		}
	}

	TReference at(std::size_t pos) {
		TPointer pElem;
		if (!lia_ABI.abiGetAt(static_cast<abi_size_t>(pos), pElem)) {
				lia_THROW1(std::out_of_range, "in at() call");
		}
		return derefElemPtr(pElem);
	}

	TConstReference at(std::size_t pos) const {
		TConstPointer pElem;
		if (!lia_ABI.abiGetAtConst(static_cast<abi_size_t>(pos), pElem)) {
				lia_THROW1(std::out_of_range, "in at() call");
		}
		return derefElemPtr(pElem);
	}

	TReference operator[](std::size_t pos) lia_NOEXCEPT {
		TPointer pElem;
		(void)lia_ABI.abiGetAt(static_cast<abi_size_t>(pos), pElem);
		return derefElemPtr(pElem);
	}

	TConstReference operator[](std::size_t pos) const lia_NOEXCEPT {
		TConstPointer pElem;
		(void)lia_ABI.abiGetAtConst(static_cast<abi_size_t>(pos), pElem);
		return derefElemPtr(pElem);
	}

	TReference front() lia_NOEXCEPT {
		TPointer pElem;
		(void)lia_ABI.abiGetAt(static_cast<abi_size_t>(0), pElem);
		return derefElemPtr(pElem);
	}

	TConstReference front() const lia_NOEXCEPT {
		TConstPointer pElem;
		(void)lia_ABI.abiGetAtConst(static_cast<abi_size_t>(0), pElem);
		return derefElemPtr(pElem);
	}

	TReference back() lia_NOEXCEPT {
		TPointer pElem;
		const abi_size_t lastPos = static_cast<abi_size_t>(lia_ABI.abiGetSize() - 1u);
		(void)lia_ABI.abiGetAt(lastPos, pElem);
		return derefElemPtr(pElem);
	}

	TConstReference back() const lia_NOEXCEPT {
		TConstPointer pElem;
		const abi_size_t lastPos = static_cast<abi_size_t>(lia_ABI.abiGetSize() - 1u);
		(void)lia_ABI.abiGetAtConst(lastPos, pElem);
		return derefElemPtr(pElem);
	}

	bool empty() const lia_NOEXCEPT {
		return (lia_ABI.abiGetSize() == 0);
	}

	std::size_t size() const lia_NOEXCEPT {
		return static_cast<std::size_t>(lia_ABI.abiGetSize());
	}

	template<typename U, typename V>
	operator std::vector<U, V>() const lia_NOEXCEPT {
		std::vector<U, V> v;
		return v;
	}
#endif

private:

	TSubClass& downCast() lia_NOEXCEPT {
		return static_cast<TSubClass&>(*this);
	}

	const TSubClass& downCast() const lia_NOEXCEPT {
		return static_cast<const TSubClass&>(*this);
	}
};

lia_STATIC_ASSERT(sizeof(BasicStringApiMixin<char, char>) == 1u, "API class is not allowed to contain any virtual functions")

}
}

#else /* C compiler */

/* Not yet implemented */

#endif

#include <lia/detail/PopWarnings.h>

#endif
