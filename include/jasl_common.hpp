// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#pragma once

/*
 * Same as the first item of the CHANGELOG.md file
 */
#define JASL_VERSION_MAJOR 0
#define JASL_VERSION_MINOR 1
#define JASL_VERSION_PATCH 0
#define JASL_VERSION_DATE "2018-04-12"

static_assert(JASL_VERSION_MAJOR < 100, "JASL_VERSION error");
static_assert(JASL_VERSION_MINOR < 100, "JASL_VERSION error");
static_assert(JASL_VERSION_PATCH < 1000, "JASL_VERSION error");

// for example version 1.2.3 is: 10020003
#define JASL_VERSION                                \
  (JASL_VERSION_PATCH + JASL_VERSION_MINOR * 1000 + \
   JASL_VERSION_MAJOR * 1000 * 100)

/*
 * https://msdn.microsoft.com/en-us/library/b0084kay.aspx
 * https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B
 * https://docs.microsoft.com/en-us/cpp/visual-cpp-language-conformance
 */
#ifdef _MSC_VER
#ifndef __cpp_constexpr
#define __cpp_constexpr                              \
  (_MSC_VER >= 1911L && _MSVC_LANG >= 201703L        \
       ? 201603L                                     \
       : (_MSC_VER >= 1910L && _MSVC_LANG >= 201402L \
              ? 201304L                              \
              : (_MSC_VER >= 1900L ? 200704L : 0L)))
#if !__cpp_constexpr
#undef __cpp_constexpr
#endif  // !__cpp_constexpr
#endif  // __cpp_constexpr
#ifndef __cpp_lib_string_view
#define __cpp_lib_string_view \
  (_MSC_VER >= 1910L && _MSVC_LANG >= 201703L ? 201606L : 0L)
#if !__cpp_lib_string_view
#undef __cpp_lib_string_view
#endif  // !__cpp_lib_string_view
#endif  // __cpp_lib_string_view
#endif  // _MSC_VER

/*
 * https://gcc.gnu.org/projects/cxx-status.html
 * https://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html
 * http://nadeausoftware.com/articles/2012/10/c_c_tip_how_detect_compiler_name_and_version_using_compiler_predefined_macros
 */
#if defined(__GNUG__) && !defined(__clang__)
#ifndef __cpp_constexpr
#define __cpp_constexpr  \
  (__GNUC__ >= 7L        \
       ? 201603L         \
       : (__GNUC__ >= 5L \
              ? 201304L  \
              : (__GNUC__ == 4L && __GNUC_MINOR__ >= 6L ? 200704L : 0L)))
#if !__cpp_constexpr
#undef __cpp_constexpr
#endif  // !__cpp_constexpr
#endif  // __cpp_constexpr
#ifndef __cpp_lib_string_view
#define __cpp_lib_string_view \
  (__GNUC__ > 7L || (__GNUC__ == 7L && __GNUC_MINOR__ >= 1L) ? 201606L : 0L)
#if !__cpp_lib_string_view
#undef __cpp_lib_string_view
#endif  // !__cpp_lib_string_view
#endif  // __cpp_lib_string_view
#endif  // defined(__GNUG__) && !defined(__clang__)

/*
 *http://en.cppreference.com/w/User:D41D8CD98F/feature_testing_macros
 */

// http://en.cppreference.com/w/cpp/language/constexpr
#ifdef __cpp_constexpr
#if __cpp_constexpr >= 201304L
#define JASL_CONSTEXPR_FROM_14 constexpr
#define JASL_CONSTEXPR_FROM_11 constexpr
#elif __cpp_constexpr >= 200704L
#define JASL_CONSTEXPR_FROM_14
#define JASL_CONSTEXPR_FROM_11 constexpr
#else
#error "Something wrong with defines: __cpp_constexpr should not be defined."
#endif
#else  // ifdef __cpp_constexpr
// There is no reason to support it without supporting noexcept.
#error "Unsupported C++ standard!"
#endif

#if !defined(JASL_USE_JASL_STRING_VIEW_TYPE_AS_BASE) && \
    !defined(__cpp_lib_string_view)
#define JASL_USE_JASL_STRING_VIEW_TYPE_AS_BASE
#endif

#ifdef JASL_TERMINATE_ON_EXCEPTION_ON
#include <exception>
#define JASL_THROW(exception) ::std::terminate()
#else
#define JASL_THROW(exception) throw exception
#endif

#if defined(JASL_ASSERT_ON) && __cpp_constexpr >= 201304L
#include <cstdlib>
#include <iostream>
#define JASL_ASSERT(expr, message)                                    \
  do {                                                                \
    if (!(expr)) {                                                    \
      ::std::cerr << __FILE__ << '(' << __LINE__ << "): " << message; \
      ::std::terminate();                                             \
    }                                                                 \
  } while (false)
#else
#define JASL_ASSERT(...)
#endif
