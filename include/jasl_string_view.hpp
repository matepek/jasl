// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#pragma once

#include "jasl_common.hpp"

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace jasl {

template <typename CharT, typename Traits = std::char_traits<CharT> /**/>
class basic_string_view {
  const CharT* _ptr;
  std::size_t _size;

 public:
  // types
  typedef Traits traits_type;
  typedef CharT value_type;
  typedef CharT* pointer;
  typedef const CharT* const_pointer;
  typedef CharT& reference;
  typedef const CharT& const_reference;
  typedef const_pointer const_iterator;
  typedef const_iterator iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef const_reverse_iterator reverse_iterator;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  static constexpr const size_type npos = static_cast<size_type>(-1);

 public:
  constexpr basic_string_view() noexcept : _ptr(nullptr), _size(0) {}

  constexpr basic_string_view(const CharT* ptr, size_type size) noexcept
      : _ptr(ptr), _size(size) {}

  constexpr basic_string_view(const basic_string_view& other) noexcept =
      default;

  JASL_CONSTEXPR_FROM_14 basic_string_view& operator=(
      const basic_string_view& other) noexcept = default;

  ~basic_string_view() noexcept = default;

  constexpr const_iterator begin() const noexcept { return cbegin(); }
  constexpr const_iterator end() const noexcept { return cend(); }
  constexpr const_iterator cbegin() const noexcept { return this->_ptr; }
  constexpr const_iterator cend() const noexcept {
    return this->_ptr + this->_size;
  }
  constexpr const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(cend());
  }
  constexpr const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(cbegin());
  }
  constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }
  constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(cbegin());
  }

  constexpr bool empty() const noexcept { return this->_size == 0; }
  constexpr const CharT* data() const noexcept { return this->_ptr; }
  constexpr size_type size() const noexcept { return this->_size; }
  constexpr size_type length() const noexcept { return this->_size; }

  constexpr const_reference operator[](size_type pos) const {
    return this->_ptr[pos];
  }

  JASL_CONSTEXPR_FROM_14 const_reference at(size_type pos) const {
    if (pos >= this->size())
      JASL_THROW(std::out_of_range("string_view::substr"));
    return this->_ptr[pos];
  }

  constexpr const_reference front() const noexcept {
    JASL_ASSERT(!this->empty(), "string_view::front(): string is empty");
    return this->_ptr[0];
  }

  constexpr const_reference back() const noexcept {
    JASL_ASSERT(!this->empty(), "string_view::back(): string is empty");
    return this->_ptr[this->_size - 1];
  }

  JASL_CONSTEXPR_FROM_14 void swap(basic_string_view& other) noexcept {
    const value_type* p = this->_ptr;
    this->_ptr = other._ptr;
    other._ptr = p;

    size_type s = this->_size;
    this->_size = other._size;
    other._size = s;
  }

  size_type copy(CharT* s, size_type n, size_type pos = 0) const {
    if (pos > this->size())
      JASL_THROW(std::out_of_range("string_view::copy"));
    size_type rlen = std::min(n, this->size() - pos);
    Traits::copy(s, this->data() + pos, rlen);
    return rlen;
  }

  JASL_CONSTEXPR_FROM_14 basic_string_view
  substr(size_type pos, size_type count = npos) const {
    if (pos > this->size())
      JASL_THROW(std::out_of_range("basic_string_view::substr"));
    size_type rlen = std::min(count, this->size() - pos);
    return basic_string_view(data() + pos, rlen);
  }

  JASL_CONSTEXPR_FROM_14 void remove_prefix(size_type n) noexcept {
    JASL_ASSERT(n <= size(), "remove_prefix() can't remove more than size()");
    this->_ptr += n;
    this->_size -= n;
  }

  JASL_CONSTEXPR_FROM_14 void remove_suffix(size_type n) noexcept {
    JASL_ASSERT(n <= size(), "remove_suffix() can't remove more than size()");
    this->_size -= n;
  }

  JASL_CONSTEXPR_FROM_14 int compare(const basic_string_view& right) const
      noexcept {
    size_type rlen = std::min(size(), right.size());
    int retval = Traits::compare(data(), right.data(), rlen);
    if (retval == 0)  // first rlen chars matched
      retval = size() == right.size() ? 0 : (size() < right.size() ? -1 : 1);
    return retval;
  }

  JASL_CONSTEXPR_FROM_14
  size_type find(const basic_string_view& s) const noexcept {
    JASL_ASSERT(s.size() == 0 || s.data() != nullptr,
                "string_view::find(): received nullptr");
    auto itWhere = this->cbegin();
    auto itWhat = s.cbegin();
    while (itWhere != this->cend() && itWhat != s.cend()) {
      if (*itWhere != *itWhat) {
        ++itWhere;
      } else {
        auto match = itWhere++;
        ++itWhat;
        while (itWhere != this->cend() && itWhat != s.cend() &&
               *itWhere == *itWhat) {
          ++itWhere;
          ++itWhat;
        }
        if (itWhat == s.cend()) {
          return static_cast<size_type>(match - this->cbegin());
        }
        itWhat = s.cbegin();
      }
    }
    return npos;
  }
};  // namespace jasl

// http://en.cppreference.com/w/cpp/algorithm/lexicographical_compare

template <typename CharT, typename Traits>
JASL_CONSTEXPR_FROM_14 bool operator==(
    const basic_string_view<CharT, Traits>& left,
    const basic_string_view<CharT, Traits>& right) {
  if (left.size() != right.size()) {
    return false;
  }
  return left.compare(right) == 0;
}

template <typename CharT, typename Traits>
JASL_CONSTEXPR_FROM_14 bool operator!=(
    const basic_string_view<CharT, Traits>& left,
    const basic_string_view<CharT, Traits>& right) {
  return left.compare(right) != 0;
}

template <typename CharT, typename Traits>
JASL_CONSTEXPR_FROM_14 bool operator<(
    const basic_string_view<CharT, Traits>& left,
    const basic_string_view<CharT, Traits>& right) {
  return left.compare(right) < 0;
}

template <typename CharT, typename Traits>
JASL_CONSTEXPR_FROM_14 bool operator<=(
    const basic_string_view<CharT, Traits>& left,
    const basic_string_view<CharT, Traits>& right) {
  return left.compare(right) <= 0;
}

template <typename CharT, typename Traits>
JASL_CONSTEXPR_FROM_14 bool operator>(
    const basic_string_view<CharT, Traits>& left,
    const basic_string_view<CharT, Traits>& right) {
  return left.compare(right) > 0;
}

template <typename CharT, typename Traits>
JASL_CONSTEXPR_FROM_14 bool operator>=(
    const basic_string_view<CharT, Traits>& left,
    const basic_string_view<CharT, Traits>& right) {
  return left.compare(right) >= 0;
}

template <class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<<(
    std::basic_ostream<CharT, Traits>& os,
    basic_string_view<CharT, Traits> v) {
  os.write(v.data(),
           static_cast<std::make_signed<std::size_t>::type>(v.size()));
  return os;
}

typedef basic_string_view<char> string_view;
typedef basic_string_view<wchar_t> wstring_view;
typedef basic_string_view<char16_t> u16string_view;
typedef basic_string_view<char32_t> u32string_view;

}  // namespace jasl

#ifdef __cpp_lib_string_view
#include <string_view>
namespace std {
template <typename CharT, typename Traits>
struct hash<jasl::basic_string_view<CharT, Traits>> {
  std::size_t operator()(const jasl::basic_string_view<CharT, Traits>& x) const
      noexcept {
    return std::hash<std::basic_string_view<CharT, Traits>>{}(
        std::basic_string_view<CharT, Traits>(x.data(), x.size()));
  }
};
}  // namespace std
#else
namespace std {
template <typename CharT, typename Traits>
struct hash<jasl::basic_string_view<CharT, Traits>> {
  std::size_t operator()(const jasl::basic_string_view<CharT, Traits>& x) const
      noexcept {
    return std::hash<std::basic_string<CharT, Traits>>{}(
        std::basic_string<CharT, Traits>(x.data(), x.size()));
  }
};
}  // namespace std
#endif
