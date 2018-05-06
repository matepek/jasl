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

#include "jasl_static_string.hpp"
#include "jasl_string.hpp"

constexpr static size_t ItemCount = 100000;
constexpr static size_t IterCount = 5;

template <typename ItemType,
          size_t ItemCountT = ItemCount,
          size_t IterCountT = IterCount>
std::pair<std::string, double> measure(
    std::string description,
    const std::function<void(ItemType*)>& construct,
    const std::function<void(ItemType&)>& destruct,
    const std::function<void(ItemType&)>& doIt = nullptr) {
  std::chrono::duration<double> durSum;
  for (size_t iter = 0; iter < IterCountT; ++iter) {
    std::unique_ptr<char[]> array(new char[ItemCountT * sizeof(ItemType)]);
    // it has to sleep enough time to eliminate other effects as much as
    // reasonable.
    std::this_thread::sleep_for(std::chrono::seconds(10));
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
  auto elapsed = (durSum / ItemCountT / IterCountT).count();
  return std::pair<std::string, double>(std::move(description), elapsed);
}

void print_compare(const std::pair<std::string, double>& left,
                   const std::pair<std::string, double>& right) {
  std::cout << left.first << ": " << left.second << " / unit" << std::endl
            << right.first << ": " << right.second << " / unit" << std::endl
            << right.first << " / " << left.first << " = "
            << static_cast<int>(100 * right.second / left.second) << " %"
            << std::endl;
}

int main() {
  std::cout << "This performance test compares construction and destruction of"
            << std::endl
            << "the jasl::static_string and the jasl::string to std::string."
            << std::endl
            << "NOTE: Base type of the jasl::static_string and jasl::string is "
#ifdef JASL_USE_JASL_STRING_VIEW_AS_BASE
            << "*jasl::string_view*."
#else
            << "*std::string_view*."
#endif
            << std::endl
            << std::endl
            << "ATTENTION: Are you sure you don't run any resourceful "
               "application right now?!"
            << std::endl
            << "(I know its tempting to read xkcd.org while you are waiting "
               "for the result, "
            << std::endl
            << "but you should preolad your articles before you measure.)"
            << std::endl
            << std::endl;
#ifdef JASL_DEBUG
  std::cout << "!!! WARNING !!! This is not an optimized build!" << std::endl
            << std::endl;
#endif

  auto small_jasl_static_string = measure<jasl::static_string>(
      "small jasl::static_string",
      [](jasl::static_string* ptr) { new (ptr) jasl::static_string("small"); },
      [](jasl::static_string& str) {
        using namespace jasl;
        str.~static_string();
      });

  auto small_jasl_string_static = measure<jasl::string>(
      "small jasl::string with static",
      [](jasl::string* ptr) { new (ptr) jasl::string("small"); },
      [](jasl::string& str) {
        using namespace jasl;
        str.~string();
      });

  auto small_std_string = measure<std::string>(
      "small std::string",
      [](std::string* ptr) { new (ptr) std::string("small"); },
      [](std::string& str) {
        using namespace std;
        str.~string();
      });

  print_compare(small_jasl_static_string, small_std_string);
  print_compare(small_jasl_string_static, small_std_string);

  auto long_jasl_static_string = measure<jasl::static_string>(
      "long jasl::static_string",
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

  auto long_jasl_string_static = measure<jasl::string>(
      "long jasl::string static",
      [](jasl::string* ptr) {
        new (ptr) jasl::string(
            "n4iwohgoiuwhvjknvirwthbuihvnjkrwhtuinvkjrwnviuwphjewh"
            "gvuiwnvw giuwrtgj itwrj vjern ijowthgiu whg "
            "uipwvjprwg piuwrhtg puiwrfj prin 4irufjwe "
            "iupnwiuptg");
      },
      [](jasl::string& str) {
        using namespace jasl;
        str.~string();
      });

  auto long_std_string = measure<std::string>(
      "long std::string",
      [](std::string* ptr) {
        new (ptr) std::string(
            "bigger than std::string's static storage, which is "
            "usually 16 character");
      },
      [](std::string& str) {
        using namespace std;
        str.~string();
      });

  print_compare(long_jasl_static_string, long_std_string);
  print_compare(long_jasl_string_static, long_std_string);

  return 0;
}
