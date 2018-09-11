// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#include "jasl/jasl_string.hpp"
#include "test_helper.hpp"

int main() {
  jasl::string x("onestatic");
  const char* dynPtr = "dynamic";
  // should cause compilation error
  x = dynPtr;

  ASSERT_TRUE(x == x);
  return 0;
}
