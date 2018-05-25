// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#pragma once

// MSVC doesn't use __cplusplus well.
#if !defined(_MSC_VER) && (__cplusplus < 201103L)
#error "This library needs at least a C++11 compliant compiler."
#endif

#ifdef _MSC_VER  // msvc
/*
 * https://msdn.microsoft.com/en-us/library/b0084kay.aspx
 * https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B
 * https://docs.microsoft.com/en-us/cpp/visual-cpp-language-conformance
 */

#ifndef __cpp_constexpr
#define JASL_cpp_constexpr                           \
  (_MSC_VER >= 1911L && _MSVC_LANG >= 201703L        \
       ? 201603L                                     \
       : (_MSC_VER >= 1910L && _MSVC_LANG >= 201402L \
              ? 201304L                              \
              : (_MSC_VER >= 1900L ? 200704L : 0L)))
#else
static_assert(false, "Compiler behaviour has been changed. Review the change!");
#endif  // __cpp_constexpr
#ifndef __cpp_lib_string_view
#define JASL_cpp_lib_string_view                                       \
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
#define JASL_cpp_constexpr \
  (__GNUC__ >= 7L          \
       ? 201603L           \
       : (__GNUC__ >= 5L   \
              ? 201304L    \
              : (__GNUC__ == 4L && __GNUC_MINOR__ >= 6L ? 200704L : 0L)))
#else
static_assert(false, "Compiler behaviour has been changed. Review the change!");
#endif  // __cpp_constexpr
#ifndef __cpp_lib_string_view
#define JASL_cpp_lib_string_view                                       \
  (__cplusplus >= 201703 &&                                            \
           (__GNUC__ > 7L || (__GNUC__ == 7L && __GNUC_MINOR__ >= 1L)) \
       ? 201606L                                                       \
       : 0L)
#else
static_assert(false, "Compiler behaviour has been changed. Review the change!");
#endif  // __cpp_lib_string_view

#elif defined(__clang__)  // clang

#ifdef __cpp_constexpr
#define JASL_cpp_constexpr __cpp_constexpr
#else
static_assert(false, "Probably unsupported compiler version!");
#endif
#ifndef __cpp_lib_string_view
#define JASL_cpp_lib_string_view (__has_include(<string_view>) ? 201606L : 0L)
#else
static_assert(false, "Compiler behaviour has been changed. Review the change!");
#endif

#else

static_assert(false, "Unsupported compiler!");

#endif  // <compiler types>

#if !JASL_cpp_constexpr
#undef JASL_cpp_constexpr
#endif  // !JASL_cpp_constexpr

#if !JASL_cpp_lib_string_view
#undef JASL_cpp_lib_string_view
#endif  // !JASL_cpp_lib_string_view
