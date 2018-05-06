// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#pragma once

#include "jasl_diagnostic.hpp"

#ifdef _MSC_VER  // msvc

/*
 * https://msdn.microsoft.com/en-us/library/b0084kay.aspx
 * https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B
 * https://docs.microsoft.com/en-us/cpp/visual-cpp-language-conformance
 */

#ifndef __cpp_constexpr
#define __cpp_constexpr                              \
  (_MSC_VER >= 1911L && _MSVC_LANG >= 201703L        \
       ? 201603L                                     \
       : (_MSC_VER >= 1910L && _MSVC_LANG >= 201402L \
              ? 201304L                              \
              : (_MSC_VER >= 1900L ? 200704L : 0L)))
#else
static_assert(false, "Compiler behaviour has been changed. Review the change!");
#endif  // __cpp_constexpr
#ifndef __cpp_lib_string_view
#define __cpp_lib_string_view                                          \
  (__cplusplus >= 201703 && _MSC_VER >= 1910L && _MSVC_LANG >= 201703L \
       ? 201606L                                                       \
       : 0L)
#else
static_assert(false, "Compiler behaviour has been changed. Review the change!");
#endif  // __cpp_lib_string_view

#elif defined(__GNUG__) && !defined(__clang__)  // gcc
/*
 * https://gcc.gnu.org/projects/cxx-status.html
 * https://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html
 * http://nadeausoftware.com/articles/2012/10/c_c_tip_how_detect_compiler_name_and_version_using_compiler_predefined_macros
 */

#ifndef __cpp_constexpr
#define __cpp_constexpr  \
  (__GNUC__ >= 7L        \
       ? 201603L         \
       : (__GNUC__ >= 5L \
              ? 201304L  \
              : (__GNUC__ == 4L && __GNUC_MINOR__ >= 6L ? 200704L : 0L)))
#else
static_assert(false, "Compiler behaviour has been changed. Review the change!");
#endif  // __cpp_constexpr
#ifndef __cpp_lib_string_view
#define __cpp_lib_string_view                                          \
  (__cplusplus >= 201703 &&                                            \
           (__GNUC__ > 7L || (__GNUC__ == 7L && __GNUC_MINOR__ >= 1L)) \
       ? 201606L                                                       \
       : 0L)
#else
static_assert(false, "Compiler behaviour has been changed. Review the change!");
#endif  // __cpp_lib_string_view

#elif defined(__clang__)  // clang

#ifndef __cpp_constexpr
static_assert(false, "Unsupported compiler version!");
#endif
#ifndef __cpp_lib_string_view
JASL_DIAGNOSTIC_PUSH()
JASL_DIAGNOSTIC_DISABLE_WARNING_RESERVER_MACRO()
#define __cpp_lib_string_view \
  (__cplusplus >= 201703 && __clang_major__ >= 6 ? 201606L : 0L)
JASL_DIAGNOSTIC_POP()
#else
static_assert(false, "Compiler behaviour has been changed. Review the change!");
#endif

#else

static_assert(false, "Unsupported compiler!");

#endif  // <compiler types>

JASL_DIAGNOSTIC_PUSH()
JASL_DIAGNOSTIC_DISABLE_WARNING_RESERVER_MACRO()
#if !__cpp_constexpr
#undef __cpp_constexpr
#endif  // !__cpp_constexpr

#if !__cpp_lib_string_view
#undef __cpp_lib_string_view
#endif  // !__cpp_lib_string_view
JASL_DIAGNOSTIC_POP()
