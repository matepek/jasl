// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

/** @file */

#pragma once

#include "jasl_feature_test_macro.hpp"

#define JASL_INNER_STR(x) #x
#define JASL_STR(x) JASL_INNER_STR(x)

/*
 * Same as the first item of the CHANGELOG.md file
 */
#define JASL_VERSION_MAJOR 1
#define JASL_VERSION_MINOR 0
#define JASL_VERSION_PATCH 1
#define JASL_VERSION_LABEL "-beta"
#define JASL_VERSION_DATE "2018-08-20"

static_assert(JASL_VERSION_MAJOR < 100, "JASL_VERSION error");
static_assert(JASL_VERSION_MINOR < 100, "JASL_VERSION error");
static_assert(JASL_VERSION_PATCH < 1000, "JASL_VERSION error");

#define JASL_VERSION_STR                                            \
  JASL_STR(JASL_VERSION_MAJOR)                                      \
  "." JASL_STR(JASL_VERSION_MINOR) "." JASL_STR(JASL_VERSION_PATCH) \
      JASL_VERSION_LABEL " (" JASL_VERSION_DATE ")"

#ifdef JASL_TERMINATE_ON_EXCEPTION_ON
#include <exception>
#define JASL_THROW(exception) ::std::terminate()
#else
#define JASL_THROW(exception) throw(exception)
#endif

#if defined(JASL_ASSERT_ON) && JASL_cpp_constexpr >= 201304L
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

#if defined(JASL_TESTIFY_STD_STRING_VIEW_EXISTS)
#if !defined(JASL_cpp_lib_string_view)
#define JASL_cpp_lib_string_view 201606L
#include <string_view>
#endif
#endif

/*
 * This part is only for documentation puposes.
 */
#ifdef JASL_INNER_DOXYGEN_DOCUMENTATION
#error "Should not be defined!"

/**
 * using _jasl::string_view_ = _jasl::nonstd::string_view_;
 *
 * The result of this that jasl::static_string and jasl::string will use it its
 * funtionality. _jasl::nonstd::string_view_ will be used even if
 * _std::string_view_ is possible to use (C++17).
 *
 * __Remarks:__
 *  If none of _JASL_USE_JASL_STRING_VIEW_AS_BASE_ and
 * _JASL_USE_STD_STRING_VIEW_AS_BASE_ are defined: If you have no
 * [std::string_view](http://en.cppreference.com/w/cpp/string/basic_string_view),
 * _JASL_USE_JASL_STRING_VIEW_AS_BASE_ will be defined automatically.
 */
#define JASL_USE_JASL_STRING_VIEW_AS_BASE

/**
 * using _jasl::string_view_ =
 * [std::string_view](http://en.cppreference.com/w/cpp/string/basic_string_view);
 *
 * The result of this that jasl::static_string and jasl::string will use it its
 * funtionality If it is not possible, the source won't fallback to
 * _jasl::nonstd::string_view_.
 *
 * __Remarks:__
 *  If none of _JASL_USE_JASL_STRING_VIEW_AS_BASE_ and
 * _JASL_USE_STD_STRING_VIEW_AS_BASE_ are defined: If you have no
 * [std::string_view](http://en.cppreference.com/w/cpp/string/basic_string_view),
 * _JASL_USE_JASL_STRING_VIEW_AS_BASE_ will be defined automatically.
 */
#define JASL_USE_STD_STRING_VIEW_AS_BASE

/**
 * If this macro is provided, _JASL_ classes will support construction and
 * assign operations from _STD_ classes.
 */
#define JASL_SUPPORT_STD_TO_JASL

/**
 * If this macro is provided, _JASL_ classes will support conversion to _STD_
 * classes.
 */
#define JASL_SUPPORT_JASL_TO_STD

/**
 * There are a few functions which can throw. (The design reason is the
 * similarity to _STD_.) The exceptions are easily avoidable. Defining this
 * macro will result of a call of
 * [std::terminate](http://en.cppreference.com/w/cpp/error/terminate) instead of
 * throwing an exception.
 *
 * __Note__: If this is defined, the tests will terminate when they test the
 *  exceptions.
 */
#define JASL_TERMINATE_ON_EXCEPTION_ON

/**
 * This macro is recommended to be defined during development and testing. It
 * helps identify unproper use of the library.
 */
#define JASL_ASSERT_ON

/**
 * This macro can be useful, if the JASL library couldn't recognise the the
 * existing
 * [std::string_view](http://en.cppreference.com/w/cpp/string/basic_string_view).
 *
 * __Remarks:__ This macro defines
 * _JASL_cpp_lib_string_view_ to 201606. This result can be achieved with
 * defining _JASL_cpp_lib_string_view_ manually.
 */
#define JASL_TESTIFY_STD_STRING_VIEW_EXISTS

/**
 * If this macro is provided, then _jasl::nonstd::string_view_ has no std::hash
 * specialization. The user can manually add one.
 */
#define JASL_DISABLE_JASL_STRING_VIEW_HASH

/**
 * In case of this macro if the
 * [std::string_view](http://en.cppreference.com/w/cpp/string/basic_string_view)
 * is provided _jasl::nonstd::string_view_ still will use murmurhash.
 */
#define JASL_FORCE_USE_MURMURHASH_HASH

#endif  // JASL_DOXYGEN_DOCUMENTATION
