// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#pragma once

#include "jasl_common.hpp"

#include <string>
#include "jasl_string_view.hpp"

namespace jasl {

template <typename CharT, class Traits = std::char_traits<CharT> >
class basic_static_string : public basic_string_view<CharT, Traits> {
 public:
  typedef basic_string_view<CharT, Traits> base_type;

 private:
  static const CharT empty_string[1];

 private:
  constexpr basic_static_string(const base_type& other) noexcept(
      std::is_nothrow_constructible<base_type, const base_type&>::value)
      : base_type(other) {}

 public:
  JASL_CONSTEXPR_FROM_14 basic_static_string() noexcept(
      std::is_nothrow_constructible<base_type, const CharT*, size_t>::value)
      : base_type(empty_string, 0) {}

  template <std::size_t N>
  constexpr basic_static_string(const CharT (&str)[N]) noexcept(
      std::is_nothrow_constructible<base_type, const CharT*, std::size_t>::
          value)
      : base_type(str, str[N - 1] == 0 ? N - 1 : N) {}

  constexpr basic_static_string(const basic_static_string& other) noexcept(
      std::is_nothrow_constructible<base_type, const base_type&>::value)
      : base_type(static_cast<const base_type&>(other)) {}

  ~basic_static_string() noexcept(
      std::is_nothrow_destructible<base_type>::value) = default;

  template <std::size_t N>
  JASL_CONSTEXPR_FROM_14 basic_static_string&
  operator=(const CharT (&str)[N]) noexcept(
      std::is_nothrow_assignable<base_type, base_type>::value) {
    this->base_type::operator=(base_type(str, str[N - 1] == 0 ? N - 1 : N));
    return *this;
  }

  JASL_CONSTEXPR_FROM_14 basic_static_string&
  operator=(const basic_static_string& other) noexcept(
      std::is_nothrow_assignable<base_type, const base_type&>::value) {
    this->base_type::operator=(static_cast<const base_type&>(other));
    return *this;
  }

  JASL_CONSTEXPR_FROM_14 void swap(basic_static_string& other) noexcept(
#ifdef __cpp_lib_is_swappable
      std::is_nothrow_swappable<base_type>::value
#else
      // http://en.cppreference.com/w/cpp/algorithm/swap
      std::is_nothrow_move_constructible<base_type>::value&&
          std::is_nothrow_move_assignable<base_type>::value
#endif
  ) {
    std::swap<base_type>(*this, other);
  }

  constexpr basic_static_string substr(
      typename base_type::size_type pos,
      typename base_type::size_type count = base_type::npos) const {
    return basic_static_string(this->base_type::substr(pos, count));
  }
};

template <typename CharT, typename Traits>
const CharT basic_static_string<CharT, Traits>::empty_string[1] = {0};

typedef basic_static_string<char> static_string;
typedef basic_static_string<wchar_t> static_wstring;
typedef basic_static_string<char16_t> static_u16string;
typedef basic_static_string<char32_t> static_u32string;

}  // namespace jasl
