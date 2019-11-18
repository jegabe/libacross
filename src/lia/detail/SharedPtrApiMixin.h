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
#ifndef lia_detail_SharedPtrApiMixin_h_INCLUDED
#define lia_detail_SharedPtrApiMixin_h_INCLUDED

#include <lia/defs.h>
#include <lia/detail/PushWarnings.h>

#ifdef __cplusplus

#include <memory> // for std::shared_ptr

namespace lia {
namespace detail {

// Mix-in class for adding public std::basic_string compatible API into sub-class.
// The class TSubClass that derives from this mixin is required to implement
// a public function getAbi() that returns a reference to some type that's
// equal or API compatible to IBasicString
template<typename T,
         typename TSubClass>
class SharedPtrApiMixin {

public:

private:

	TSubClass& downCast() lia_NOEXCEPT {
		return static_cast<TSubClass&>(*this);
	}

	const TSubClass& downCast() const lia_NOEXCEPT {
		return static_cast<const TSubClass&>(*this);
	}
};

lia_STATIC_ASSERT(sizeof(SharedPtrApiMixin<char, char>) == 1u, "API class is not allowed to contain any virtual functions")

}
}

#else /* C compiler */

/* Not yet implemented */

#endif

#include <lia/detail/PopWarnings.h>

#endif
