// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#pragma once

#include "jasl_common.hpp"

#ifdef _MSC_VER  // msvc

// https://msdn.microsoft.com/en-us/library/d9x1s805.aspx
#define JASL_DIAGNOSTIC_PUSH() __pragma(warning(push))
#define JASL_DIAGNOSTIC_POP() __pragma(warning(pop))

#define JASL_DIAGNOSTIC_IGNORED_MSVC(id) __pragma(warning(disable : id))
#define JASL_DIAGNOSTIC_IGNORED_GCC(id)
#define JASL_DIAGNOSTIC_IGNORED_GCC_SINCE7(id)
#define JASL_DIAGNOSTIC_IGNORED_CLANG(id)

#define JASL_DIAGNOSTIC_IGNORED(id) JASL_DIAGNOSTIC_IGNORED_MSVC(id)

#define JASL_WARNING_OLD_STYLE_CAST
#define JASL_WARNING_SIGN_CONVERSION
#define JASL_WARNING_IMPLICIT_FALLTHROUGH
#define JAS_WARNING_CAST_ALING

#elif defined(__GNUG__) && !defined(__clang__)  // gcc

#define JASL_DIAGNOSTIC_PUSH() _Pragma("GCC diagnostic push")
#define JASL_DIAGNOSTIC_POP() _Pragma("GCC diagnostic pop")

#define JASL_DIAGNOSTIC_IGNORED_MSVC(id)
#define JASL_DIAGNOSTIC_IGNORED_GCC(id) \
  _Pragma(JASL_STR(GCC diagnostic ignored id))
#if __GNUC__ >= 7L
#define JASL_DIAGNOSTIC_IGNORED_GCC_SINCE7(id) JASL_DIAGNOSTIC_IGNORED_GCC(id)
#else
#define JASL_DIAGNOSTIC_IGNORED_GCC_SINCE7(id)
#endif
#define JASL_DIAGNOSTIC_IGNORED_CLANG(id)

#define JASL_DIAGNOSTIC_IGNORED(id) JASL_DIAGNOSTIC_IGNORED_GCC(id)

#define JASL_WARNING_OLD_STYLE_CAST
#define JASL_WARNING_SIGN_CONVERSION
#define JASL_WARNING_IMPLICIT_FALLTHROUGH "-Wimplicit-fallthrough"
#define JAS_WARNING_CAST_ALING

#elif defined(__clang__)  // clang

// https://stackoverflow.com/questions/13826722/how-do-i-define-a-macro-with-multiple-pragmas-for-clang
#define JASL_DIAGNOSTIC_PUSH() _Pragma("clang diagnostic push")
#define JASL_DIAGNOSTIC_POP() _Pragma("clang diagnostic pop")

#define JASL_DIAGNOSTIC_IGNORED_MSVC(id)
#define JASL_DIAGNOSTIC_IGNORED_GCC(id)
#define JASL_DIAGNOSTIC_IGNORED_GCC_SINCE7(id)
#define JASL_DIAGNOSTIC_IGNORED_CLANG(id) \
  _Pragma(JASL_STR(clang diagnostic ignored id))

#define JASL_DIAGNOSTIC_IGNORED(id) JASL_DIAGNOSTIC_IGNORED_CLANG(id)

#define JASL_WARNING_OLD_STYLE_CAST "-Wold-style-cast"
#define JASL_WARNING_SIGN_CONVERSION "-Wsign-conversion"
#define JASL_WARNING_IMPLICIT_FALLTHROUGH "-Wimplicit-fallthrough"
#define JAS_WARNING_CAST_ALING "-Wcast-align"

#else

static_assert(false, "Unsupported compiler!");

#endif
