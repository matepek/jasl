// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#include "jasl_static_string.hpp"
#include "test_helper.hpp"

int main() {
  jasl::static_string::base_type b("one", 3);
  // should cause compilation error
  jasl::static_string x(b);
}
