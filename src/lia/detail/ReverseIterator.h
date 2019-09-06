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
#ifndef lia_detail_ReverseIterator_h_INCLUDED
#define lia_detail_ReverseIterator_h_INCLUDED

#ifdef __cplusplus
	#include <iterator>
#endif

#include <lia/detail/PushWarnings.h>

#ifdef __cplusplus

namespace lia {
namespace detail {

// Re-implementation of std::reverse_iterator, needed because the proxy objects (for example lia::detail::VectorProxy) act different
// to pointers and so some functions, such as reverse_iterator::operator->, don't work on all platforms/tool chains.
// This implementation is aware of the quirks of libacross, so this one is used instead of std::reverse_iterator for several r[c]begin()/r[c]end() implementations.
template<typename TIter>
class ReverseIterator
{
public:

	typedef ReverseIterator<TIter> ThisType;

	// API according to https://en.cppreference.com/w/cpp/iterator/reverse_iterator

	typedef TIter                                                    iterator_type;
	typedef typename std::iterator_traits<TIter>::iterator_category  iterator_category;
	typedef typename std::iterator_traits<TIter>::value_type	     value_type;
	typedef typename std::iterator_traits<TIter>::difference_type    difference_type;
	typedef typename std::iterator_traits<TIter>::pointer            pointer;
	typedef typename std::iterator_traits<TIter>::reference          reference;

	ReverseIterator() {}

	explicit ReverseIterator(TIter iter): m_impl(iter) {}

	template<class U>
	ReverseIterator(const ReverseIterator<U>& iter): m_impl(iter) {}

	template<class U>
	ThisType& operator=(const ReverseIterator<U>& other) {
		m_impl = other.m_impl;
		return *this;
	}

	TIter base() const {
		return m_impl;
	}

	reference operator*() const {
		TIter tmp = m_impl;
		(void)--tmp;
		return *tmp;
	}

	pointer operator->() const {
		TIter tmp = m_impl;
		(void)--tmp;
		return tmp.operator->();
	}

	ReverseIterator& operator++() {
		(void)--m_impl;
		return *this;
	}

	ReverseIterator& operator++(int) {
		ThisType tmp = *this;
		(void)--m_impl;
		return tmp;
	}

	ReverseIterator& operator--() {
		(void)++m_impl;
		return *this;
	}

	ReverseIterator& operator--(int) {
		ThisType tmp = *this;
		(void)++m_impl;
		return tmp;
	}

	bool operator==(const ThisType& other) const {
		return m_impl == other.m_impl;
	}

	bool operator!=(const ThisType& other) const {
		return m_impl != other.m_impl;
	}

	ThisType& operator+=(difference_type i) {
		m_impl -= i;
		return *this;
	}

	ThisType operator+(difference_type i) const {
		return (ThisType(m_impl - i));
	}

	ThisType& operator-=(difference_type i) {
		m_impl += i;
		return *this;
	}

	ThisType operator-(difference_type i) const {
		return (ThisType(m_impl + i));
	}

	reference operator[](difference_type i) const {
		return (*(*this + i));
	}

private:
	TIter m_impl;
};

}
}

#endif

#include <lia/detail/PopWarnings.h>

#endif
