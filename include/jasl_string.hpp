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

template <typename CharT, typename Traits = std::char_traits<CharT>,
          typename AllocatorT = std::allocator<CharT>>
class basic_string : public basic_string_view<CharT, Traits> {
 public:
  typedef basic_string_view<CharT, Traits> base_type;
  typedef AllocatorT allocator_type;

 private:
  typedef std::allocator_traits<AllocatorT> alloc_traits;
  constexpr static bool is_nothrow_constructible_base_type =
      std::is_nothrow_constructible<base_type, const CharT*, size_t>::value;
  // https://stackoverflow.com/questions/12332772/why-arent-container-move-assignment-operators-noexcept
  constexpr static bool is_noexcept_assign_rvalue =
      alloc_traits::propagate_on_container_move_assignment::value &&
      std::is_nothrow_move_assignable<AllocatorT>::value &&
      noexcept(std::declval<base_type&>().swap(std::declval<base_type&>()));

 private:
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
    static_assert(std::is_integral<CharT>::value,
                  "Unsupported CharT. Need construct and destruct.");
    if (size == 0) {
      return;
    }
    size_t constructed = 0;
    std::unique_ptr<CharT, std::function<void(CharT*)>> begin(
        alloc_traits::allocate(allocator, size), [&](CharT * ptd) noexcept {
          alloc_traits::deallocate(allocator, ptd, size);
        });
    constructed = size;
    base_type::operator=(base_type(begin.get(), size));
    capacity = size;
    auto raw_begin = begin.release();
    Traits::copy(raw_begin, ptr, size);
  }

  inline void dispose() noexcept {
    if (capacity > 0) {
      const auto begin = const_cast<CharT*>(base_type::data());
      alloc_traits::deallocate(allocator, begin, capacity);
      capacity = 0;
    }
  }

  basic_string(const base_type& other, const AllocatorT& alloc)
      : base_type(), allocator(alloc), capacity(0) {
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
      is_nothrow_constructible_base_type&&
          std::is_nothrow_default_constructible<AllocatorT>::value)
      : basic_string(AllocatorT()) {}

  basic_string(const AllocatorT& a) noexcept(
      is_nothrow_constructible_base_type&&
          std::is_nothrow_copy_constructible<AllocatorT>::value)
      : base_type(), allocator(a), capacity(0) {}

  basic_string(const ParamterWrapper& paramW,
               const AllocatorT& alloc = AllocatorT())
      : base_type(), allocator(alloc), capacity(0) {
    JASL_ASSERT(paramW.ptr != nullptr, "paramW != nullptr");
    init(paramW.ptr, paramW.size);
  }

  template <size_t N>
  basic_string(const CharT (&str)[N]) noexcept(
      is_nothrow_constructible_base_type&&
          std::is_nothrow_default_constructible<AllocatorT>::value)
      : base_type(str, str[N - 1] == 0 ? N - 1 : N), allocator(), capacity(0) {}

  template <size_t N>
  basic_string(const CharT (&str)[N], const AllocatorT& alloc) noexcept(
      is_nothrow_constructible_base_type&&
          std::is_nothrow_copy_constructible<AllocatorT>::value)
      : base_type(str, str[N - 1] == 0 ? N - 1 : N),
        allocator(alloc),
        capacity(0) {}

  basic_string(const basic_string& other)
      : basic_string(other, alloc_traits::select_on_container_copy_construction(
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

  basic_string& assign(basic_string&& other) noexcept(
      is_noexcept_assign_rvalue) {
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
    base_type::swap(other);
    std::swap(capacity, other.capacity);
    return *this;
  }

  template <size_t N>
  basic_string& operator=(const CharT (&str)[N]) noexcept {
    return assign<N>(str);
  }

  basic_string& operator=(const basic_string& other) { return assign(other); }

  basic_string& operator=(basic_string&& other) noexcept(
      is_noexcept_assign_rvalue) {
    return assign(std::move(other));
  }

  constexpr bool is_static() const noexcept { return capacity == 0; }

  void swap(basic_string& other) noexcept(
      (!alloc_traits::propagate_on_container_swap::value ||
       JASL_is_nothrow_swappable_value(AllocatorT)) &&
      noexcept(std::declval<base_type>().swap(other))) {
    using std::swap;
    /*propagate_on_container_swap
     * true if the allocators of type A need to be swapped when two containers
     * that use them are swapped. If this member is false and the allocators of
     * the two containers do not compare equal, the behavior of container swap
     * is undefined.
     */
    if (alloc_traits::propagate_on_container_swap::value) {
      swap(allocator, other.allocator);
    } else if (allocator != other.allocator) {
      JASL_ASSERT(false, "Undefined behaviour");
      std::terminate();
    }
    base_type::swap(other);
    swap(capacity, other.capacity);
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

template <typename CharT, typename Traits>
void swap(basic_string<CharT, Traits>& lhs,
          basic_string<CharT, Traits>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}

typedef basic_string<char> string;
typedef basic_string<wchar_t> wstring;
typedef basic_string<char16_t> u16string;
typedef basic_string<char32_t> u32string;

}  // namespace jasl
