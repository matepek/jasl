// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#pragma once

#ifdef _MSC_VER  // msvc

#define JASL_DIAGNOSTIC_PUSH() __pragma(warning(push))
#define JASL_DIAGNOSTIC_POP() __pragma(warning(pop))
#define JASL_DIAGNOSTIC_DISABLE_WARNING_RESERVER_MACRO()

#elif defined(__GNUG__) && !defined(__clang__)  // gcc

#define JASL_DIAGNOSTIC_PUSH()
#define JASL_DIAGNOSTIC_POP()
#define JASL_DIAGNOSTIC_DISABLE_WARNING_RESERVER_MACRO()

#elif defined(__clang__)  // clang

#define JASL_DIAGNOSTIC_PUSH() _Pragma("clang diagnostic push")
#define JASL_DIAGNOSTIC_POP() _Pragma("clang diagnostic pop")
#define JASL_DIAGNOSTIC_DISABLE_WARNING_RESERVER_MACRO() \
  _Pragma("clang diagnostic ignored \"-Wreserved-id-macro\"")

#else

static_assert(false, "Unsupported compiler!");

#endif  // <compiler types>
