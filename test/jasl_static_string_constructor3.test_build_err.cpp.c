// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#include "jasl/static_string.hpp"
#include "test_helper.hpp"

int main() {
  const char* oneStr = "one";
  // should cause compilation error
  jasl::static_string x(oneStr);

  ASSERT_TRUE(x == x);
  return 0;
}
