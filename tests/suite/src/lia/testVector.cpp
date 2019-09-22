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
#include <memory>
#include <utility>
#include <gtest/gtest.h>
#include <lia/DllLoader.h>
#include <lia/IVector.h>

using namespace lia::dll_loader;
using namespace lia;
using namespace std;

namespace {

typedef IVector<int32_t>* (lia_CALL *CreateInt32VectorFunction)(void);
typedef IVector<IVector<int32_t>>* (lia_CALL *CreateInt32VectorVectorFunction)(void);

vector<pair<CreateInt32VectorFunction, CreateInt32VectorVectorFunction>> makeVectors() {
	vector<pair<CreateInt32VectorFunction, CreateInt32VectorVectorFunction>> result;
	auto creatorsSimple  = getDllFunctions<CreateInt32VectorFunction>("createInt32Vector");
	auto creatorsComplex = getDllFunctions<CreateInt32VectorVectorFunction>("createInt32VectorVector");
	if (creatorsSimple.size() == creatorsComplex.size()) {
		result.reserve(creatorsSimple.size());
		for (size_t i=0; i<creatorsSimple.size(); ++i) {
			result.push_back({creatorsSimple[i], creatorsComplex[i]});
		}
	}
	return result;
}

}

TEST(IVector, assignmentOperator) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*vecs[i].second)());
		auto& rVectorSimple  = *pVectorSimple;
		auto& rVectorComplex = *pVectorComplex;
		{ // test assignment to vector
			{
				vector<int32_t> v { 1, 2, 3 };
				rVectorSimple = v; // assignment to vector
				ASSERT_EQ(rVectorSimple.size(), 3);
				EXPECT_EQ(rVectorSimple[0], 1);
				EXPECT_EQ(rVectorSimple[1], 2);
				EXPECT_EQ(rVectorSimple[2], 3);
				rVectorSimple.clear();
				EXPECT_EQ(rVectorSimple.size(), 0);
			}
			{
				vector<vector<int32_t>> v { { 1 }, { 1, 2 }, { 1, 2, 3 }};
				rVectorComplex = v; // assignment to vector
				ASSERT_EQ(rVectorComplex.size(), 3);
				EXPECT_EQ(rVectorComplex[0].size(), 1);
				EXPECT_EQ(rVectorComplex[1].size(), 2);
				EXPECT_EQ(rVectorComplex[2].size(), 3);
				rVectorComplex.clear();
				EXPECT_EQ(rVectorComplex.size(), 0);
			}
		}
		{ // test assignment to other IVector 
			{
				unique_ptr<IVector<int32_t>> pSrc((*vecs[i].first)());
				auto& rSrc = *pSrc;
				vector<int32_t> v { 1, 2, 3 };
				rSrc = v;
				rVectorSimple = rSrc; // assignment to IVector
				ASSERT_EQ(rVectorSimple.size(), 3);
				EXPECT_EQ(rVectorSimple[0], 1);
				EXPECT_EQ(rVectorSimple[1], 2);
				EXPECT_EQ(rVectorSimple[2], 3);
				rVectorSimple.clear();
				EXPECT_EQ(rVectorSimple.size(), 0);
			}
			{
				unique_ptr<IVector<IVector<int32_t>>> pSrc((*vecs[i].second)());
				auto& rSrc = *pSrc;
				vector<vector<int32_t>> v { { 1 }, { 1, 2}, { 1, 2, 3 }};
				rSrc = v;
				rVectorComplex = rSrc; // assignment to IVector
				ASSERT_EQ(rVectorComplex.size(), 3);
				EXPECT_EQ(rVectorComplex[0].size(), 1);
				EXPECT_EQ(rVectorComplex[1].size(), 2);
				EXPECT_EQ(rVectorComplex[2].size(), 3);
				rVectorComplex.clear();
				EXPECT_EQ(rVectorComplex.size(), 0);
			}
		}
		{ // test assignment to initializer_list
			{
				initializer_list<int32_t> il { 1, 2, 3 };
				rVectorSimple = il; // assignment to initializer_list
				ASSERT_EQ(rVectorSimple.size(), 3);
				EXPECT_EQ(rVectorSimple[0], 1);
				EXPECT_EQ(rVectorSimple[1], 2);
				EXPECT_EQ(rVectorSimple[2], 3);
				rVectorSimple.clear();
				EXPECT_EQ(rVectorSimple.size(), 0);
			}
			// note: nested initializer_list aren't supported!
		}
	}
}

TEST(IVector, assign) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*vecs[i].second)());
		auto& rVectorSimple  = *pVectorSimple;
		auto& rVectorComplex = *pVectorComplex;
		{ // test assign(count, value)
			{
				rVectorSimple.assign(std::size_t(3), int32_t(4711));
				ASSERT_EQ(rVectorSimple.size(), 3);
				EXPECT_EQ(rVectorSimple[0], 4711);
				EXPECT_EQ(rVectorSimple[1], 4711);
				EXPECT_EQ(rVectorSimple[2], 4711);
				rVectorSimple.clear();
				EXPECT_EQ(rVectorSimple.size(), 0);
			}
			{
				vector<int32_t> v { 1, 2, 3 };
				rVectorComplex.assign(std::size_t(3), v);
				ASSERT_EQ(rVectorComplex.size(), 3);
				EXPECT_EQ(rVectorComplex[0].size(), 3);
				EXPECT_EQ(rVectorComplex[1].size(), 3);
				EXPECT_EQ(rVectorComplex[2].size(), 3);
				rVectorComplex.clear();
				EXPECT_EQ(rVectorComplex.size(), 0);
			}
		}
		{ // test assign(iterator, iterator)
			{
				vector<int32_t> v { 1, 2, 3 };
				rVectorSimple.assign(v.begin(), v.end());
				ASSERT_EQ(rVectorSimple.size(), 3);
				EXPECT_EQ(rVectorSimple[0], 1);
				EXPECT_EQ(rVectorSimple[1], 2);
				EXPECT_EQ(rVectorSimple[2], 3);
				rVectorSimple.clear();
				EXPECT_EQ(rVectorSimple.size(), 0);
			}
			{
				vector<vector<int32_t>> v { { 1 }, { 1, 2 }, { 1, 2, 3 } };
				rVectorComplex.assign(v.begin(), v.end());
				ASSERT_EQ(rVectorComplex.size(), 3);
				EXPECT_EQ(rVectorComplex[0].size(), 1);
				EXPECT_EQ(rVectorComplex[1].size(), 2);
				EXPECT_EQ(rVectorComplex[2].size(), 3);
				rVectorComplex.clear();
				EXPECT_EQ(rVectorComplex.size(), 0);
			}
		}
		{ // test assign(initializer_list)
			{
				initializer_list<int32_t> il { 1, 2, 3 };
				rVectorSimple.assign(il);
				ASSERT_EQ(rVectorSimple.size(), 3);
				EXPECT_EQ(rVectorSimple[0], 1);
				EXPECT_EQ(rVectorSimple[1], 2);
				EXPECT_EQ(rVectorSimple[2], 3);
				rVectorSimple.clear();
				EXPECT_EQ(rVectorSimple.size(), 0);
			}
		}
	}
}

TEST(IVector, at) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*vecs[i].second)());
		auto& rVectorSimple  = *pVectorSimple;
		auto& rVectorComplex = *pVectorComplex;
		const auto& rcVectorSimple  = *pVectorSimple;
		const auto& rcVectorComplex = *pVectorComplex;
		{
			const vector<int32_t> vs { 1, 2, 3 };
			rVectorSimple = vs;
			ASSERT_EQ(rVectorSimple.size(), 3);
			EXPECT_EQ(rVectorSimple.at(0), 1);
			EXPECT_EQ(rVectorSimple.at(1), 2);
			EXPECT_EQ(rVectorSimple.at(2), 3);
			EXPECT_EQ(rcVectorSimple.at(0), 1);
			EXPECT_EQ(rcVectorSimple.at(1), 2);
			EXPECT_EQ(rcVectorSimple.at(2), 3);
			bool caught = false;
			try {
				(void)rVectorSimple.at(4);
			}
			catch(const out_of_range&) {
				caught = true;
			}
			catch(...) {
			}
			EXPECT_TRUE(caught);
			caught = false;
			try {
				(void)rcVectorSimple.at(4);
			}
			catch(const out_of_range&) {
				caught = true;
			}
			catch(...) {
			}
			EXPECT_TRUE(caught);
		}
		{
			const vector<vector<int32_t>> vc { { 1 }, { 1, 2 }, { 1, 2, 3 } };
			rVectorComplex = vc;
			ASSERT_EQ(rVectorComplex.size(), 3);
			EXPECT_EQ(rVectorComplex.at(0).size(), 1);
			EXPECT_EQ(rVectorComplex.at(1).size(), 2);
			EXPECT_EQ(rVectorComplex.at(2).size(), 3);
			EXPECT_EQ(rcVectorComplex.at(0).size(), 1);
			EXPECT_EQ(rcVectorComplex.at(1).size(), 2);
			EXPECT_EQ(rcVectorComplex.at(2).size(), 3);
			bool caught = false;
			try {
				(void)rVectorComplex.at(4);
			}
			catch(const out_of_range&) {
				caught = true;
			}
			catch(...) {
			}
			EXPECT_TRUE(caught);
			caught = false;
			try {
				(void)rcVectorComplex.at(4);
			}
			catch(const out_of_range&) {
				caught = true;
			}
			catch(...) {
			}
			EXPECT_TRUE(caught);
		}
	}
}

TEST(IVector, operatorBracket) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*vecs[i].second)());
		auto& rVectorSimple  = *pVectorSimple;
		auto& rVectorComplex = *pVectorComplex;
		const auto& rcVectorSimple  = *pVectorSimple;
		const auto& rcVectorComplex = *pVectorComplex;
		{
			const vector<int32_t> vs { 1, 2, 3 };
			rVectorSimple = vs;
			ASSERT_EQ(rVectorSimple.size(), 3);
			EXPECT_EQ(rVectorSimple[0], 1);
			EXPECT_EQ(rVectorSimple[1], 2);
			EXPECT_EQ(rVectorSimple[2], 3);
			EXPECT_EQ(rcVectorSimple[0], 1);
			EXPECT_EQ(rcVectorSimple[1], 2);
			EXPECT_EQ(rcVectorSimple[2], 3);
		}
		{
			const vector<vector<int32_t>> vc { { 1 }, { 1, 2 }, { 1, 2, 3 } };
			rVectorComplex = vc;
			ASSERT_EQ(rVectorComplex.size(), 3);
			EXPECT_EQ(rVectorComplex[0].size(), 1);
			EXPECT_EQ(rVectorComplex[1].size(), 2);
			EXPECT_EQ(rVectorComplex[2].size(), 3);
			EXPECT_EQ(rcVectorComplex[0].size(), 1);
			EXPECT_EQ(rcVectorComplex[1].size(), 2);
			EXPECT_EQ(rcVectorComplex[2].size(), 3);
		}
	}
}

TEST(IVector, front) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*vecs[i].second)());
		auto& rVectorSimple  = *pVectorSimple;
		auto& rVectorComplex = *pVectorComplex;
		const auto& rcVectorSimple  = *pVectorSimple;
		const auto& rcVectorComplex = *pVectorComplex;
		{
			const vector<int32_t> vs { 1, 2, 3 };
			rVectorSimple = vs;
			ASSERT_EQ(rVectorSimple.size(), 3);
			EXPECT_EQ(rVectorSimple.front(), 1);
			EXPECT_EQ(rcVectorSimple.front(), 1);
		}
		{
			const vector<vector<int32_t>> vc { { 1 }, { 1, 2 }, { 1, 2, 3 } };
			rVectorComplex = vc;
			ASSERT_EQ(rVectorComplex.size(), 3);
			EXPECT_EQ(rVectorComplex.front().size(), 1);
			EXPECT_EQ(rcVectorComplex.front().size(), 1);
		}
	}
}

TEST(IVector, back) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*vecs[i].second)());
		auto& rVectorSimple  = *pVectorSimple;
		auto& rVectorComplex = *pVectorComplex;
		const auto& rcVectorSimple  = *pVectorSimple;
		const auto& rcVectorComplex = *pVectorComplex;
		{
			const vector<int32_t> vs { 1, 2, 3 };
			rVectorSimple = vs;
			ASSERT_EQ(rVectorSimple.size(), 3);
			EXPECT_EQ(rVectorSimple.back(), 3);
			EXPECT_EQ(rcVectorSimple.back(), 3);
		}
		{
			const vector<vector<int32_t>> vc { { 1 }, { 1, 2 }, { 1, 2, 3 } };
			rVectorComplex = vc;
			ASSERT_EQ(rVectorComplex.size(), 3);
			EXPECT_EQ(rVectorComplex.back().size(), 3);
			EXPECT_EQ(rcVectorComplex.back().size(), 3);
		}
	}
}

TEST(IVector, data) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		auto& rVectorSimple  = *pVectorSimple;
		const auto& rcVectorSimple  = *pVectorSimple;
		{
			const vector<int32_t> vs { 1, 2, 3 };
			rVectorSimple = vs;
			ASSERT_EQ(*rVectorSimple.data(), 1);
			EXPECT_EQ(*rcVectorSimple.data(), 1);
		}
	}
}

TEST(IVector, iterateNonConst) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*vecs[i].second)());
		auto& rVectorSimple  = *pVectorSimple;
		auto& rVectorComplex = *pVectorComplex;
		{
			const vector<int32_t> vs { 1, 2, 3 };
			rVectorSimple = vs;
			const auto end = rVectorSimple.end();
			int32_t j = 1;
			for (auto iter = rVectorSimple.begin(); iter != end; ++iter, ++j) {
				EXPECT_EQ(*iter, j);
			}
			EXPECT_EQ(j, 4);
			*rVectorSimple.begin() = 4;
			EXPECT_EQ(*rVectorSimple.begin(), 4);
		}
		{
			const vector<vector<int32_t>> vc { { 1 }, { 1, 2 }, { 1, 2, 3 } };
			rVectorComplex = vc;
			const auto end = rVectorComplex.end();
			int32_t j = 1;
			for (auto iter = rVectorComplex.begin(); iter != end; ++iter, ++j) {
				EXPECT_EQ(iter->size(), j);
			}
			EXPECT_EQ(j, 4);
			vector<int32_t> x { 4, 5, 6 };
			rVectorComplex.begin()->clear();
			EXPECT_EQ(rVectorComplex.begin()->size(), 0);
		}
	}
}

TEST(IVector, iterateConst) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*vecs[i].second)());
		auto& rVectorSimple  = *pVectorSimple;
		auto& rVectorComplex = *pVectorComplex;
		const auto& rcVectorSimple  = *pVectorSimple;
		const auto& rcVectorComplex = *pVectorComplex;
		{
			const vector<int32_t> vs { 1, 2, 3 };
			rVectorSimple = vs;
			const auto end = rcVectorSimple.end();
			int32_t j = 1;
			for (auto iter = rcVectorSimple.begin(); iter != end; ++iter, ++j) {
				EXPECT_EQ(*iter, j);
			}
			EXPECT_EQ(j, 4);
			*rVectorSimple.begin() = 4;
			EXPECT_EQ(*rcVectorSimple.begin(), 4);
		}
		{
			const vector<vector<int32_t>> vc { { 1 }, { 1, 2 }, { 1, 2, 3 } };
			rVectorComplex = vc;
			const auto end = rcVectorComplex.end();
			int32_t j = 1;
			for (auto iter = rcVectorComplex.begin(); iter != end; ++iter, ++j) {
				EXPECT_EQ(iter->size(), j);
			}
			EXPECT_EQ(j, 4);
			vector<int32_t> x { 4, 5, 6 };
			rVectorComplex.begin()->clear();
			EXPECT_EQ(rcVectorComplex.begin()->size(), 0);
		}
	}
}

TEST(IVector, iterateWithInterfaceUpcast) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		auto& rVectorSimple  = *pVectorSimple;
		{
			const vector<int32_t> vs { 1, 2, 3 };
			rVectorSimple = vs;
			int32_t j = 1;
			auto iter = rVectorSimple.begin();
			auto end = rVectorSimple.end();
			IVectorIterator<int32_t>& rIter = iter;
			IVectorIterator<int32_t>& rEnd = end;
			for (;rIter != rEnd; ++rIter, ++j) {
				EXPECT_EQ(*rIter, j);
			}
			EXPECT_EQ(j, 4);
			*rVectorSimple.begin() = 4;
			EXPECT_EQ(*rVectorSimple.begin(), 4);
		}
	}
}

TEST(IVector, iterateRangeBased) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*vecs[i].second)());
		auto& rVectorSimple  = *pVectorSimple;
		const auto& rcVectorSimple  = *pVectorSimple;
		auto& rVectorComplex = *pVectorComplex;
		const auto& rcVectorComplex = *pVectorComplex;
		{
			const vector<int32_t> vs { 1, 2, 3 };
			rVectorSimple = vs;
			int32_t j = 1;
			for (auto& element: rVectorSimple) {
				EXPECT_EQ(element, j);
				++j;
			}
			EXPECT_EQ(j, 4);
			j = 1;
			for (const auto& element: rcVectorSimple) {
				EXPECT_EQ(element, j);
				++j;
			}
			EXPECT_EQ(j, 4);
		}
		{
			const vector<vector<int32_t>> vc { { 1 }, { 1, 2 }, { 1, 2, 3 } };
			rVectorComplex = vc;
			int32_t j = 1;
			for (lia_ELEM_REF(rVectorComplex) element: rVectorComplex) {
				EXPECT_EQ(element.size(), j);
				++j;
			}
			EXPECT_EQ(j, 4);
			j = 1;
			for (lia_ELEM_CONST_REF(rcVectorComplex) element: rcVectorComplex) {
				EXPECT_EQ(element.size(), j);
				++j;
			}
			EXPECT_EQ(j, 4);
		}
	}
}

TEST(IVector, iterateReverse) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*vecs[i].second)());
		auto& rVectorSimple  = *pVectorSimple;
		auto& rVectorComplex = *pVectorComplex;
		{
			const vector<int32_t> vs { 1, 2, 3 };
			rVectorSimple = vs;
			const auto end = rVectorSimple.rend();
			int32_t j = 3;
			for (auto iter = rVectorSimple.rbegin(); iter != end; ++iter, --j) {
				EXPECT_EQ(*iter, j);
			}
			EXPECT_EQ(j, 0);
		}
		{
			const vector<vector<int32_t>> vc { { 1 }, { 1, 2 }, { 1, 2, 3 } };
			rVectorComplex = vc;
			const auto end = rVectorComplex.rend();
			int32_t j = 3;
			for (auto iter = rVectorComplex.rbegin(); iter != end; ++iter, --j) {
				EXPECT_EQ(iter->size(), j);
			}
			EXPECT_EQ(j, 0);
		}
	}
}

TEST(IVector, empty) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*vecs[i].second)());
		auto& rVectorSimple  = *pVectorSimple;
		auto& rVectorComplex = *pVectorComplex;
		const auto& rcVectorSimple  = *pVectorSimple;
		const auto& rcVectorComplex = *pVectorComplex;
		{
			EXPECT_TRUE(rcVectorSimple.empty());
			const vector<int32_t> vs { 1, 2, 3 };
			rVectorSimple = vs;
			EXPECT_FALSE(rcVectorSimple.empty());
		}
		{
			EXPECT_TRUE(rcVectorComplex.empty());
			const vector<vector<int32_t>> vc { { 1 }, { 1, 2 }, { 1, 2, 3 } };
			rVectorComplex = vc;
			EXPECT_FALSE(rcVectorComplex.empty());
		}
	}
}

TEST(IVector, size) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*vecs[i].second)());
		auto& rVectorSimple  = *pVectorSimple;
		auto& rVectorComplex = *pVectorComplex;
		const auto& rcVectorSimple  = *pVectorSimple;
		const auto& rcVectorComplex = *pVectorComplex;
		{
			EXPECT_EQ(rcVectorSimple.size(), 0);
			const vector<int32_t> vs { 1, 2, 3 };
			rVectorSimple = vs;
			EXPECT_EQ(rcVectorSimple.size(), 3);
		}
		{
			EXPECT_EQ(rcVectorComplex.size(), 0);
			const vector<vector<int32_t>> vc { { 1 }, { 1, 2 }, { 1, 2, 3 } };
			rVectorComplex = vc;
			EXPECT_EQ(rcVectorComplex.size(), 3);
		}
	}
}

TEST(IVector, capacity) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*vecs[i].second)());
		auto& rVectorSimple  = *pVectorSimple;
		auto& rVectorComplex = *pVectorComplex;
		const auto& rcVectorSimple  = *pVectorSimple;
		const auto& rcVectorComplex = *pVectorComplex;
		{
			const vector<int32_t> vs { 1, 2, 3 };
			rVectorSimple = vs;
			EXPECT_GE(rcVectorSimple.capacity(), 3);
		}
		{
			const vector<vector<int32_t>> vc { { 1 }, { 1, 2 }, { 1, 2, 3 } };
			rVectorComplex = vc;
			EXPECT_GE(rcVectorComplex.capacity(), 3);
		}
	}
}

TEST(IVector, clear) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*vecs[i].second)());
		auto& rVectorSimple  = *pVectorSimple;
		auto& rVectorComplex = *pVectorComplex;
		const auto& rcVectorSimple  = *pVectorSimple;
		const auto& rcVectorComplex = *pVectorComplex;
		{
			const vector<int32_t> vs { 1, 2, 3 };
			rVectorSimple = vs;
			EXPECT_FALSE(rcVectorSimple.empty());
			rVectorSimple.clear();
			EXPECT_TRUE(rcVectorSimple.empty());
		}
		{
			const vector<vector<int32_t>> vc { { 1 }, { 1, 2 }, { 1, 2, 3 } };
			rVectorComplex = vc;
			EXPECT_FALSE(rcVectorComplex.empty());
			rVectorComplex.clear();
			EXPECT_TRUE(rcVectorComplex.empty());
		}
	}
}

TEST(IVector, insert) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		auto& rVectorSimple  = *pVectorSimple;
		const auto& rcVectorSimple  = *pVectorSimple;
		{
			auto iter = rVectorSimple.insert(rVectorSimple.begin(), 1);
			EXPECT_EQ(iter, rVectorSimple.begin());
			iter = rVectorSimple.insert(rVectorSimple.cbegin(), 2);
			EXPECT_EQ(iter, rVectorSimple.begin());
			iter = rVectorSimple.insert(rVectorSimple.begin(),  3);
			EXPECT_EQ(iter, rVectorSimple.begin());
			ASSERT_EQ(rcVectorSimple.size(), 3);
			EXPECT_EQ(rcVectorSimple[0], 3);
			EXPECT_EQ(rcVectorSimple[1], 2);
			EXPECT_EQ(rcVectorSimple[2], 1);
			rVectorSimple.clear();
		}
		{
			auto iter = rVectorSimple.insert(rVectorSimple.begin(),  std::size_t(3), 1);
			EXPECT_EQ(iter, rVectorSimple.begin());
			iter = rVectorSimple.insert(rVectorSimple.cbegin(), std::size_t(2), 2);
			EXPECT_EQ(iter, rVectorSimple.begin());
			ASSERT_EQ(rcVectorSimple.size(), 5);
			EXPECT_EQ(rcVectorSimple[0], 2);
			EXPECT_EQ(rcVectorSimple[1], 2);
			EXPECT_EQ(rcVectorSimple[2], 1);
			EXPECT_EQ(rcVectorSimple[3], 1);
			EXPECT_EQ(rcVectorSimple[4], 1);
			rVectorSimple.clear();
		}
		{
			vector<int32_t> x { 1, 2, 3 };
			auto iter = rVectorSimple.insert(rVectorSimple.begin(), x.begin(), x.end());
			EXPECT_EQ(iter, rVectorSimple.begin());
			iter = rVectorSimple.insert(rVectorSimple.cbegin(), x.begin(), x.end());
			EXPECT_EQ(iter, rVectorSimple.begin());
			ASSERT_EQ(rcVectorSimple.size(), 6);
			EXPECT_EQ(rcVectorSimple[0], 1);
			EXPECT_EQ(rcVectorSimple[1], 2);
			EXPECT_EQ(rcVectorSimple[2], 3);
			EXPECT_EQ(rcVectorSimple[3], 1);
			EXPECT_EQ(rcVectorSimple[4], 2);
			EXPECT_EQ(rcVectorSimple[5], 3);
			rVectorSimple.clear();
		}
		{
			initializer_list<int32_t> x { 1, 2, 3 };
			auto iter = rVectorSimple.insert(rVectorSimple.cbegin(), x);
			EXPECT_EQ(iter, rVectorSimple.begin());
			ASSERT_EQ(rcVectorSimple.size(), 3);
			EXPECT_EQ(rcVectorSimple[0], 1);
			EXPECT_EQ(rcVectorSimple[1], 2);
			EXPECT_EQ(rcVectorSimple[2], 3);
			rVectorSimple.clear();
		}
	}
}

TEST(IVector, emplace) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		auto& rVectorSimple  = *pVectorSimple;
		const auto& rcVectorSimple  = *pVectorSimple;
		{
			auto iter = rVectorSimple.emplace(rVectorSimple.cbegin(), 1);
			EXPECT_EQ(iter, rVectorSimple.begin());
			ASSERT_EQ(rcVectorSimple.size(), 1);
			EXPECT_EQ(rcVectorSimple[0], 1);
		}
	}
}

TEST(IVector, erase) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		auto& rVectorSimple  = *pVectorSimple;
		const auto& rcVectorSimple  = *pVectorSimple;
		{
			const vector<int32_t> vs { 1, 2, 3 };
			rVectorSimple = vs;
			auto iter = rVectorSimple.erase(rcVectorSimple.begin()+1);
			EXPECT_EQ(*iter, 3);
			rVectorSimple.clear();
			rVectorSimple = vs;
			iter = rVectorSimple.erase(rVectorSimple.cbegin(), rVectorSimple.cend());
			EXPECT_TRUE(rcVectorSimple.empty());
		}
	}
}

TEST(IVector, conversionOfIteratorIntoConstIterator) {
	const auto vecs = makeVectors();
	EXPECT_GT(vecs.size(), 0);
	for (size_t i=0; i<vecs.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*vecs[i].first)());
		auto& rVectorSimple  = *pVectorSimple;
		const auto& rcVectorSimple  = *pVectorSimple;
		{
			const vector<int32_t> vs { 1, 2, 3 };
			rVectorSimple = vs;
			VectorIteratorHandle<int32_t> iter = pVectorSimple->begin();
			VectorIteratorHandle<const int32_t> constIter = iter;
		}
	}
}
