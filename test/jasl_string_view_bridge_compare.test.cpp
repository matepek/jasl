// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#include <set>

#include "jasl_static_string.hpp"
#include "jasl_string_view.hpp"
#include "test_helper.hpp"

template <size_t N1, size_t N2>
int eq_helper(const char (&left)[N1], const char (&right)[N2]) {
  return jasl::static_string(left) == jasl::static_string(right);
}

template <size_t N>
jasl::string_view jasl_string_view(const char (&s)[N]) {
  return jasl::string_view(s, N - 1);
}

int main() {
  // Becasue of constexpr the coverage reports it uncovered if the values are
  // directly used in the expression.
  ASSERT_FALSE(eq_helper("a", "ab"));

  // jasl::static_string - jasl::static_string
  ASSERT_TRUE(jasl::static_string("") == jasl::static_string(""));
  ASSERT_FALSE(jasl::static_string("") != jasl::static_string(""));
  ASSERT_FALSE(jasl::static_string("") < jasl::static_string(""));
  ASSERT_TRUE(jasl::static_string("") <= jasl::static_string(""));
  ASSERT_FALSE(jasl::static_string("") > jasl::static_string(""));
  ASSERT_TRUE(jasl::static_string("") >= jasl::static_string(""));

  ASSERT_TRUE(jasl::static_string("a") == jasl::static_string("a"));
  ASSERT_FALSE(jasl::static_string("a") != jasl::static_string("a"));
  ASSERT_FALSE(jasl::static_string("a") < jasl::static_string("a"));
  ASSERT_TRUE(jasl::static_string("a") <= jasl::static_string("a"));
  ASSERT_FALSE(jasl::static_string("a") > jasl::static_string("a"));
  ASSERT_TRUE(jasl::static_string("a") >= jasl::static_string("a"));

  ASSERT_TRUE(jasl::static_string("ab") == jasl::static_string("ab"));
  ASSERT_FALSE(jasl::static_string("ab") != jasl::static_string("ab"));
  ASSERT_FALSE(jasl::static_string("ab") < jasl::static_string("ab"));
  ASSERT_TRUE(jasl::static_string("ab") <= jasl::static_string("ab"));
  ASSERT_FALSE(jasl::static_string("ab") > jasl::static_string("ab"));
  ASSERT_TRUE(jasl::static_string("ab") >= jasl::static_string("ab"));

  ASSERT_FALSE(jasl::static_string("a") == jasl::static_string("b"));
  ASSERT_TRUE(jasl::static_string("a") != jasl::static_string("b"));
  ASSERT_TRUE(jasl::static_string("a") < jasl::static_string("b"));
  ASSERT_TRUE(jasl::static_string("a") <= jasl::static_string("b"));
  ASSERT_FALSE(jasl::static_string("a") > jasl::static_string("b"));
  ASSERT_FALSE(jasl::static_string("a") >= jasl::static_string("b"));

  ASSERT_FALSE(jasl::static_string("a") == jasl::static_string("ab"));
  ASSERT_TRUE(jasl::static_string("a") != jasl::static_string("ab"));
  ASSERT_TRUE(jasl::static_string("a") < jasl::static_string("ab"));
  ASSERT_TRUE(jasl::static_string("a") <= jasl::static_string("ab"));
  ASSERT_FALSE(jasl::static_string("a") > jasl::static_string("ab"));
  ASSERT_FALSE(jasl::static_string("a") >= jasl::static_string("ab"));

  ASSERT_FALSE(jasl::static_string("ab") == jasl::static_string("a"));
  ASSERT_TRUE(jasl::static_string("ab") != jasl::static_string("a"));
  ASSERT_FALSE(jasl::static_string("ab") < jasl::static_string("a"));
  ASSERT_FALSE(jasl::static_string("ab") <= jasl::static_string("a"));
  ASSERT_TRUE(jasl::static_string("ab") > jasl::static_string("a"));
  ASSERT_TRUE(jasl::static_string("ab") >= jasl::static_string("a"));

  // jasl::static_string - jasl_string_view
  ASSERT_TRUE(jasl::static_string("") == jasl_string_view(""));
  ASSERT_FALSE(jasl::static_string("") != jasl_string_view(""));
  ASSERT_FALSE(jasl::static_string("") < jasl_string_view(""));
  ASSERT_TRUE(jasl::static_string("") <= jasl_string_view(""));
  ASSERT_FALSE(jasl::static_string("") > jasl_string_view(""));
  ASSERT_TRUE(jasl::static_string("") >= jasl_string_view(""));

  ASSERT_TRUE(jasl::static_string("a") == jasl_string_view("a"));
  ASSERT_FALSE(jasl::static_string("a") != jasl_string_view("a"));
  ASSERT_FALSE(jasl::static_string("a") < jasl_string_view("a"));
  ASSERT_TRUE(jasl::static_string("a") <= jasl_string_view("a"));
  ASSERT_FALSE(jasl::static_string("a") > jasl_string_view("a"));
  ASSERT_TRUE(jasl::static_string("a") >= jasl_string_view("a"));

  ASSERT_TRUE(jasl::static_string("ab") == jasl_string_view("ab"));
  ASSERT_FALSE(jasl::static_string("ab") != jasl_string_view("ab"));
  ASSERT_FALSE(jasl::static_string("ab") < jasl_string_view("ab"));
  ASSERT_TRUE(jasl::static_string("ab") <= jasl_string_view("ab"));
  ASSERT_FALSE(jasl::static_string("ab") > jasl_string_view("ab"));
  ASSERT_TRUE(jasl::static_string("ab") >= jasl_string_view("ab"));

  ASSERT_FALSE(jasl::static_string("a") == jasl_string_view("b"));
  ASSERT_TRUE(jasl::static_string("a") != jasl_string_view("b"));
  ASSERT_TRUE(jasl::static_string("a") < jasl_string_view("b"));
  ASSERT_TRUE(jasl::static_string("a") <= jasl_string_view("b"));
  ASSERT_FALSE(jasl::static_string("a") > jasl_string_view("b"));
  ASSERT_FALSE(jasl::static_string("a") >= jasl_string_view("b"));

  ASSERT_FALSE(jasl::static_string("a") == jasl_string_view("ab"));
  ASSERT_TRUE(jasl::static_string("a") != jasl_string_view("ab"));
  ASSERT_TRUE(jasl::static_string("a") < jasl_string_view("ab"));
  ASSERT_TRUE(jasl::static_string("a") <= jasl_string_view("ab"));
  ASSERT_FALSE(jasl::static_string("a") > jasl_string_view("ab"));
  ASSERT_FALSE(jasl::static_string("a") >= jasl_string_view("ab"));

  ASSERT_FALSE(jasl::static_string("ab") == jasl_string_view("a"));
  ASSERT_TRUE(jasl::static_string("ab") != jasl_string_view("a"));
  ASSERT_FALSE(jasl::static_string("ab") < jasl_string_view("a"));
  ASSERT_FALSE(jasl::static_string("ab") <= jasl_string_view("a"));
  ASSERT_TRUE(jasl::static_string("ab") > jasl_string_view("a"));
  ASSERT_TRUE(jasl::static_string("ab") >= jasl_string_view("a"));

  // jasl_string_view - jasl::static_string
  ASSERT_TRUE(jasl_string_view("") == jasl::static_string(""));
  ASSERT_FALSE(jasl_string_view("") != jasl::static_string(""));
  ASSERT_FALSE(jasl_string_view("") < jasl::static_string(""));
  ASSERT_TRUE(jasl_string_view("") <= jasl::static_string(""));
  ASSERT_FALSE(jasl_string_view("") > jasl::static_string(""));
  ASSERT_TRUE(jasl_string_view("") >= jasl::static_string(""));

  ASSERT_TRUE(jasl_string_view("a") == jasl::static_string("a"));
  ASSERT_FALSE(jasl_string_view("a") != jasl::static_string("a"));
  ASSERT_FALSE(jasl_string_view("a") < jasl::static_string("a"));
  ASSERT_TRUE(jasl_string_view("a") <= jasl::static_string("a"));
  ASSERT_FALSE(jasl_string_view("a") > jasl::static_string("a"));
  ASSERT_TRUE(jasl_string_view("a") >= jasl::static_string("a"));

  ASSERT_TRUE(jasl_string_view("ab") == jasl::static_string("ab"));
  ASSERT_FALSE(jasl_string_view("ab") != jasl::static_string("ab"));
  ASSERT_FALSE(jasl_string_view("ab") < jasl::static_string("ab"));
  ASSERT_TRUE(jasl_string_view("ab") <= jasl::static_string("ab"));
  ASSERT_FALSE(jasl_string_view("ab") > jasl::static_string("ab"));
  ASSERT_TRUE(jasl_string_view("ab") >= jasl::static_string("ab"));

  ASSERT_FALSE(jasl_string_view("a") == jasl::static_string("b"));
  ASSERT_TRUE(jasl_string_view("a") != jasl::static_string("b"));
  ASSERT_TRUE(jasl_string_view("a") < jasl::static_string("b"));
  ASSERT_TRUE(jasl_string_view("a") <= jasl::static_string("b"));
  ASSERT_FALSE(jasl_string_view("a") > jasl::static_string("b"));
  ASSERT_FALSE(jasl_string_view("a") >= jasl::static_string("b"));

  ASSERT_FALSE(jasl_string_view("a") == jasl::static_string("ab"));
  ASSERT_TRUE(jasl_string_view("a") != jasl::static_string("ab"));
  ASSERT_TRUE(jasl_string_view("a") < jasl::static_string("ab"));
  ASSERT_TRUE(jasl_string_view("a") <= jasl::static_string("ab"));
  ASSERT_FALSE(jasl_string_view("a") > jasl::static_string("ab"));
  ASSERT_FALSE(jasl_string_view("a") >= jasl::static_string("ab"));

  ASSERT_FALSE(jasl_string_view("ab") == jasl::static_string("a"));
  ASSERT_TRUE(jasl_string_view("ab") != jasl::static_string("a"));
  ASSERT_FALSE(jasl_string_view("ab") < jasl::static_string("a"));
  ASSERT_FALSE(jasl_string_view("ab") <= jasl::static_string("a"));
  ASSERT_TRUE(jasl_string_view("ab") > jasl::static_string("a"));
  ASSERT_TRUE(jasl_string_view("ab") >= jasl::static_string("a"));

  return 0;
}
