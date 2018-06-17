// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#include <set>

#include "jasl_string.hpp"
#include "test_helper.hpp"

// struct CustomAllocator {
//  public:
//   using namespace std;
//   typedef size_t size_type;
//   typedef ptrdiff_t difference_type;
//   typedef char* pointer;
//   typedef const char* const_pointer;
//   typedef char& reference;
//   typedef const char& const_reference;
//   typedef char value_type;

//   typedef true_type propagate_on_container_move_assignment;

//   CustomAllocator() noexcept {}
//   template <class _Up>
//   CustomAllocator(const CustomAllocator&) noexcept {}
//   pointer address(reference __x) const noexcept {
//     return _VSTD::addressof(__x);
//   }
//   const_pointer address(const_reference __x) const noexcept {
//     return _VSTD::addressof(__x);
//   }
//   pointer allocate(size_type __n, CustomAllocator::const_pointer = 0) {
//     return static_cast<pointer>(new char[__n * sizeof(char)]);
//   }
//   void deallocate(pointer __p, size_type) noexcept { delete __p; }
//   size_type max_size() const noexcept { return size_type(~0) / sizeof(char);
//   } template <class _Up, class... _Args> void construct(_Up* __p, _Args&&...
//   __args) {
//     ::new ((void*)__p) _Up(_VSTD::forward<_Args>(__args)...);
//   }

//   void destroy(pointer __p) {}
// };

int main(int, char* argv[]) {
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
      jasl::string y(twoPtr);
      ASSERT_FALSE(y.is_static());
      y = x;
      ASSERT_FALSE(y.is_static());
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

  // typedef jasl::basic_string<char, std::char_traits<char>, CustomAllocator>
  //     cus_string;
  // {
  //   CustomAllocator a1, a2;
  //   cus_string s1("a", a1);
  //   cus_string s2("b", a2);
  //   // s1.assign(s2);
  // }

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
  ASSERT_TRUE(std::set<jasl::string>().size() == 0);

  // #ifdef JASL_cpp_lib_string_view
  //   {
  //     std::string_view s("apple");
  //     jasl::string js(s);
  //     ASSERT_TRUE(js.data() != nullptr);
  //     ASSERT_TRUE(*js.data() == 'a');
  //     ASSERT_TRUE(js.size() == 5);

  //     std::string s2(js);
  //     ASSERT_TRUE(s2 == "apple");

  //     s = "peachh";
  //     js = s;
  //     ASSERT_TRUE(js.data() != nullptr);
  //     ASSERT_TRUE(*js.data() == 'p');
  //     ASSERT_TRUE(js.size() == 6);

  //     s2 = js;
  //     ASSERT_TRUE(s2 == "peachh");
  //   }
  // #endif
  //   {
  //     std::string s("apple");
  //     jasl::string js(s);
  //     ASSERT_TRUE(js.data() != nullptr);
  //     ASSERT_TRUE(*js.data() == 'a');
  //     ASSERT_TRUE(js.size() == 5);

  //     std::string s2(js);
  //     ASSERT_TRUE(s2 == "apple");

  //     s = "peachh";
  //     js = s;
  //     ASSERT_TRUE(js.data() != nullptr);
  //     ASSERT_TRUE(*js.data() == 'p');
  //     ASSERT_TRUE(js.size() == 6);

  //     s2 = js;
  //     ASSERT_TRUE(s2 == "peachh");
  //   }
  return 0;
}
