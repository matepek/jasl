// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#include "jasl_string.hpp"
#include "test_helper.hpp"

int main() {
  jasl::simple_string y("onestatic");
  const char* dynPtr = "dynamic";
  // should cause compilation error
  y = dynPtr;
}
