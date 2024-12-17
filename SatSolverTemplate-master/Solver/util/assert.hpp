/**
* @author Tim Luchterhand
* @date 29.11.24
* @file assert.hpp
* @brief Wrapper around the classic C assert that always evaluates an expression but only checks the result in debug
* mode
*/

#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <cassert>

#ifdef NDEBUG
#define ASSERT_RESULT(expr) expr;
#else
#define ASSERT_RESULT(expr) assert(expr)
#endif


#endif //ASSERT_HPP
