// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#include <set>

#include "test_helper.hpp"

int common() {
  {
    jasl::string x;
    ASSERT_TRUE(x.data() != nullptr);
    ASSERT_TRUE(*x.data() == 0);
    ASSERT_TRUE(x.size() == 0);
    ASSERT_TRUE(x.is_static());
  }
  {
    jasl::string x = "one";
    ASSERT_TRUE(*x.data() == 'o');
    ASSERT_TRUE(x.size() == 3);
    ASSERT_TRUE(x.is_static());
  }
  {
    jasl::string x("");
    ASSERT_TRUE(*x.data() == 0);
    ASSERT_TRUE(x.size() == 0);
    ASSERT_TRUE(x.is_static());
  }
  {
    const char* onePtr = "one";
    jasl::string x(onePtr);
    ASSERT_TRUE(x.data() != nullptr);
    ASSERT_TRUE(x.data() != onePtr);
    ASSERT_TRUE(*x.data() == 'o');
    ASSERT_TRUE(x.size() == 3);
    ASSERT_FALSE(x.is_static());
    {
      jasl::string y(x);
      ASSERT_TRUE(y.data() != x.data());
      ASSERT_TRUE(y.size() == 3);
      ASSERT_FALSE(y.is_static());
    }
    {
      jasl::string y = x;
      ASSERT_TRUE(y.data() != x.data());
      ASSERT_TRUE(y.size() == 3);
      ASSERT_FALSE(y.is_static());
    }
    const char* twoPtr = "twoo";
    {
      jasl::string y("onestatic");
      ASSERT_TRUE(y.is_static());
      y = jasl::string(twoPtr);
      ASSERT_TRUE(*y.data() == 't');
      ASSERT_TRUE(y.size() == 4);
      ASSERT_FALSE(y.is_static());
      {
        y = jasl::string("static again");
        ASSERT_TRUE(*y.data() == 's');
        ASSERT_TRUE(y.size() == 12);
        ASSERT_TRUE(y.is_static());
      }
    }
    {
      jasl::string y("onestatic");
      ASSERT_TRUE(y.is_static());
      y.assign(twoPtr);
      ASSERT_TRUE(*y.data() == 't');
      ASSERT_TRUE(y.size() == 4);
      ASSERT_FALSE(y.is_static());
      {
        y = "static again";
        ASSERT_TRUE(*y.data() == 's');
        ASSERT_TRUE(y.size() == 12);
        ASSERT_TRUE(y.is_static());
      }
    }
  }
  {
    const char oneStr[] = "one";
    jasl::string x(oneStr);
    ASSERT_TRUE(x.data() == oneStr);
    ASSERT_TRUE(*x.data() == 'o');
    ASSERT_TRUE(x.size() == 3);
    ASSERT_TRUE(x.is_static());
    {
      jasl::string y(x);
      ASSERT_TRUE(y.data() == oneStr);
      ASSERT_TRUE(y.size() == 3);
      ASSERT_TRUE(y.is_static());
    }

    {
      jasl::string y = x;
      ASSERT_TRUE(y.data() == oneStr);
      ASSERT_TRUE(y.size() == 3);
      ASSERT_TRUE(y.is_static());
    }
    const char twoStr[] = "twoo";
    {
      jasl::string y(oneStr);
      y = jasl::string(twoStr);
      ASSERT_TRUE(y.data() == twoStr);
      ASSERT_TRUE(y.size() == 4);
      ASSERT_TRUE(y.is_static());
    }

    {
      jasl::string y(oneStr);
      y = twoStr;
      ASSERT_TRUE(y.data() == twoStr);
      ASSERT_TRUE(y.size() == 4);
      ASSERT_TRUE(y.is_static());
    }
  }
  {
    const char oneStr[] = {'o', 'n', 'e'};
    jasl::string x(oneStr);
    ASSERT_TRUE(x.data() == oneStr);
    ASSERT_TRUE(*x.data() == 'o');
    ASSERT_TRUE(x.size() == 3);
    ASSERT_TRUE(x.is_static());
    {
      jasl::string y(x);
      ASSERT_TRUE(y.data() == oneStr);
      ASSERT_TRUE(y.size() == 3);
      ASSERT_TRUE(y.is_static());
    }
    {
      jasl::string y = x;
      ASSERT_TRUE(y.data() == oneStr);
      ASSERT_TRUE(y.size() == 3);
      ASSERT_TRUE(y.is_static());
    }
    const char twoStr[] = {'t', 'w', 'o', 'o'};
    {
      jasl::string y(oneStr);
      y = jasl::string(twoStr);
      ASSERT_TRUE(y.data() == twoStr);
      ASSERT_TRUE(y.size() == 4);
      ASSERT_TRUE(y.is_static());
    }
    {
      jasl::string y(oneStr);
      y = twoStr;
      ASSERT_TRUE(y.data() == twoStr);
      ASSERT_TRUE(y.size() == 4);
      ASSERT_TRUE(y.is_static());
    }
  }
  {
    jasl::string x("a"), y("bb");
    x.swap(y);
    ASSERT_TRUE(*x.data() == 'b');
    ASSERT_TRUE(x.size() == 2);
    ASSERT_TRUE(*y.data() == 'a');
    ASSERT_TRUE(y.size() == 1);
    ASSERT_TRUE(y.is_static());
  }
  {
    jasl::string x("apble"), y;

    y = x.substr(2);
    ASSERT_TRUE(*y.data() == 'b');
    ASSERT_TRUE(y.size() == 3);
    ASSERT_TRUE(y.is_static());

    jasl::string::base_type z(x.substr(2, 1));
    ASSERT_TRUE(*z.data() == 'b');
    ASSERT_TRUE(z.size() == 1);
  }
  {
    const char* strPtr = "apble";
    jasl::string x(strPtr), y;

    y = x.substr(2);
    ASSERT_TRUE(*y.data() == 'b');
    ASSERT_TRUE(y.size() == 3);
    ASSERT_FALSE(y.is_static());

    jasl::string::base_type z(x.substr(2, 1));
    ASSERT_TRUE(*z.data() == 'b');
    ASSERT_TRUE(z.size() == 1);

    y = x.substr(x.size());
    ASSERT_TRUE(y.size() == 0);
    ASSERT_EXCEPTION(x.substr(x.size() + 1), std::out_of_range);
  }

  {
    jasl::string x("apble");
    [](jasl::string::base_type) {}(x);
    [](const jasl::string::base_type&) {}(x);
  }

  ASSERT_TRUE(jasl::string("") == jasl::string(""));
  ASSERT_FALSE(jasl::string("") != jasl::string(""));
  ASSERT_FALSE(jasl::string("") < jasl::string(""));
  ASSERT_TRUE(jasl::string("") <= jasl::string(""));
  ASSERT_FALSE(jasl::string("") > jasl::string(""));
  ASSERT_TRUE(jasl::string("") >= jasl::string(""));

  ASSERT_TRUE(jasl::string("a") == jasl::string("a"));
  ASSERT_FALSE(jasl::string("a") != jasl::string("a"));
  ASSERT_FALSE(jasl::string("a") < jasl::string("a"));
  ASSERT_TRUE(jasl::string("a") <= jasl::string("a"));
  ASSERT_FALSE(jasl::string("a") > jasl::string("a"));
  ASSERT_TRUE(jasl::string("a") >= jasl::string("a"));

  ASSERT_TRUE(jasl::string("ab") == jasl::string("ab"));
  ASSERT_FALSE(jasl::string("ab") != jasl::string("ab"));
  ASSERT_FALSE(jasl::string("ab") < jasl::string("ab"));
  ASSERT_TRUE(jasl::string("ab") <= jasl::string("ab"));
  ASSERT_FALSE(jasl::string("ab") > jasl::string("ab"));
  ASSERT_TRUE(jasl::string("ab") >= jasl::string("ab"));

  ASSERT_FALSE(jasl::string("a") == jasl::string("b"));
  ASSERT_TRUE(jasl::string("a") != jasl::string("b"));
  ASSERT_TRUE(jasl::string("a") < jasl::string("b"));
  ASSERT_TRUE(jasl::string("a") <= jasl::string("b"));
  ASSERT_FALSE(jasl::string("a") > jasl::string("b"));
  ASSERT_FALSE(jasl::string("a") >= jasl::string("b"));

  ASSERT_FALSE(jasl::string("a") == jasl::string("ab"));
  ASSERT_TRUE(jasl::string("a") != jasl::string("ab"));
  ASSERT_TRUE(jasl::string("a") < jasl::string("ab"));
  ASSERT_TRUE(jasl::string("a") <= jasl::string("ab"));
  ASSERT_FALSE(jasl::string("a") > jasl::string("ab"));
  ASSERT_FALSE(jasl::string("a") >= jasl::string("ab"));

  ASSERT_FALSE(jasl::string("ab") == jasl::string("a"));
  ASSERT_TRUE(jasl::string("ab") != jasl::string("a"));
  ASSERT_FALSE(jasl::string("ab") < jasl::string("a"));
  ASSERT_FALSE(jasl::string("ab") <= jasl::string("a"));
  ASSERT_TRUE(jasl::string("ab") > jasl::string("a"));
  ASSERT_TRUE(jasl::string("ab") >= jasl::string("a"));

  // test only syntax
  std::cout << jasl::string("");

  // test only syntax
  std::hash<jasl::string::base_type>{}(jasl::string(""));

  // test only hash
  std::set<jasl::string>().size();

  return 0;
}
