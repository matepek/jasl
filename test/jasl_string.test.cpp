// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#include <set>

#include "jasl/string.hpp"
#include "test_helper.hpp"

struct different_alloc : public std::allocator<char> {
  typedef std::false_type propagate_on_container_move_assignment;
};
bool operator==(const different_alloc&, const different_alloc&) {
  return false;
}
bool operator!=(const different_alloc&, const different_alloc&) {
  return true;
}

typedef jasl::basic_string<char, std::char_traits<char>, different_alloc>
    jasl_diffalloc_string;

int main(int, char* argv[]) {
  std::allocator<char> alloc1, alloc2;
  {
    // pretty sure it isn't constexpr
    jasl::string x(argv[0]);
    ASSERT_TRUE(x.data() != nullptr);
    x.assign(x);
    x.assign(std::move(x));
  }
  {
    jasl::string x;
    ASSERT_TRUE(x.data() == nullptr);
    ASSERT_TRUE(x.size() == 0);
    ASSERT_TRUE(x.is_static());
  }
  {
    jasl::string x = "one";
    ASSERT_TRUE(*x.data() == 'o');
    ASSERT_TRUE(x.size() == 3);
    ASSERT_TRUE(x.is_static());
    {
      jasl::string y = "two";
      y = x;
    }
  }
  {
    jasl::string x("");
    ASSERT_TRUE(*x.data() == 0);
    ASSERT_TRUE(x.size() == 0);
    ASSERT_TRUE(x.is_static());
  }
  {
    jasl::string x("apple", 3);
    ASSERT_TRUE(x.size() == 3);
    ASSERT_TRUE(*x.data() == 'a');
    ASSERT_FALSE(x.is_static());
  }
  {
    const char* onePtr = "one";
    const jasl::string x(onePtr);
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
    {
      jasl::string y = x;
      jasl::string z = std::move(y);
      ASSERT_TRUE(z.data() != x.data());
      ASSERT_TRUE(z.size() == 3);
      ASSERT_FALSE(z.is_static());
    }
    {
      jasl::string y = x;
      jasl::string z(std::move(y), alloc1);
      ASSERT_TRUE(z.data() != x.data());
      ASSERT_TRUE(z.size() == 3);
      ASSERT_FALSE(z.is_static());
    }
    /*{
      jasl_diffalloc_string y(jasl_diffalloc_string("apple"),
                              different_alloc());
      ASSERT_TRUE(y.size() == 5);
      ASSERT_FALSE(y.is_static());
      jasl_diffalloc_string z;
      z.assign(y);
      z.assign(jasl_diffalloc_string());
    }*/
    const char* twoPtr = "twoo";
    {
      jasl::string y(twoPtr);
      ASSERT_FALSE(y.is_static());
      y = x;
      ASSERT_FALSE(y.is_static());
      const char* emptyPtr = "";
      y.assign(emptyPtr);
      ASSERT_TRUE(y.size() == 0);
    }
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
    const jasl::string x(oneStr);
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
    {
      jasl::string y = x;
      jasl::string z = std::move(y);
      ASSERT_TRUE(z.data() == x.data());
      ASSERT_TRUE(z.size() == 3);
      ASSERT_TRUE(z.is_static());
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
    const jasl::string x(oneStr);
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
    ASSERT_TRUE(x.size() == 2);
    ASSERT_TRUE(*x.data() == 'b');
    ASSERT_TRUE(x.is_static());
    ASSERT_TRUE(y.size() == 1);
    ASSERT_TRUE(*y.data() == 'a');
    ASSERT_TRUE(y.is_static());
    swap(x, y);
    ASSERT_TRUE(y.size() == 2);
    ASSERT_TRUE(*y.data() == 'b');
    ASSERT_TRUE(y.is_static());
    ASSERT_TRUE(x.size() == 1);
    ASSERT_TRUE(*x.data() == 'a');
    ASSERT_TRUE(x.is_static());
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
  }

  {
    jasl::static_string ss("apble");
    jasl::string js(ss);
    ASSERT_TRUE(js.is_static());
    ASSERT_TRUE(js.size() == 5);
    ASSERT_TRUE(*js.data() == 'a');
  }
  {
    jasl::static_string ss("apble");
    jasl::string js("else");
    js = ss;
    ASSERT_TRUE(js.is_static());
    ASSERT_TRUE(js.size() == 5);
    ASSERT_TRUE(*js.data() == 'a');
  }

  {
    jasl::string x("apble");
    [](jasl::string::base_type) {}(x);
    [](const jasl::string::base_type&) {}(x);
  }

  {
    std::allocator<char> a1, a2;
    jasl::string s1("a", a1), s2("b", a2);
    s1.assign(s2);
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
  unused_variable(std::hash<jasl::string::base_type>{}(jasl::string("")));

  // test only hash
  ASSERT_TRUE(std::set<jasl::string>().size() == 0);

#if defined(JASL_SUPPORT_STD_TO_JASL)
#if defined(JASL_cpp_lib_string_view)
  {
    std::string_view ssv("apple", 5);
    jasl::string js(ssv);
    ASSERT_TRUE(js.size() == 5);
    ASSERT_TRUE(*js.data() == 'a');
  }
  {
    std::string_view ssv("apple", 5);
    jasl::string js;
    js = ssv;
    ASSERT_TRUE(js.size() == 5);
    ASSERT_TRUE(*js.data() == 'a');
  }
#endif
  {
    std::string ss("apple");
    jasl::string js(ss);
    ASSERT_TRUE(js.size() == 5);
    ASSERT_TRUE(*js.data() == 'a');
  }
  {
    std::string ss("apple", 5);
    jasl::string js;
    js = ss;
    ASSERT_TRUE(js.size() == 5);
    ASSERT_TRUE(*js.data() == 'a');
  }
#endif

#if defined(JASL_SUPPORT_JASL_TO_STD)
#if defined(JASL_cpp_lib_string_view)
  {
    jasl::string js("apple");
    std::string_view ssv(js);
    ASSERT_TRUE(ssv.size() == 5);
    ASSERT_TRUE(*ssv.data() == 'a');
  }
  {
    jasl::string js("apple", 5);
    std::string_view ssv;
    ssv = js;
    ASSERT_TRUE(ssv.size() == 5);
    ASSERT_TRUE(*ssv.data() == 'a');
  }
#endif
  {
    jasl::string js("apple");
    std::string ss(js);
    ASSERT_TRUE(ss.size() == 5);
    ASSERT_TRUE(*ss.data() == 'a');
  }
  {
    jasl::string js("apple");
    std::string ss;
    ss = js;
    ASSERT_TRUE(ss.size() == 5);
    ASSERT_TRUE(*ss.data() == 'a');
  }
#endif

  return 0;
}
