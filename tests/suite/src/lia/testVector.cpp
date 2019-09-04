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
#include <gtest/gtest.h>
#include <lia/DllLoader.h>
#include <lia/IVector.h>

using namespace lia::dll_loader;
using namespace lia;
using namespace std;

namespace {

typedef IVector<int32_t>* (lia_CALL *CreateInt32VectorFunction)(void);
typedef IVector<IVector<int32_t>>* (lia_CALL *CreateInt32VectorVectorFunction)(void);

}

TEST(IVector, assignmentOperator) {
	const auto creatorsSimple  = getDllFunctions<CreateInt32VectorFunction>("createInt32Vector");
	const auto creatorsComplex = getDllFunctions<CreateInt32VectorVectorFunction>("createInt32VectorVector");
	EXPECT_GT(creatorsSimple.size(), 0);
	ASSERT_EQ(creatorsSimple.size(), creatorsComplex.size());
	for (size_t i=0; i<creatorsSimple.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*creatorsSimple[i])());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*creatorsComplex[i])());
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
				unique_ptr<IVector<int32_t>> pSrc((*creatorsSimple[i])());
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
				unique_ptr<IVector<IVector<int32_t>>> pSrc((*creatorsComplex[i])());
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
	const auto creatorsSimple  = getDllFunctions<CreateInt32VectorFunction>("createInt32Vector");
	const auto creatorsComplex = getDllFunctions<CreateInt32VectorVectorFunction>("createInt32VectorVector");
	EXPECT_GT(creatorsSimple.size(), 0);
	ASSERT_EQ(creatorsSimple.size(), creatorsComplex.size());
	for (size_t i=0; i<creatorsSimple.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*creatorsSimple[i])());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*creatorsComplex[i])());
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
	const auto creatorsSimple  = getDllFunctions<CreateInt32VectorFunction>("createInt32Vector");
	const auto creatorsComplex = getDllFunctions<CreateInt32VectorVectorFunction>("createInt32VectorVector");
	EXPECT_GT(creatorsSimple.size(), 0);
	ASSERT_EQ(creatorsSimple.size(), creatorsComplex.size());
	for (size_t i=0; i<creatorsSimple.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*creatorsSimple[i])());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*creatorsComplex[i])());
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
	const auto creatorsSimple  = getDllFunctions<CreateInt32VectorFunction>("createInt32Vector");
	const auto creatorsComplex = getDllFunctions<CreateInt32VectorVectorFunction>("createInt32VectorVector");
	EXPECT_GT(creatorsSimple.size(), 0);
	ASSERT_EQ(creatorsSimple.size(), creatorsComplex.size());
	for (size_t i=0; i<creatorsSimple.size(); ++i) {
		unique_ptr<IVector<int32_t>> pVectorSimple((*creatorsSimple[i])());
		unique_ptr<IVector<IVector<int32_t>>> pVectorComplex((*creatorsComplex[i])());
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
