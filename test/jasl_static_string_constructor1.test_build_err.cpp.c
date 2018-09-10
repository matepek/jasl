// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#include "jasl/jasl_static_string.hpp"
#include "test_helper.hpp"

int main() {
  // should cause compilation error
  jasl::static_string x("", 0);

  ASSERT_TRUE(x == x);
  return 0;
}
