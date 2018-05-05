// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#include <chrono>
#include <functional>
#include <iostream>
#include <new>
#include <string>
#include <thread>

//#include "jasl_string_view.hpp"
//#define JASL_STRING_VIEW_TYPE jasl::basic_jasl_string_view
#include "jasl_static_string.hpp"

constexpr static size_t ItemCount = 100000;
constexpr static size_t IterCount = 1;

template <typename ItemType,
          size_t ItemCountT = ItemCount,
          size_t IterCountT = IterCount>
void measure(const char* description,
             const std::function<void(ItemType*)>& construct,
             const std::function<void(ItemType&)>& destruct,
             const std::function<void(ItemType&)>& doIt = nullptr) {
  std::chrono::duration<double> durSum;
  for (size_t iter = 0; iter < IterCountT; ++iter) {
    std::unique_ptr<char[]> array(new char[ItemCountT * sizeof(ItemType)]);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ItemCountT; ++i) {
      construct(reinterpret_cast<ItemType*>(&array[i * sizeof(ItemType)]));
    }
    if (doIt != nullptr) {
      for (size_t i = 0; i < ItemCountT; ++i) {
        doIt(reinterpret_cast<ItemType&>(array[i * sizeof(ItemType)]));
      }
    }
    for (size_t i = 0; i < ItemCountT; ++i) {
      destruct(reinterpret_cast<ItemType&>(array[i * sizeof(ItemType)]));
    }
    auto stop = std::chrono::high_resolution_clock::now();
    durSum += std::chrono::duration<double>(stop - start);
  }
  std::cout << description << (durSum / ItemCountT / IterCountT).count()
            << std::endl;
}

int main() {
  // some timing problem still there
  measure<jasl::static_string>(
      "small static_string: ",
      [](jasl::static_string* ptr) { new (ptr) jasl::static_string("small"); },
      [](jasl::static_string& str) {
        using namespace jasl;
        str.~static_string();
      });

  measure<jasl::static_string>(
      "long static_string: ",
      [](jasl::static_string* ptr) {
        new (ptr) jasl::static_string(
            "n4iwohgoiuwhvjknvirwthbuihvnjkrwhtuinvkjrwnviuwphjewh"
            "gvuiwnvw giuwrtgj itwrj vjern ijowthgiu whg "
            "uipwvjprwg piuwrhtg puiwrfj prin 4irufjwe "
            "iupnwiuptg");
      },
      [](jasl::static_string& str) {
        using namespace jasl;
        str.~static_string();
      });
  measure<std::string>("small std::string: ",
                       [](std::string* ptr) { new (ptr) std::string("small"); },
                       [](std::string& str) {
                         using namespace std;
                         str.~string();
                       });

  measure<std::string>(
      "long std::string: ",
      [](std::string* ptr) {
        new (ptr) std::string(
            "bigger than std::string's static storage, which is "
            "usually 16 character");
      },
      [](std::string& str) {
        using namespace std;
        str.~string();
      });
  return 0;
}
