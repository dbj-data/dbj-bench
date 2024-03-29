/// https://godbolt.org/z/TE74oK
/// https://github.com/attractivechaos/benchmarks

#include "../../ubut/ubench.h"
#include "../../dbj-fwk/printing_macros.h"

#include <stdexcept>
#include <minwinbase.h>

///-----------------------------------------------
struct buffer_type { char data[0xFF]; };
using test_array_type = buffer_type;
static const test_array_type test_array_element = { {'?'} };

///-----------------------------------------------
// on bad index ATL does ATL_ASSERT on debug build
// which provokes UCRT dialogue offering to debug
// so lets do only release builds
#ifndef _DEBUG
#if __has_include(<atlsimpcoll.h>)

// simply the opposite
#define _ATL_NO_EXCEPTIONS (0 == _CPPUNWIND)
#include <atlcoll.h>

#if (_CPPUNWIND == 0) && ( _ATL_NO_EXCEPTIONS == 0 )
#pragma message("_CPPUNWIND == 0 and _ATL_NO_EXCEPTIONS is not defined")
#endif // _CPPUNWIND == 0

static void atl_array()
{
	using atl_arr = ATL::CAtlArray<test_array_type>;
	// _ATL_NO_EXCEPTIONS ?
#if _CPPUNWIND
	try {
#endif // _CPPUNWIND
		atl_arr array_;
		// add 3 elements
		array_.Add(test_array_element);
		array_.Add(test_array_element);
		array_.Add(test_array_element);
		// use illegal index
		(void)array_.operator[](9);
#if _CPPUNWIND
	}
	catch (...) {
	}
#endif // _CPPUNWIND

}

UBENCH(bad_index_vector, atl_arr)
{
	static bool done_that = false;
	// SEH is always available but not mixed
	// in the same function
	// with C++ exceptions
	__try {
		atl_array();
	}
	__except (
		//GetExceptionCode() == EXCEPTION_ILLEGAL_INSTRUCTION
		///* unlike simple arr which raises EXCEPTION_ARRAY_BOUNDS_EXCEEDED */
		//? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_EXECUTION
		EXCEPTION_EXECUTE_HANDLER
		)
	{
		// RELEASE or DEBUG build
		// on bad index ATL raises SE with id: EXCEPTION_ARRAY_BOUNDS_EXCEEDED
		// which is defined in minwinbase.h
		// ATL never throws C++ exceptions
		if (!done_that) {
			DBJ_WARN("ATL index out of range caught by SEH, on each call!");
			done_that = true;
		}

	}
}

#include <atlsimpcoll.h>

static void atl_simple_array() {

	using atl_vec = ATL::CSimpleArray<test_array_type>;

#if _CPPUNWIND
	try {
#endif // _CPPUNWIND
		atl_vec array_;
		// add 3 elements
		array_.Add(test_array_element);
		array_.Add(test_array_element);
		array_.Add(test_array_element);
		// use illegal index
		(void)array_.operator[](9);

#if _CPPUNWIND
	}
	catch (...) {
	}
#endif // _CPPUNWIND

}

UBENCH(bad_index_vector, atl_simple_arr)
{
	static bool done_that = false;
	// SEH is always available but not mixed
	// in the same function
	// with C++ exceptions
	__try {
		atl_simple_array();
	}
	__except (
		GetExceptionCode() == EXCEPTION_ARRAY_BOUNDS_EXCEEDED
		? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_EXECUTION)
	{
		// RELEASE or DEBUG build
		// on bad index ATL raises SE with id: EXCEPTION_ARRAY_BOUNDS_EXCEEDED
		// which is defined in minwinbase.h
		// ATL never throws C++ exceptions
		if (!done_that) {
			DBJ_WARN("ATL index out of range caught by SEH, on each call!");
			done_that = true;
		}

	}
}
#endif // __has_include(<atlsimpcoll.h>)
#endif // ! _DEBUG

///-----------------------------------------------
#include <vector>

// SEH and C++ exceptions can not be 
//mixed in one function
static void mst_stl_bad_index_vector() {
#if _CPPUNWIND
	static bool done_that = false;
	try {
#endif
		std::vector<test_array_type> array_{};
		array_.push_back(test_array_element);
		array_.push_back(test_array_element);
		array_.push_back(test_array_element);
		// use illegal index
		(void)array_.at(9);
#if _CPPUNWIND
	}
	catch (std::out_of_range& x) {
		if (!done_that) {
			DBJ_WARN("MS STL C++ exception: '%s' ,caught on each call!", x.what());
			done_that = true;
		}
	}
#endif
}
// SEH is always there
UBENCH(bad_index_vector, ms_stl_vec_)
{
	static bool done_that = false;
	__try {
		mst_stl_bad_index_vector();
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		if (!done_that) {
			DBJ_WARN("MS STL index out of range caught by SEH, on each call!");
			done_that = true;
		}
	}

}

// #endif // _DEBUG


