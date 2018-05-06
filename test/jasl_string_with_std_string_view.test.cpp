// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#include "jasl_string.hpp"

#if defined(__cpp_lib_string_view) && \
    defined(JASL_USE_JASL_STRING_VIEW_TYPE_AS_BASE)
#error "Should NOT have been defined!"
#endif

#include "jasl_string_common.test.hpp"

int main() {
  return common();
}
