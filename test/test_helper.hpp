// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#pragma once

#include <iostream>

#define ASSERT_TRUE(expr)                                              \
  do {                                                                 \
    if (!(expr)) {                                                     \
      std::cerr << __FILE__ << '(' << __LINE__ << "):" << std::endl    \
                << "Expression: " << #expr << " failed." << std::endl; \
      return 1;                                                        \
    }                                                                  \
  } while (false)

#define ASSERT_FALSE(expr) ASSERT_TRUE(!(expr))

#define ASSERT_EXCEPTION(expr, expected_exception)   \
  do {                                               \
    try {                                            \
      expr;                                          \
      bool no_exception_has_been_thrown = true;      \
      ASSERT_FALSE(no_exception_has_been_thrown);    \
    } catch (const expected_exception&) {            \
    } catch (...) {                                  \
      bool wrong_exception_has_been_caught = true;   \
      ASSERT_FALSE(wrong_exception_has_been_caught); \
    }                                                \
  } while (false)

template<typename T>
void unused_variable(T&&) {}
