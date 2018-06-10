// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#pragma once

#include <functional>
#include <memory>
#include <string>

#include "jasl_common.hpp"
#include "jasl_feature_test_macro.hpp"
#include "jasl_string_view.hpp"

namespace jasl {

template <typename CharT,
          class Traits = std::char_traits<CharT>,
          class AllocatorT = std::allocator<CharT>>
class basic_string : public basic_string_view<CharT, Traits> {
 public:
  typedef basic_string_view<CharT, Traits> base_type;
  typedef AllocatorT allocator_type;

 private:
  typedef std::allocator_traits<AllocatorT> alloc_traits;
  constexpr static bool base_type_nothrow_constructible =
      std::is_nothrow_constructible<base_type, const CharT*, size_t>::value;
  // https://stackoverflow.com/questions/12332772/why-arent-container-move-assignment-operators-noexcept
  constexpr static bool assing_operator_rvalue =
      alloc_traits::propagate_on_container_move_assignment::value &&
      std::is_nothrow_move_assignable<AllocatorT>::value;

 private:
  static const CharT empty_string[1];
  allocator_type allocator;
  size_t capacity;

 private:
  struct ParamterWrapper {
    size_t size;
    const CharT* ptr;

// constexpr: http://en.cppreference.com/w/cpp/string/char_traits/length
// char_traits:
// https://docs.microsoft.com/en-us/cpp/visual-cpp-language-conformance
#ifdef _MSC_VER
#define JASL_TEMP_CONSTEXPR_FOR_PW
#else
#define JASL_TEMP_CONSTEXPR_FOR_PW constexpr
#endif
    JASL_TEMP_CONSTEXPR_FOR_PW ParamterWrapper(const CharT* p) noexcept
        : size(Traits::length(p)), ptr(p) {}
#undef JASL_TEMP_CONSTEXPR_FOR_PW
  };

  inline void init(const CharT* ptr, size_t size) {
    JASL_ASSERT(capacity == 0, "capacity == 0");
    size_t constructed = 0;
    std::unique_ptr<CharT, std::function<void(CharT*)>> begin(
        alloc_traits::allocate(allocator, size), [&](CharT * ptd) noexcept {
          if (!std::is_integral<CharT>::value) {
            for (; 0 < constructed; --constructed)
              alloc_traits::destroy(allocator, ptd + constructed - 1);
          }
          alloc_traits::deallocate(allocator, ptd, size);
        });
    if (std::is_integral<CharT>::value) {
      constructed = size;
    } else {
      for (; constructed < size; ++constructed)
        alloc_traits::construct(allocator, begin.get() + constructed);
    }
    base_type::operator=(base_type(begin.get(), size));
    capacity = size;
    auto raw_begin = begin.release();
    Traits::copy(raw_begin, ptr, size);
  }

  inline void dispose() noexcept {
    if (capacity > 0) {
      const auto begin = const_cast<CharT*>(base_type::data());
      if (!std::is_integral<CharT>::value) {
        auto end = begin + capacity;
        for (auto p = begin; p < end; ++p)
          alloc_traits::destroy(allocator, p);
      }
      alloc_traits::deallocate(allocator, begin, capacity);
      capacity = 0;
    }
  }

  basic_string(const base_type& other, const AllocatorT& alloc)
      : base_type(nullptr), allocator(alloc), capacity(0) {
    init(other.data(), other.size());
  }

 public:
  ~basic_string() {
    if (capacity > 0) {
      alloc_traits::deallocate(allocator, const_cast<CharT*>(base_type::data()),
                               capacity);
    }
  }

  basic_string() noexcept(
      base_type_nothrow_constructible&&
          std::is_nothrow_default_constructible<AllocatorT>::value)
      : basic_string(AllocatorT()) {}

  basic_string(const AllocatorT& a) noexcept(
      base_type_nothrow_constructible&&
          std::is_nothrow_copy_constructible<AllocatorT>::value)
      : base_type(empty_string, 0), allocator(a), capacity(0) {}

  basic_string(const ParamterWrapper& paramW,
               const AllocatorT& alloc = AllocatorT())
      : base_type(nullptr), allocator(alloc), capacity(0) {
    JASL_ASSERT(paramW.ptr != nullptr, "paramW != nullptr");
    init(paramW.ptr, paramW.size);
  }

  template <size_t N>
  basic_string(const CharT (&str)[N]) noexcept(
      base_type_nothrow_constructible&&
          std::is_nothrow_default_constructible<AllocatorT>::value)
      : base_type(str, str[N - 1] == 0 ? N - 1 : N), allocator(), capacity(0) {}

  template <size_t N>
  basic_string(const CharT (&str)[N], const AllocatorT& alloc) noexcept(
      base_type_nothrow_constructible&&
          std::is_nothrow_copy_constructible<AllocatorT>::value)
      : base_type(str, str[N - 1] == 0 ? N - 1 : N),
        allocator(alloc),
        capacity(0) {}

  basic_string(const basic_string& other)
      : basic_string(other,
                     alloc_traits::select_on_container_copy_construction(
                         other.allocator)) {}

  basic_string(const basic_string& other, const AllocatorT& alloc)
      : base_type(other), allocator(alloc), capacity(0) {
    if (other.capacity > 0) {
      init(other.data(), other.size());
    }
  }

  basic_string(basic_string&& other) noexcept(
      std::is_nothrow_move_constructible<base_type>::value&&
          std::is_nothrow_move_constructible<AllocatorT>::value)
      : base_type(std::move(other)),
        allocator(std::move(other.allocator)),
        capacity(other.capacity) {
    other.capacity = 0;
  }

  basic_string(basic_string&& other, const AllocatorT& alloc)
      : base_type(nullptr), allocator(alloc), capacity(0) {
    if (allocator == other.allocator) {
      base_type::operator=(base_type(other.data(), other.size()));
      std::swap(capacity, other.capacity);
    } else {
      init(other.data(), other.size());
    }
  }

  template <size_t N>
  basic_string& assign(const CharT (&str)[N]) noexcept {
    dispose();
    base_type::operator=(base_type(str, str[N - 1] == 0 ? N - 1 : N));
    return *this;
  }

  basic_string& assign(const ParamterWrapper& paramW) {
    dispose();
    init(paramW.ptr, paramW.size);
    return *this;
  }

  basic_string& assign(const basic_string& other) {
    if (this == &other) {
      return *this;
    }
    /*propagate_on_container_copy_assignment
     * true if the allocator of type A needs to be copied when the container
     * that uses it is copy-assigned. Note that if the allocators of the source
     * and the target containers do not compare equal, copy assignment has to
     * deallocate the target's memory using the old allocator and then allocate
     * it using the new allocator before copying the elements (and the
     * allocator).
     */
    dispose();
    // https://en.cppreference.com/w/cpp/concept/Allocator
    if (alloc_traits::propagate_on_container_copy_assignment::value ||
        allocator != other.allocator) {
      allocator = other.allocator;
    }
    if (other.capacity == 0) {
      base_type::operator=(other);
    } else {
      init(other.data(), other.size());
    }
    return *this;
  }

  basic_string& assign(basic_string&& other) noexcept(assing_operator_rvalue) {
    if (this == &other) {
      return *this;
    }
    /*propagate_on_container_move_assignment:
     * true if the allocator of type A needs to be moved when the container that
     * uses it is move-assigned. If this member is false and the allocators of
     * the source and the target containers do not compare equal, move
     * assignment cannot take ownership of the source memory and must
     * move-assign or move-construct the elements individually, resizing its own
     * memory as needed.
     */
    dispose();
    if (alloc_traits::propagate_on_container_move_assignment::value) {
      allocator = std::move(other.allocator);
    } else if (allocator != other.allocator) {
      init(other.data(), other.size());
      return *this;
    }
    std::swap<base_type>(*this, other);
    std::swap(capacity, other.capacity);
    return *this;
  }

  template <size_t N>
  basic_string& operator=(const CharT (&str)[N]) noexcept(
      std::is_nothrow_assignable<base_type, base_type>::value) {
    return assign<N>(str);
  }

  basic_string& operator=(const basic_string& other) { return assign(other); }

  basic_string& operator=(basic_string&& other) noexcept(
      assing_operator_rvalue) {
    return assign(std::move(other));
  }

  constexpr bool is_static() const noexcept { return capacity == 0; }

  void swap(basic_string& other) noexcept(
      alloc_traits::propagate_on_container_swap::value) {
    /*propagate_on_container_swap
     * true if the allocators of type A need to be swapped when two containers
     * that use them are swapped. If this member is false and the allocators of
     * the two containers do not compare equal, the behavior of container swap
     * is undefined.
     */
    if (alloc_traits::propagate_on_container_swap::value) {
      std::swap(allocator, other.allocator);
    } else if (allocator != other.allocator) {
      JASL_ASSERT(false, "Undefined behaviour");
      std::terminate();
    }
    std::swap<base_type>(*this, other);
    std::swap(capacity, other.capacity);
  }

  basic_string substr(typename base_type::size_type pos) const {
    if (capacity == 0) {
      basic_string cpy(*this, allocator);
      cpy.base_type::operator=(base_type::substr(pos));
      return cpy;
    } else {
      return basic_string(base_type::substr(pos), allocator);
    }
  }

  constexpr base_type substr(typename base_type::size_type pos,
                             typename base_type::size_type count) const {
    return base_type::substr(pos, count);
  }
};

template <typename CharT, typename Traits, typename Allocator>
const CharT basic_string<CharT, Traits, Allocator>::empty_string[1] = {0};

typedef basic_string<char> string;
typedef basic_string<wchar_t> wstring;
typedef basic_string<char16_t> u16string;
typedef basic_string<char32_t> u32string;

}  // namespace jasl
