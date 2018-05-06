// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#include "jasl_string.hpp"

#if !defined(JASL_USE_JASL_STRING_VIEW_AS_BASE)
#error "Should have been defined!"
#endif

#include "jasl_string_common.test.hpp"

int main() {
  return common();
}
