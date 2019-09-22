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
#ifndef lia_detail_VectorApiMixin_h_INCLUDED
#define lia_detail_VectorApiMixin_h_INCLUDED

#ifdef __cplusplus
	#include <new>
	#include <stdexcept>
	#include <vector>
	#include <utility>
#endif
#include <lia/defs.h>
#include <lia/detail/ReverseIterator.h>
#include <lia/detail/PushWarnings.h>

#ifdef __cplusplus

namespace lia {
namespace detail {

// Mix-in class for adding public RandomAccessIterator API into sub-class.
template<typename T,
         typename TSubClass,
         typename TInterface,
         typename TReference,
         typename TPointer>
class VectorIteratorApiMixin {
public:

	// API according to https://en.cppreference.com/w/cpp/iterator/iterator_traits

	typedef std::ptrdiff_t                  difference_type;
	typedef T                               value_type;
	typedef TPointer                        pointer;
	typedef TReference                      reference;
	typedef std::random_access_iterator_tag iterator_category;

	TReference operator*() const {
		const TInterface& rThis = downCast().getAbi();
		TPointer pElem;
		rThis.abiDereference(pElem, 0);
		return derefElemPtr(pElem);
	}

	TPointer operator->() const {
		const TInterface& rThis = downCast().getAbi();
		TPointer pElem;
		rThis.abiDereference(pElem, 0);
		return pElem;
	}

	TReference operator[](std::ptrdiff_t i) const {
		const TInterface& rThis = downCast().getAbi();
		TPointer pElem;
		rThis.abiDereference(pElem, static_cast<abi_ptrdiff_t>(i));
		return derefElemPtr(pElem);
	}

	TSubClass& operator++() {
		TInterface& rThis = downCast().getAbi();
		rThis.abiAdvance(1);
		return downCast();
	}

	TSubClass& operator--() {
		TInterface& rThis = downCast().getAbi();
		rThis.abiAdvance(-1);
		return downCast();
	}
	
	template<typename TSub = TSubClass> 
	typename lia::detail::EnableIf<!lia::detail::IsLiaInterface<TSub>::value, TSub>::type operator++(int) {
		TInterface& rThis = downCast().getAbi();
		TSub result = downCast();
		rThis.abiAdvance(1);
		return result;
	}

	template<typename TSub = TSubClass> 
	typename lia::detail::EnableIf<!lia::detail::IsLiaInterface<TSub>::value, TSub>::type operator--(int) {
		TInterface& rThis = downCast().getAbi();
		TSub result = downCast();
		rThis.abiAdvance(-1);
		return result;
	}

	template<typename TSub = TSubClass> 
	typename lia::detail::EnableIf<!lia::detail::IsLiaInterface<TSub>::value, TSub>::type operator+(std::ptrdiff_t i) {
		TSub result = downCast();
		result.getAbi().abiAdvance(static_cast<abi_ptrdiff_t>(i));
		return result;
	}

	template<typename TSub = TSubClass> 
	typename lia::detail::EnableIf<!lia::detail::IsLiaInterface<TSub>::value, TSub>::type operator-(std::ptrdiff_t i) {
		TSub result = downCast();
		result.getAbi().abiAdvance(static_cast<abi_ptrdiff_t>(-i));
		return result;
	}

	std::ptrdiff_t operator-(const TSubClass& other) const {
		const TInterface& rThis = downCast().getAbi();
		const std::ptrdiff_t result = rThis.abiGetDistance(other.getAbi());
		return result;
	}

	TSubClass& operator+=(std::ptrdiff_t i) {
		TInterface& rThis = downCast().getAbi();
		rThis.abiAdvance(static_cast<abi_ptrdiff_t>(i));
		return downCast();
	}

	TSubClass& operator-=(std::ptrdiff_t i) {
		TInterface& rThis = downCast().getAbi();
		rThis.abiAdvance(static_cast<abi_ptrdiff_t>(-i));
		return downCast();
	}

	bool operator==(const TSubClass& other) const lia_NOEXCEPT {
		const TInterface& rThis = downCast().getAbi();
		const abi_ptrdiff_t distance = rThis.abiGetDistance(other.getAbi());
		return (distance == 0);
	}

	bool operator!=(const TSubClass& other) const lia_NOEXCEPT {
		return !(*this == other);
	}

private:

	TSubClass& downCast() lia_NOEXCEPT {
		return static_cast<TSubClass&>(*this);
	}

	const TSubClass& downCast() const lia_NOEXCEPT {
		return static_cast<const TSubClass&>(*this);
	}
};

lia_STATIC_ASSERT(sizeof(VectorIteratorApiMixin<int, int, int, int&, int*>) == 1u, "API class is not allowed to contain any virtual functions")

// Mix-in class for adding public std::vector compatible API into sub-class.
template<typename T,
         typename TSubClass,
         typename TInterface,
         typename TReference,
         typename TPointer,
         typename TConstReference,
         typename TConstPointer,
         typename TIterator,
         typename TConstIterator>
class VectorApiMixin {
public:

	// API according to https://en.cppreference.com/w/cpp/container/vector with following restrictions or modifications:
	// - no public 'allocator' typedef since there is none
	// - the types for 'pointer', 'const_pointer', 'reference' and 'const_reference' may be proxy objects acting like a
	//   reference or pointer that help delegating calls to the other side of the ABI boundary. Proxy objects are used
	//   when the type 'T' which is accessed, is some interface 'lia::ISomething'
	// - no support for assignment to nested std::initializer_list objects (unnested initializer_list<T> works!)
	// - no support for the data() function when type T is another 'ISomeInterface'
	// - no support for range-based for loop with references "for(auto& x: cont) {}" when T is is another 'ISomeInterface'.
	//   Using values instead of references works though, or alternatively 
	//   the member typedefs "reference" or "const_reference" of the container. The macros lia_ELEM_REF and lia_ELEM_CONST_REF
	//   return references for a specified container (when using C++11 or higher)

	typedef T                                            value_type;
	typedef std::size_t                                  size_type;
	typedef std::ptrdiff_t                               difference_type;
	typedef TReference                                   reference;
	typedef TConstReference                              const_reference;
	typedef TPointer                                     pointer;
	typedef TConstPointer                                const_pointer;
	typedef TIterator                                    iterator;
	typedef TConstIterator                               const_iterator;
	typedef lia::detail::ReverseIterator<iterator>       reverse_iterator;
	typedef lia::detail::ReverseIterator<const_iterator> const_reverse_iterator;

#define lia_NONCONST_ITER typename lia::detail::IfThenElse<!lia::detail::IsSame<typename lia::detail::RemoveConst<iterator>::type, typename lia::detail::RemoveConst<const_iterator>::type>::value, iterator, lia::detail::Incomplete>::type

	template<typename U, typename V>
	VectorApiMixin& operator=(const std::vector<U, V>& v) {
		TInterface& rThis = downCast().getAbi();
		rThis.abiClear();
		if (!rThis.abiReserve(static_cast<abi_size_t>(v.size()))) {
			lia_THROW0(std::bad_alloc);
		}
		abi_size_t newPos = 0;
		typename lia::detail::MakeTypes<T>::ConstPointer pElem;
		for (typename std::vector<U, V>::const_iterator iter = v.begin(); iter != v.end(); ++iter) {
			assignElemPtr(pElem, *iter);
			if (!rThis.abiInsert(newPos, pElem)) {
				rThis.abiClear();
				lia_THROW0(std::bad_alloc);
			}
			++newPos;
		}
		return *this;
	}

	VectorApiMixin& operator=(const TInterface& v) {
		TInterface& rThis = downCast().getAbi();
		if (&v != &rThis) {
			rThis.abiClear();
			const abi_size_t vSize = v.abiGetSize();
			if (!rThis.abiReserve(vSize)) {
				lia_THROW0(std::bad_alloc);
			}
			abi_size_t newPos = 0;
			typename lia::detail::MakeTypes<T>::ConstPointer pElem;
			for (abi_size_t i=0; i<vSize; ++i) {
				v.abiGetAtConst(i, pElem);
				if (!rThis.abiInsert(i, pElem)) {
					rThis.abiClear();
					lia_THROW0(std::bad_alloc);
				}
			}
		}
		return *this;
	}

#if lia_CPP11_API

	template<typename U>
	VectorApiMixin& operator=(std::initializer_list<U> v) {
		lia_STATIC_ASSERT((!lia::detail::IsInitializerList<U>::value), "No nested std::initializer_list support in assignment")
		TInterface& rThis = downCast().getAbi();
		rThis.abiClear();
		if (!rThis.abiReserve(static_cast<abi_size_t>(v.size()))) {
			lia_THROW0(std::bad_alloc);
		}
		abi_size_t newPos = 0;
		typename lia::detail::MakeTypes<T>::ConstPointer pElem;
		for (auto iter = v.begin(); iter != v.end(); ++iter) {
			assignElemPtr(pElem, *iter);
			if (!rThis.abiInsert(newPos, pElem)) {
				rThis.abiClear();
				lia_THROW0(std::bad_alloc);
			}
			++newPos;
		}
		return *this;
	}

#endif

	template<typename U>
	void assign(std::size_t count, const U& value) {
		TInterface& rThis = downCast().getAbi();
		rThis.abiClear();
		if (!rThis.abiReserve(static_cast<abi_size_t>(count))) {
			lia_THROW0(std::bad_alloc);
		}
		typename lia::detail::MakeTypes<T>::ConstPointer pElem;
		assignElemPtr(pElem, value);
		abi_size_t newPos = 0;
		for (std::size_t i=0; i<count; ++i) {
			if (!rThis.abiInsert(newPos, pElem)) {
				rThis.abiClear();
				lia_THROW0(std::bad_alloc);
			}
			++newPos;
		}
	}

	template<class InputIt>
	void assign(InputIt first, InputIt last) {
		TInterface& rThis = downCast().getAbi();
		rThis.abiClear();
		typename lia::detail::MakeTypes<T>::ConstPointer pElem;
		abi_size_t newPos = 0;
		for (InputIt iter=first; iter != last; ++iter) {
			assignElemPtr(pElem, *iter);
			if (!rThis.abiInsert(newPos, pElem)) {
				rThis.abiClear();
				lia_THROW0(std::bad_alloc);
			}
			++newPos;
		}
	}

#if lia_CPP11_API

	template<typename U>
	void assign(const std::initializer_list<U> v) {
		(void)(*this = v);
	}

#endif

	TReference at(std::size_t pos) {
		TInterface& rThis = downCast().getAbi();
		TPointer pElem;
		if (!rThis.abiGetAt(static_cast<abi_size_t>(pos), pElem)) {
				lia_THROW1(std::out_of_range, "in at() call");
		}
		return derefElemPtr(pElem);
	}

	TConstReference at(std::size_t pos) const {
		const TInterface& rThis = downCast().getAbi();
		TConstPointer pElem;
		if (!rThis.abiGetAtConst(static_cast<abi_size_t>(pos), pElem)) {
				lia_THROW1(std::out_of_range, "in at() call");
		}
		return derefElemPtr(pElem);
	}

	TReference operator[](std::size_t pos) lia_NOEXCEPT {
		TInterface& rThis = downCast().getAbi();
		TPointer pElem;
		(void)rThis.abiGetAt(static_cast<abi_size_t>(pos), pElem);
		return derefElemPtr(pElem);
	}

	TConstReference operator[](std::size_t pos) const lia_NOEXCEPT {
		const TInterface& rThis = downCast().getAbi();
		TConstPointer pElem;
		(void)rThis.abiGetAtConst(static_cast<abi_size_t>(pos), pElem);
		return derefElemPtr(pElem);
	}

	TReference front() lia_NOEXCEPT {
		TInterface& rThis = downCast().getAbi();
		TPointer pElem;
		(void)rThis.abiGetAt(static_cast<abi_size_t>(0), pElem);
		return derefElemPtr(pElem);
	}

	TConstReference front() const lia_NOEXCEPT {
		const TInterface& rThis = downCast().getAbi();
		TConstPointer pElem;
		(void)rThis.abiGetAtConst(static_cast<abi_size_t>(0), pElem);
		return derefElemPtr(pElem);
	}

	TReference back() lia_NOEXCEPT {
		TInterface& rThis = downCast().getAbi();
		TPointer pElem;
		const abi_size_t lastPos = static_cast<abi_size_t>(rThis.abiGetSize() - 1u);
		(void)rThis.abiGetAt(lastPos, pElem);
		return derefElemPtr(pElem);
	}

	TConstReference back() const lia_NOEXCEPT {
		const TInterface& rThis = downCast().getAbi();
		TConstPointer pElem;
		const abi_size_t lastPos = static_cast<abi_size_t>(rThis.abiGetSize() - 1u);
		(void)rThis.abiGetAtConst(lastPos, pElem);
		return derefElemPtr(pElem);
	}

	template<typename U = T> 
	typename lia::detail::EnableIf<!lia::detail::IsLiaInterface<U>::value, U*>::type data() lia_NOEXCEPT {
		TInterface& rThis = downCast().getAbi();
		U* pResult;
		if (!rThis.abiGetAt(0, pResult)) {
			pResult = lia_NULLPTR;
		}
		return pResult;
	}

	template<typename U = T> 
	typename lia::detail::EnableIf<!lia::detail::IsLiaInterface<U>::value, const U*>::type data() const lia_NOEXCEPT {
		const TInterface& rThis = downCast().getAbi();
		const U* pResult;
		if (!rThis.abiGetAtConst(0, pResult)) {
			pResult = lia_NULLPTR;
		}
		return pResult;
	}

	TIterator begin() lia_NOEXCEPT {
		TInterface& rThis = downCast().getAbi();
		TIterator iter;
		rThis.abiConstructIterator(abi_true, iter.getBuffer());
		iter.setConstructed();
		return iter;
	}

	TConstIterator begin() const lia_NOEXCEPT {
		const TInterface& rThis = downCast().getAbi();
		TConstIterator iter;
		rThis.abiConstructConstIterator(abi_true, iter.getBuffer());
		iter.setConstructed();
		return iter;
	}

	TConstIterator cbegin() const lia_NOEXCEPT {
		return begin();
	}

	TIterator end() lia_NOEXCEPT {
		TInterface& rThis = downCast().getAbi();
		TIterator iter;
		rThis.abiConstructIterator(abi_false, iter.getBuffer());
		iter.setConstructed();
		return iter;
	}

	TConstIterator end() const lia_NOEXCEPT {
		const TInterface& rThis = downCast().getAbi();
		TConstIterator iter;
		rThis.abiConstructConstIterator(abi_false, iter.getBuffer());
		iter.setConstructed();
		return iter;
	}

	TConstIterator cend() const lia_NOEXCEPT {
		return end();
	}

	reverse_iterator rbegin() lia_NOEXCEPT {
		return reverse_iterator(end());
	}

	reverse_iterator rend() lia_NOEXCEPT {
		return reverse_iterator(begin());
	}

	const_reverse_iterator rbegin() const lia_NOEXCEPT {
		return const_reverse_iterator(end());
	}

	const_reverse_iterator rend() const lia_NOEXCEPT {
		return const_reverse_iterator(begin());
	}

	const_reverse_iterator rcbegin() const lia_NOEXCEPT {
		return const_reverse_iterator(end());
	}

	const_reverse_iterator rcend() const lia_NOEXCEPT {
		return const_reverse_iterator(begin());
	}

	bool empty() const lia_NOEXCEPT {
		const TInterface& rThis = downCast().getAbi();
		return (rThis.abiGetSize() == 0);
	}

	std::size_t size() const lia_NOEXCEPT {
		const TInterface& rThis = downCast().getAbi();
		return static_cast<std::size_t>(rThis.abiGetSize());
	}

	std::size_t max_size() const lia_NOEXCEPT {
		return static_cast<std::size_t>(-1);
	}

	void reserve(std::size_t n) {
		TInterface& rThis = downCast().getAbi();
		if (!rThis.abiReserve(static_cast<abi_size_t>(n))) {
			lia_THROW0(std::bad_alloc);
		}
	}

	std::size_t capacity() const lia_NOEXCEPT {
		const TInterface& rThis = downCast().getAbi();
		abi_size_t result = 0;
		(void)rThis.abiGetSize(&result);
		return result;
	}

	void shrink_to_fit() lia_NOEXCEPT {
		const TInterface& rThis = downCast().getAbi();
		rThis = downCast().getAbi();
		(void)rThis.abiReserve(0, abi_true);
	}

	void clear() lia_NOEXCEPT {
		TInterface& rThis = downCast().getAbi();
		rThis.abiClear();
	}

	iterator insert(const_iterator pos, const T& value) {
		const std::ptrdiff_t distanceFromBegin = pos - cbegin();
		return insertImpl(distanceFromBegin, 1u, value);
	}

	iterator insert(lia_NONCONST_ITER pos, const T& value) {
		const std::ptrdiff_t distanceFromBegin = pos - begin();
		return insertImpl(distanceFromBegin, 1u, value);
	}

	iterator insert(const_iterator pos, std::size_t count, const T& value) {
		const std::ptrdiff_t distanceFromBegin = pos - cbegin();
		return insertImpl(distanceFromBegin, count, value);
	}

	iterator insert(lia_NONCONST_ITER pos, std::size_t count, const T& value) {
		const std::ptrdiff_t distanceFromBegin = pos - begin();
		return insertImpl(distanceFromBegin, count, value);
	}

	template<class InputIt>
	iterator insert(const_iterator pos, InputIt first, InputIt last) {
		const std::ptrdiff_t distanceFromBegin = pos - cbegin();
		return insertImpl<InputIt>(distanceFromBegin, first, last);
	}

	template<class InputIt>
	iterator insert(lia_NONCONST_ITER pos, InputIt first, InputIt last) {
		const std::ptrdiff_t distanceFromBegin = pos - begin();
		return insertImpl<InputIt>(distanceFromBegin, first, last);
	}

#if lia_CPP11_API

	iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
		const std::ptrdiff_t distanceFromBegin = pos - cbegin();
		return insertImpl(distanceFromBegin, ilist.begin(), ilist.end());
	}

	template<typename... Args> 
	iterator emplace(const_iterator pos, Args&&... args) {
		return insert(pos, T(std::forward<Args>(args)...));
	}

#endif

	iterator erase(const_iterator pos) {
		const std::ptrdiff_t distanceFromBegin = pos - cbegin();
		return eraseImpl(distanceFromBegin, 1u);
	}

	iterator erase(lia_NONCONST_ITER pos) {
		const std::ptrdiff_t distanceFromBegin = pos - begin();
		return eraseImpl(distanceFromBegin, 1u);
	}

	iterator erase(const_iterator first, const_iterator last) {
		const std::ptrdiff_t distanceFromBegin = first - cbegin();
		const std::size_t n = static_cast<std::size_t>(last - first);
		return eraseImpl(distanceFromBegin, n);
	}

	iterator erase(lia_NONCONST_ITER first, lia_NONCONST_ITER last) {
		const std::ptrdiff_t distanceFromBegin = first - begin();
		const std::size_t n = static_cast<std::size_t>(last - first);
		return eraseImpl(distanceFromBegin, n);
	}

	void push_back(const T& value) {
		const TInterface& rThis = downCast().getAbi();
		typename lia::detail::MakeTypes<T>::ConstPointer pElem;
		assignElemPtr(pElem, value);
		const abi_size_t size = rThis.abiGetSize();
		if (!rThis.abiInsert(size, pElem)) {
			lia_THROW0(std::bad_alloc);
		}
	}

#if lia_CPP11_API

	void push_back(T&& value) {
		const T& tmp = value;
		push_back(tmp);
	}

#endif

	// helper functions

	template<typename U, typename V>
	operator std::vector<U, V>() const lia_NOEXCEPT {
		const TInterface& rThis = downCast().getAbi();
		std::vector<U, V> v;
		const abi_size_t sz = rThis.abiGetSize();
		v.reserve(static_cast<std::size_t>(sz));
		TConstPointer pElem;
		for (abi_size_t i=0; i<sz; ++i) {
			(void)rThis.abiGetAtConst(i, pElem);
			v.push_back(derefElemPtr(pElem));
		}
		return v;
	}

private:

	iterator insertImpl(std::ptrdiff_t distanceFromBegin, std::size_t n, const T& value) {
		TInterface& rThis = downCast().getAbi();
		if (distanceFromBegin < 0) {
				lia_THROW1(std::out_of_range, "in insert() call");
		}
		if (!rThis.abiReserve(static_cast<abi_size_t>(n))) {
			lia_THROW0(std::bad_alloc);
		}
		typename lia::detail::MakeTypes<T>::ConstPointer pElem;
		assignElemPtr(pElem, value);
		std::ptrdiff_t j = distanceFromBegin;
		for (std::size_t i=0; i<n; ++i, ++j) {
			if (!rThis.abiInsert(static_cast<abi_size_t>(j), pElem)) {
				lia_THROW0(std::bad_alloc);
			}
		}
		return begin() + distanceFromBegin;
	}

	template<class InputIt>
	iterator insertImpl(std::ptrdiff_t distanceFromBegin, InputIt first, InputIt last) {
		TInterface& rThis = downCast().getAbi();
		if (distanceFromBegin < 0) {
				lia_THROW1(std::out_of_range, "in insert() call");
		}
		const std::size_t n = static_cast<std::size_t>(std::distance<InputIt>(first, last));
		if (!rThis.abiReserve(static_cast<abi_size_t>(n))) {
			lia_THROW0(std::bad_alloc);
		}
		typename lia::detail::MakeTypes<T>::ConstPointer pElem;
		std::ptrdiff_t j = distanceFromBegin;
		for (InputIt iter = first; iter != last; ++iter, ++j) {
			assignElemPtr(pElem, *iter);
			if (!rThis.abiInsert(static_cast<abi_size_t>(j), pElem)) {
				lia_THROW0(std::bad_alloc);
			}
		}
		return begin() + distanceFromBegin;
	}

	iterator eraseImpl(std::ptrdiff_t distanceFromBegin, std::size_t n) {
		if (distanceFromBegin < 0) {
				lia_THROW1(std::out_of_range, "in erase() call");
		}
		TInterface& rThis = downCast().getAbi();
		if (!rThis.abiRemove(static_cast<abi_size_t>(distanceFromBegin), static_cast<abi_size_t>(n))) {
				lia_THROW1(std::out_of_range, "in erase() call");
		}
		return begin() + distanceFromBegin;
	}

	TSubClass& downCast() lia_NOEXCEPT {
		return static_cast<TSubClass&>(*this);
	}

	const TSubClass& downCast() const lia_NOEXCEPT {
		return static_cast<const TSubClass&>(*this);
	}

	#undef lia_NONCONST_ITER
};

}
}

#else /* C compiler */

/* Not yet implemented */

#endif

#include <lia/detail/PopWarnings.h>

#endif
