// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#include <cstring>
#include <iostream>

#include "jasl_inner/jasl_murmurhash3.hpp"
#include "test_helper.hpp"

static const size_t res_size = 16;

bool equals(const unsigned char (&result)[res_size],
            const unsigned char (&right)[res_size]) {
  if (0 == std::memcmp(result, right, res_size))
    return true;
  for (size_t i = 0; i < res_size; ++i)
    std::cout << "0x" << std::hex << static_cast<int>(result[i]) << ", ";
  std::cout << std::endl;
  return false;
}

// It is mostly for coverage.
int main() {
  const uint32_t seed = 33;

  const int input_size = 24;
  const unsigned char input[input_size]{};
  unsigned char res[res_size]{};

  {
    for (int i = input_size - res_size; i <= input_size; ++i)
      jasl::murmurhash3::MurmurHash3_x86_128(input, i, seed, res);

    ASSERT_TRUE(equals(res, {0x15, 0xf4, 0xab, 0xa9, 0x9d, 0xbf, 0xde, 0x79,
                             0x53, 0x4a, 0x00, 0xb9, 0x88, 0x77, 0x9f, 0x3c}));
  }
  {
    for (int i = input_size - res_size; i <= input_size; ++i)
      jasl::murmurhash3::MurmurHash3_x64_128(input, i, seed, res);

    ASSERT_TRUE(equals(res, {0x2a, 0x92, 0x3f, 0xfe, 0x90, 0x12, 0x47, 0x54,
                             0xf2, 0x7d, 0xb2, 0xba, 0x02, 0xd2, 0x17, 0x14}));
  }

  return 0;
}
