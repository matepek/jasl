// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#include "jasl/jasl_string_view.hpp"
#include "test_helper.hpp"

int main() {
  // should cause compilation error
  const char* ptr = "apple";
  jasl::nonstd::basic_string_view<char> x(ptr);

  ASSERT_TRUE(x == x);
  return 0;
}
