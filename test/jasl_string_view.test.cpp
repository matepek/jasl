// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#include <set>

#include "jasl_string_view.hpp"
#include "test_helper.hpp"

int main() {
  {
    jasl::string_view x;
    ASSERT_TRUE(x.data() == nullptr);
    ASSERT_TRUE(x.size() == 0);
  }
  {
    jasl::string_view x("", 0);
    ASSERT_TRUE(x.data() != nullptr);
    ASSERT_TRUE(*x.data() == 0);
    ASSERT_TRUE(x.size() == 0);
    ASSERT_TRUE(x.length() == 0);
    ASSERT_TRUE(x.empty());
  }
  {
    const char* oneStr = "one";
    jasl::string_view x(oneStr, 3);
    ASSERT_TRUE(x.data() == oneStr);
    ASSERT_TRUE(*x.data() == 'o');
    ASSERT_TRUE(x.size() == 3);
    ASSERT_TRUE(x.length() == 3);
    ASSERT_FALSE(x.empty());
    {
      jasl::string_view y(x);
      ASSERT_TRUE(y.data() == oneStr);
      ASSERT_TRUE(y.size() == 3);
    }
    {
      jasl::string_view y = x;
      ASSERT_TRUE(y.data() == oneStr);
      ASSERT_TRUE(y.size() == 3);
    }
    const char* twoStr = "twoo";
    {
      jasl::string_view y(x);
      y = jasl::string_view(twoStr, 4);
      ASSERT_TRUE(y.data() == twoStr);
      ASSERT_TRUE(y.size() == 4);
    }
  }
  {
    jasl::string_view x("a", 1), y("bb", 2);
    x.swap(y);
    ASSERT_TRUE(*x.data() == 'b');
    ASSERT_TRUE(x.size() == 2);
    ASSERT_TRUE(*y.data() == 'a');
    ASSERT_TRUE(y.size() == 1);
  }
  {
    jasl::string_view x("apble", 5), y;

    y = x.substr(2);
    ASSERT_TRUE(*y.data() == 'b');
    ASSERT_TRUE(y.size() == 3);

    y = x.substr(2, 1);
    ASSERT_TRUE(*y.data() == 'b');
    ASSERT_TRUE(y.size() == 1);

    y = x.substr(2, 10);
    ASSERT_TRUE(*y.data() == 'b');
    ASSERT_TRUE(y.size() == 3);

    y = x.substr(x.size());
    ASSERT_TRUE(y.size() == 0);
    ASSERT_EXCEPTION(x.substr(x.size() + 1), std::out_of_range);
  }

  // Becasue of constexpr the coverage reports it uncovered if the values are
  // directly used in the expression.
  size_t _0 = 0;
  size_t _1 = 1;
  size_t _2 = 2;

  ASSERT_TRUE(jasl::string_view(nullptr, _0) == jasl::string_view(nullptr, _0));
  ASSERT_FALSE(jasl::string_view(nullptr, _0) !=
               jasl::string_view(nullptr, _0));
  ASSERT_FALSE(jasl::string_view(nullptr, _0) < jasl::string_view(nullptr, _0));
  ASSERT_TRUE(jasl::string_view(nullptr, _0) <= jasl::string_view(nullptr, _0));
  ASSERT_FALSE(jasl::string_view(nullptr, _0) > jasl::string_view(nullptr, _0));
  ASSERT_TRUE(jasl::string_view(nullptr, _0) >= jasl::string_view(nullptr, _0));

  ASSERT_TRUE(jasl::string_view(nullptr, _0) == jasl::string_view("", _0));
  ASSERT_FALSE(jasl::string_view(nullptr, _0) != jasl::string_view("", _0));
  ASSERT_FALSE(jasl::string_view(nullptr, _0) < jasl::string_view("", _0));
  ASSERT_TRUE(jasl::string_view(nullptr, _0) <= jasl::string_view("", _0));
  ASSERT_FALSE(jasl::string_view(nullptr, _0) > jasl::string_view("", _0));
  ASSERT_TRUE(jasl::string_view(nullptr, _0) >= jasl::string_view("", _0));

  ASSERT_TRUE(jasl::string_view("", _0) == jasl::string_view("", _0));
  ASSERT_FALSE(jasl::string_view("", _0) != jasl::string_view("", _0));
  ASSERT_FALSE(jasl::string_view("", _0) < jasl::string_view("", _0));
  ASSERT_TRUE(jasl::string_view("", _0) <= jasl::string_view("", _0));
  ASSERT_FALSE(jasl::string_view("", _0) > jasl::string_view("", _0));
  ASSERT_TRUE(jasl::string_view("", _0) >= jasl::string_view("", _0));

  ASSERT_TRUE(jasl::string_view("a", _1) == jasl::string_view("a", _1));
  ASSERT_FALSE(jasl::string_view("a", _1) != jasl::string_view("a", _1));
  ASSERT_FALSE(jasl::string_view("a", _1) < jasl::string_view("a", _1));
  ASSERT_TRUE(jasl::string_view("a", _1) <= jasl::string_view("a", _1));
  ASSERT_FALSE(jasl::string_view("a", _1) > jasl::string_view("a", _1));
  ASSERT_TRUE(jasl::string_view("a", _1) >= jasl::string_view("a", _1));

  ASSERT_TRUE(jasl::string_view("a", _1) == jasl::string_view("ab", _1));
  ASSERT_FALSE(jasl::string_view("a", _1) != jasl::string_view("ab", _1));
  ASSERT_FALSE(jasl::string_view("a", _1) < jasl::string_view("ab", _1));
  ASSERT_TRUE(jasl::string_view("a", _1) <= jasl::string_view("ab", _1));
  ASSERT_FALSE(jasl::string_view("a", _1) > jasl::string_view("ab", _1));
  ASSERT_TRUE(jasl::string_view("a", _1) >= jasl::string_view("ab", _1));

  ASSERT_TRUE(jasl::string_view("ab", _1) == jasl::string_view("ab", _1));
  ASSERT_FALSE(jasl::string_view("ab", _1) != jasl::string_view("ab", _1));
  ASSERT_FALSE(jasl::string_view("ab", _1) < jasl::string_view("ab", _1));
  ASSERT_TRUE(jasl::string_view("ab", _1) <= jasl::string_view("ab", _1));
  ASSERT_FALSE(jasl::string_view("ab", _1) > jasl::string_view("ab", _1));
  ASSERT_TRUE(jasl::string_view("ab", _1) >= jasl::string_view("ab", _1));

  ASSERT_FALSE(jasl::string_view("a", _1) == jasl::string_view("b", _1));
  ASSERT_TRUE(jasl::string_view("a", _1) != jasl::string_view("b", _1));
  ASSERT_TRUE(jasl::string_view("a", _1) < jasl::string_view("b", _1));
  ASSERT_TRUE(jasl::string_view("a", _1) <= jasl::string_view("b", _1));
  ASSERT_FALSE(jasl::string_view("a", _1) > jasl::string_view("b", _1));
  ASSERT_FALSE(jasl::string_view("a", _1) >= jasl::string_view("b", _1));

  ASSERT_FALSE(jasl::string_view("a", _1) == jasl::string_view("ab", _2));
  ASSERT_TRUE(jasl::string_view("a", _1) != jasl::string_view("ab", _2));
  ASSERT_TRUE(jasl::string_view("a", _1) < jasl::string_view("ab", _2));
  ASSERT_TRUE(jasl::string_view("a", _1) <= jasl::string_view("ab", _2));
  ASSERT_FALSE(jasl::string_view("a", _1) > jasl::string_view("ab", _2));
  ASSERT_FALSE(jasl::string_view("a", _1) >= jasl::string_view("ab", _2));

  ASSERT_FALSE(jasl::string_view("ab", _2) == jasl::string_view("a", _1));
  ASSERT_TRUE(jasl::string_view("ab", _2) != jasl::string_view("a", _1));
  ASSERT_FALSE(jasl::string_view("ab", _2) < jasl::string_view("a", _1));
  ASSERT_FALSE(jasl::string_view("ab", _2) <= jasl::string_view("a", _1));
  ASSERT_TRUE(jasl::string_view("ab", _2) > jasl::string_view("a", _1));
  ASSERT_TRUE(jasl::string_view("ab", _2) >= jasl::string_view("a", _1));

  // test only syntax
  std::cout << jasl::string_view("", 0);

  // test only syntax
  std::hash<jasl::string_view>{}(jasl::string_view("", 0));

  {
    auto test_with_set = [] {
      std::set<jasl::string_view> s;
      jasl::string_view empty("", 0);
      jasl::string_view small("small", 5);
      char array[1024]{};
      jasl::string_view longg(array, 1024);
      ASSERT_TRUE(s.empty());
      s.insert(empty);
      ASSERT_TRUE(s.count(empty) == 1);
      ASSERT_TRUE(s.count(small) == 0);
      ASSERT_TRUE(s.count(longg) == 0);
      s.insert(small);
      ASSERT_TRUE(s.count(empty) == 1);
      ASSERT_TRUE(s.count(small) == 1);
      ASSERT_TRUE(s.count(longg) == 0);
      s.insert(longg);
      ASSERT_TRUE(s.count(empty) == 1);
      ASSERT_TRUE(s.count(small) == 1);
      ASSERT_TRUE(s.count(longg) == 1);

      ASSERT_TRUE(s.size() == 3);
      return 0;
    };
#if defined(__has_feature)
#if __has_feature(memory_sanitizer)
    ASSERT_TRUE(test_with_set != nullptr);
#else
    ASSERT_TRUE(test_with_set() == 0);
#endif
#else
    ASSERT_TRUE(test_with_set() == 0);
#endif
  }

  {
    jasl::string_view one("one", 3);

    auto it = one.begin();
    auto cit = one.cbegin();
    ASSERT_TRUE(*it++ == 'o');
    ASSERT_TRUE(*cit++ == 'o');
    ASSERT_TRUE(*it++ == 'n');
    ASSERT_TRUE(*cit++ == 'n');
    ASSERT_TRUE(*it++ == 'e');
    ASSERT_TRUE(*cit++ == 'e');
    ASSERT_TRUE(it == one.end());
    ASSERT_TRUE(cit == one.cend());

    auto rit = one.rbegin();
    auto crit = one.crbegin();
    ASSERT_TRUE(*rit++ == 'e');
    ASSERT_TRUE(*crit++ == 'e');
    ASSERT_TRUE(*rit++ == 'n');
    ASSERT_TRUE(*crit++ == 'n');
    ASSERT_TRUE(*rit++ == 'o');
    ASSERT_TRUE(*crit++ == 'o');
    ASSERT_TRUE(rit == one.rend());
    ASSERT_TRUE(crit == one.crend());

    ASSERT_TRUE(one[0] == 'o');
    ASSERT_TRUE(one[1] == 'n');
    ASSERT_TRUE(one[2] == 'e');
    ASSERT_TRUE(one.at(0) == 'o');
    ASSERT_TRUE(one.at(1) == 'n');
    ASSERT_TRUE(one.at(2) == 'e');
    ASSERT_EXCEPTION(one.at(3), std::out_of_range);

    ASSERT_TRUE(one.front() == 'o');
    ASSERT_TRUE(one.back() == 'e');

    {
      char fill_me[3] = {};
      ASSERT_TRUE(one.copy(fill_me, 2, 1) == 2);
      ASSERT_TRUE(fill_me[0] == 'n');
      ASSERT_TRUE(fill_me[1] == 'e');
      ASSERT_TRUE(fill_me[2] == 0);
      ASSERT_TRUE(one.copy(fill_me + 2, 2, 3) == 0);
      ASSERT_TRUE(fill_me[2] == 0);
      ASSERT_EXCEPTION(one.copy(fill_me, 2, 4), std::out_of_range);
    }
    {
      auto rp = one;
      rp.remove_prefix(1);
      ASSERT_TRUE(rp.size() == 2);
      ASSERT_TRUE(rp[0] == 'n');
      ASSERT_TRUE(rp[1] == 'e');
    }
    {
      auto rs = one;
      rs.remove_suffix(1);
      ASSERT_TRUE(rs.size() == 2);
      ASSERT_TRUE(rs[0] == 'o');
      ASSERT_TRUE(rs[1] == 'n');
    }
  }
  {
    jasl::string_view x("012", 3);
    ASSERT_TRUE(x.find(jasl::string_view("0", 1)) == 0);
    ASSERT_TRUE(x.find(jasl::string_view("1", 1)) == 1);
    ASSERT_TRUE(x.find(jasl::string_view("2", 1)) == 2);
    ASSERT_TRUE(x.find(jasl::string_view("3", 1)) == jasl::string_view::npos);

    jasl::string_view y("one two three", 13);
    ASSERT_TRUE(y.find(jasl::string_view("one", 3)) == 0);
    ASSERT_TRUE(y.find(jasl::string_view("two", 3)) == 4);
    ASSERT_TRUE(y.find(jasl::string_view("three", 5)) == 8);
    ASSERT_TRUE(y.find(jasl::string_view("twoX", 4)) ==
                jasl::string_view::npos);
    ASSERT_TRUE(y.find(jasl::string_view("threeX", 6)) ==
                jasl::string_view::npos);
  }

  return 0;
}
