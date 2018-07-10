// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#include <set>

#include "jasl_static_string.hpp"
#include "jasl_string_view.hpp"
#include "test_helper.hpp"

// Becasue of constexpr the coverage reports it uncovered if the values are
// directly used in the expression.

template <typename Type1, typename Type2 = Type1, size_t N1, size_t N2>
bool equal_to(const char (&left)[N1], const char (&right)[N2]) {
  return (Type1(left) == Type2(right));
}

template <typename Type1, typename Type2 = Type1, size_t N1, size_t N2>
bool greater(const char (&left)[N1], const char (&right)[N2]) {
  return (Type1(left) < Type2(right));
}

template <typename Type1, typename Type2 = Type1, size_t N1, size_t N2>
bool greater_equal(const char (&left)[N1], const char (&right)[N2]) {
  return (Type1(left) <= Type2(right));
}

template <typename Type1, typename Type2 = Type1, size_t N1, size_t N2>
bool less(const char (&left)[N1], const char (&right)[N2]) {
  return (Type1(left) > Type2(right));
}

template <typename Type1, typename Type2 = Type1, size_t N1, size_t N2>
bool less_equal(const char (&left)[N1], const char (&right)[N2]) {
  return (Type1(left) >= Type2(right));
}

template <typename Type1, typename Type2 = Type1, size_t N1, size_t N2>
bool not_equal_to(const char (&left)[N1], const char (&right)[N2]) {
  return (Type1(left) != Type2(right));
}

template <size_t N>
jasl::string_view jasl_string_view(const char (&s)[N]) {
  return jasl::string_view(s, N - 1);
}

int main() {
  // jasl::static_string - jasl::static_string
  ASSERT_TRUE(equal_to<jasl::static_string>("", ""));
  ASSERT_FALSE(not_equal_to<jasl::static_string>("", ""));
  ASSERT_FALSE(greater<jasl::static_string>("", ""));
  ASSERT_TRUE(greater_equal<jasl::static_string>("", ""));
  ASSERT_FALSE(less<jasl::static_string>("", ""));
  ASSERT_TRUE(less_equal<jasl::static_string>("", ""));

  ASSERT_TRUE(equal_to<jasl::static_string>("a", "a"));
  ASSERT_FALSE(not_equal_to<jasl::static_string>("a", "a"));
  ASSERT_FALSE(greater<jasl::static_string>("a", "a"));
  ASSERT_TRUE(greater_equal<jasl::static_string>("a", "a"));
  ASSERT_FALSE(less<jasl::static_string>("a", "a"));
  ASSERT_TRUE(less_equal<jasl::static_string>("a", "a"));

  ASSERT_TRUE(equal_to<jasl::static_string>("ab", "ab"));
  ASSERT_FALSE(not_equal_to<jasl::static_string>("ab", "ab"));
  ASSERT_FALSE(greater<jasl::static_string>("ab", "ab"));
  ASSERT_TRUE(greater_equal<jasl::static_string>("ab", "ab"));
  ASSERT_FALSE(less<jasl::static_string>("ab", "ab"));
  ASSERT_TRUE(less_equal<jasl::static_string>("ab", "ab"));

  ASSERT_FALSE(equal_to<jasl::static_string>("a", "b"));
  ASSERT_TRUE(not_equal_to<jasl::static_string>("a", "b"));
  ASSERT_TRUE(greater<jasl::static_string>("a", "b"));
  ASSERT_TRUE(greater_equal<jasl::static_string>("a", "b"));
  ASSERT_FALSE(less<jasl::static_string>("a", "b"));
  ASSERT_FALSE(less_equal<jasl::static_string>("a", "b"));

  ASSERT_FALSE(equal_to<jasl::static_string>("a", "ab"));
  ASSERT_TRUE(not_equal_to<jasl::static_string>("a", "ab"));
  ASSERT_TRUE(greater<jasl::static_string>("a", "ab"));
  ASSERT_TRUE(greater_equal<jasl::static_string>("a", "ab"));
  ASSERT_FALSE(less<jasl::static_string>("a", "ab"));
  ASSERT_FALSE(less_equal<jasl::static_string>("a", "ab"));

  ASSERT_FALSE(equal_to<jasl::static_string>("ab", "a"));
  ASSERT_TRUE(not_equal_to<jasl::static_string>("ab", "a"));
  ASSERT_FALSE(greater<jasl::static_string>("ab", "a"));
  ASSERT_FALSE(greater_equal<jasl::static_string>("ab", "a"));
  ASSERT_TRUE(less<jasl::static_string>("ab", "a"));
  ASSERT_TRUE(less_equal<jasl::static_string>("ab", "a"));

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
