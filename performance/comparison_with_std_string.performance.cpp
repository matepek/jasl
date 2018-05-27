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

constexpr static char short_string_array[] = "short";
constexpr static char long_string_array[] =
    "n4iwohgoiuwhvjknvirwthbuihvnjkrwhtuinvkjrwnviuwphjewh"
    "gvuiwnvw giuwrtgj itwrj vjern ijowthgiu whg "
    "uipwvjprwg piuwrhtg puiwrfj prin 4irufjwe "
    "iupnwiuptg";

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
  std::cout << " - " << right.first << " / " << left.first << " = "
            << static_cast<int>(100 * right.second / left.second) << " %"
            << std::endl
            << "   - " << left.first << ": " << left.second << " sec / unit"
            << std::endl
            << "   - " << right.first << ": " << right.second << " sec / unit"
            << std::endl
            << std::endl;
}

int main(int, char* argv[]) {
  auto& c = std::cout;
  using std::endl;
  c << "**ATTENTION**: Are you sure you don't run any resourceful "
       "application right now?!"
    << endl
    << "(I know it's tempting to read xkcd.org while you are waiting "
       "for the result, "
    << endl
    << "but you should preolad your articles before you measure.)" << endl
    << " - Is you notebook connected to power source?" << endl
#ifdef JASL_DEBUG
    << "**!!! WARNING !!!** This is not an optimized build!" << endl
#endif
    << endl
    << endl;

  c << "##### " << argv[0] << endl
    << endl
    << "JASL version: " << JASL_VERSION_STR << endl
    << endl
    << "This performance test compares construction and destruction of" << endl
    << "the jasl::static_string and the [jasl::string] to [std::string]."
    << endl
    << endl
    << "Parameters:" << endl
    << " - ( ItemCount, IterCount ) := ( " << ItemCount << ", " << IterCount
    << " )" << endl
    << " - ( sizeof(short_string_array), sizeof(long_string_array) ) := ( "
    << sizeof(short_string_array) << ", " << sizeof(long_string_array) << " )"
    << endl
    << endl
    << "   **Remarks**: The results depend on the size of the character array "
       "given to the std::string's constructor."
    << endl
    << endl
    << "Hardware:" << endl
    << " - Processor (type, frequency): TODO" << endl
    << " - Memory: TODO" << endl
    << " - Operating System: TODO" << endl
    << endl
    << "**NOTE**: Base type of the [jasl::static_string] and [jasl::string] is "
#ifdef JASL_USE_JASL_STRING_VIEW_AS_BASE
    << "[jasl::string_view]."
#else
    << "[std::string_view]."
#endif
    << endl
    << endl;

  auto short_jasl_static_string = measure<jasl::static_string>(
      "short [jasl::static_string]",
      [](jasl::static_string* ptr) {
        new (ptr) jasl::static_string(short_string_array);
      },
      [](jasl::static_string& str) {
        using namespace jasl;
        str.~static_string();
      });

  auto short_jasl_string_static = measure<jasl::string>(
      "short static [jasl::string]",
      [](jasl::string* ptr) {
        static_assert(sizeof(short_string_array) < 8, "Err!");
        new (ptr) jasl::string(short_string_array);
      },
      [](jasl::string& str) {
        using namespace jasl;
        str.~string();
      });

  auto short_std_string = measure<std::string>(
      "short [std::string]",
      [](std::string* ptr) { new (ptr) std::string(short_string_array); },
      [](std::string& str) {
        using namespace std;
        str.~string();
      });

  print_compare(short_jasl_static_string, short_std_string);
  print_compare(short_jasl_string_static, short_std_string);

  auto long_jasl_static_string = measure<jasl::static_string>(
      "long [jasl::static_string]",
      [](jasl::static_string* ptr) {
        new (ptr) jasl::static_string(long_string_array);
      },
      [](jasl::static_string& str) {
        using namespace jasl;
        str.~static_string();
      });

  auto long_jasl_string_static = measure<jasl::string>(
      "long static [jasl::string]",
      [](jasl::string* ptr) { new (ptr) jasl::string(long_string_array); },
      [](jasl::string& str) {
        using namespace jasl;
        str.~string();
      });

  auto long_jasl_string_dynamic = measure<jasl::string>(
      "long dynamic [jasl::string]",
      [](jasl::string* ptr) {
        new (ptr) jasl::string(static_cast<const char*>(long_string_array));
      },
      [](jasl::string& str) {
        using namespace jasl;
        str.~string();
      });

  auto long_std_string = measure<std::string>(
      "long [std::string]",
      [](std::string* ptr) {
        static_assert(sizeof(long_string_array) > 16, "Err!");
        new (ptr) std::string(long_string_array);
      },
      [](std::string& str) {
        using namespace std;
        str.~string();
      });

  print_compare(long_jasl_static_string, long_std_string);
  print_compare(long_jasl_string_static, long_std_string);
  print_compare(long_jasl_string_dynamic, long_std_string);

  return 0;
}
