// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#pragma once

#include "jasl_feature_test_macro.hpp"

#define JASL_INNER_STR(x) #x
#define JASL_STR(x) JASL_INNER_STR(x)

/*
 * Same as the first item of the CHANGELOG.md file
 */
#define JASL_VERSION_MAJOR 0
#define JASL_VERSION_MINOR 1
#define JASL_VERSION_PATCH 1
#define JASL_VERSION_DATE "2018-05-27"

static_assert(JASL_VERSION_MAJOR < 100, "JASL_VERSION error");
static_assert(JASL_VERSION_MINOR < 100, "JASL_VERSION error");
static_assert(JASL_VERSION_PATCH < 1000, "JASL_VERSION error");

/*
 * for example version 1.2.3 is: 10020003
 */
#define JASL_VERSION_NUMBER                         \
  (JASL_VERSION_PATCH + JASL_VERSION_MINOR * 1000 + \
   JASL_VERSION_MAJOR * 1000 * 100)

#define JASL_VERSION_STR                         \
  JASL_STR(JASL_VERSION_MAJOR)                   \
  "." JASL_STR(JASL_VERSION_MINOR) "." JASL_STR( \
      JASL_VERSION_PATCH) " (" JASL_VERSION_DATE ")"

/*
 * JASL_TERMINATE_ON_EXCEPTION_ON
 */
#ifdef JASL_TERMINATE_ON_EXCEPTION_ON
#include <exception>
#define JASL_THROW(exception) ::std::terminate()
#else
#define JASL_THROW(exception) throw(exception)
#endif

/*
 * JASL_ASSERT_ON
 */
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

/*
 * JASL_TESTIFY_STD_STRING_VIEW_EXISTS
 */
#if defined(JASL_TESTIFY_STD_STRING_VIEW_EXISTS)
#if !defined(JASL_cpp_lib_string_view)
#define JASL_cpp_lib_string_view 201606L
#include <string_view>
#endif
#endif
