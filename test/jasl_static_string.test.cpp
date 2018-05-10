// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#include <set>

#include "jasl_static_string.hpp"
#include "test_helper.hpp"

template <size_t N1, size_t N2>
int eq_helper(const char (&left)[N1], const char (&right)[N2]) {
  return jasl::static_string(left) == jasl::static_string(right);
}

int main() {
  {
    jasl::static_string x;
    ASSERT_TRUE(x.data() != nullptr);
    ASSERT_TRUE(*x.data() == 0);
    ASSERT_TRUE(x.size() == 0);
  }
  {
    jasl::static_string x = "one";
    ASSERT_TRUE(*x.data() == 'o');
    ASSERT_TRUE(x.size() == 3);
  }
  {
    jasl::static_string x("");
    ASSERT_TRUE(*x.data() == 0);
    ASSERT_TRUE(x.size() == 0);
  }
  {
    const char oneStr[] = "one";
    jasl::static_string x(oneStr);
    ASSERT_TRUE(x.data() == oneStr);
    ASSERT_TRUE(*x.data() == 'o');
    ASSERT_TRUE(x.size() == 3);
    {
      jasl::static_string y(x);
      ASSERT_TRUE(y.data() == oneStr);
      ASSERT_TRUE(y.size() == 3);
    }
    {
      jasl::static_string y = x;
      ASSERT_TRUE(y.data() == oneStr);
      ASSERT_TRUE(y.size() == 3);
    }
    const char twoStr[] = "twoo";
    {
      jasl::static_string y(oneStr);
      y = jasl::static_string(twoStr);
      ASSERT_TRUE(y.data() == twoStr);
      ASSERT_TRUE(y.size() == 4);
    }
    {
      jasl::static_string y(oneStr);
      y = twoStr;
      ASSERT_TRUE(y.data() == twoStr);
      ASSERT_TRUE(y.size() == 4);
    }
  }
  {
    const char oneStr[] = {'o', 'n', 'e'};
    jasl::static_string x(oneStr);
    ASSERT_TRUE(x.data() == oneStr);
    ASSERT_TRUE(*x.data() == 'o');
    ASSERT_TRUE(x.size() == 3);
    {
      jasl::static_string y(x);
      ASSERT_TRUE(y.data() == oneStr);
      ASSERT_TRUE(y.size() == 3);
    }
    {
      jasl::static_string y = x;
      ASSERT_TRUE(y.data() == oneStr);
      ASSERT_TRUE(y.size() == 3);
    }
    const char twoStr[] = {'t', 'w', 'o', 'o'};
    {
      jasl::static_string y(oneStr);
      y = jasl::static_string(twoStr);
      ASSERT_TRUE(y.data() == twoStr);
      ASSERT_TRUE(y.size() == 4);
    }
    {
      jasl::static_string y(oneStr);
      y = twoStr;
      ASSERT_TRUE(y.data() == twoStr);
      ASSERT_TRUE(y.size() == 4);
    }
  }
  {
    jasl::static_string x("a"), y("bb");
    x.swap(y);
    ASSERT_TRUE(*x.data() == 'b');
    ASSERT_TRUE(x.size() == 2);
    ASSERT_TRUE(*y.data() == 'a');
    ASSERT_TRUE(y.size() == 1);
  }
  {
    jasl::static_string x("apble"), y;

    y = x.substr(2);
    ASSERT_TRUE(*y.data() == 'b');
    ASSERT_TRUE(y.size() == 3);

    jasl::static_string::base_type z(x.substr(2, 1));
    ASSERT_TRUE(*z.data() == 'b');
    ASSERT_TRUE(z.size() == 1);

    y = x.substr(x.size());
    ASSERT_TRUE(y.size() == 0);
    ASSERT_EXCEPTION(x.substr(x.size() + 1), std::out_of_range);
  }
  {
    jasl::static_string x("apble");
    [](jasl::static_string::base_type) {}(x);
    [](const jasl::static_string::base_type&) {}(x);
  }

  ASSERT_TRUE(jasl::static_string("") == jasl::static_string(""));
  ASSERT_FALSE(jasl::static_string("") != jasl::static_string(""));
  ASSERT_FALSE(jasl::static_string("") < jasl::static_string(""));
  ASSERT_TRUE(jasl::static_string("") <= jasl::static_string(""));
  ASSERT_FALSE(jasl::static_string("") > jasl::static_string(""));
  ASSERT_TRUE(jasl::static_string("") >= jasl::static_string(""));

  ASSERT_TRUE(jasl::static_string("a") == jasl::static_string("a"));
  ASSERT_FALSE(jasl::static_string("a") != jasl::static_string("a"));
  ASSERT_FALSE(jasl::static_string("a") < jasl::static_string("a"));
  ASSERT_TRUE(jasl::static_string("a") <= jasl::static_string("a"));
  ASSERT_FALSE(jasl::static_string("a") > jasl::static_string("a"));
  ASSERT_TRUE(jasl::static_string("a") >= jasl::static_string("a"));

  ASSERT_TRUE(jasl::static_string("ab") == jasl::static_string("ab"));
  ASSERT_FALSE(jasl::static_string("ab") != jasl::static_string("ab"));
  ASSERT_FALSE(jasl::static_string("ab") < jasl::static_string("ab"));
  ASSERT_TRUE(jasl::static_string("ab") <= jasl::static_string("ab"));
  ASSERT_FALSE(jasl::static_string("ab") > jasl::static_string("ab"));
  ASSERT_TRUE(jasl::static_string("ab") >= jasl::static_string("ab"));

  ASSERT_FALSE(jasl::static_string("a") == jasl::static_string("b"));
  ASSERT_TRUE(jasl::static_string("a") != jasl::static_string("b"));
  ASSERT_TRUE(jasl::static_string("a") < jasl::static_string("b"));
  ASSERT_TRUE(jasl::static_string("a") <= jasl::static_string("b"));
  ASSERT_FALSE(jasl::static_string("a") > jasl::static_string("b"));
  ASSERT_FALSE(jasl::static_string("a") >= jasl::static_string("b"));

  // Becasue of constexpr the coverage reports it uncovered if the values are
  // directly used in the expression.
  ASSERT_FALSE(eq_helper("a", "ab"));

  ASSERT_FALSE(jasl::static_string("a") == jasl::static_string("ab"));
  ASSERT_TRUE(jasl::static_string("a") != jasl::static_string("ab"));
  ASSERT_TRUE(jasl::static_string("a") < jasl::static_string("ab"));
  ASSERT_TRUE(jasl::static_string("a") <= jasl::static_string("ab"));
  ASSERT_FALSE(jasl::static_string("a") > jasl::static_string("ab"));
  ASSERT_FALSE(jasl::static_string("a") >= jasl::static_string("ab"));

  ASSERT_FALSE(jasl::static_string("ab") == jasl::static_string("a"));
  ASSERT_TRUE(jasl::static_string("ab") != jasl::static_string("a"));
  ASSERT_FALSE(jasl::static_string("ab") < jasl::static_string("a"));
  ASSERT_FALSE(jasl::static_string("ab") <= jasl::static_string("a"));
  ASSERT_TRUE(jasl::static_string("ab") > jasl::static_string("a"));
  ASSERT_TRUE(jasl::static_string("ab") >= jasl::static_string("a"));

  // test only syntax
  std::cout << jasl::static_string("");

  // test only syntax
  std::hash<jasl::static_string::base_type>{}(jasl::static_string(""));

  // test only hash
  std::set<jasl::static_string>().size();

  return 0;
}
