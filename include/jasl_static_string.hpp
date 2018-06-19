// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#pragma once

#include <string>

#include "jasl_common.hpp"
#include "jasl_feature_test_macro.hpp"
#include "jasl_string_view.hpp"
#include "jasl_string_view_bridge.hpp"

namespace jasl {

template <typename CharT, class Traits = std::char_traits<CharT>>
class basic_static_string
    : public string_view_bridge<basic_string_view<CharT, Traits>> {
 public:
  typedef string_view_bridge<basic_string_view<CharT, Traits>> bridge_type;
  typedef typename bridge_type::bridge_to_type base_type;

 private:
  constexpr basic_static_string(const bridge_type& other) noexcept(
      std::is_nothrow_constructible<bridge_type, const bridge_type&>::value)
      : bridge_type(other) {}

 public:
  JASL_CONSTEXPR_CXX14 basic_static_string() noexcept(
      std::is_nothrow_constructible<bridge_type>::value)
      : bridge_type() {}

  template <size_t N>
  constexpr basic_static_string(const CharT (&str)[N]) noexcept(
      std::is_nothrow_constructible<bridge_type, const CharT*, size_t>::value)
      : bridge_type(str, str[N - 1] == 0 ? N - 1 : N) {}

  constexpr basic_static_string(const basic_static_string& other) noexcept(
      std::is_nothrow_constructible<bridge_type, const bridge_type&>::value)
      : bridge_type(static_cast<const bridge_type&>(other)) {}

  ~basic_static_string() noexcept(
      std::is_nothrow_destructible<bridge_type>::value) = default;

  template <size_t N>
  JASL_CONSTEXPR_CXX14 basic_static_string& operator=(
      const CharT (&str)[N]) noexcept(bridge_type::is_nothrow_settable) {
    bridge_type::set(str, str[N - 1] == 0 ? N - 1 : N);
    return *this;
  }

  JASL_CONSTEXPR_CXX14 basic_static_string&
  operator=(const basic_static_string& other) noexcept(
      std::is_nothrow_assignable<bridge_type, const bridge_type&>::value) {
    bridge_type::operator=(static_cast<const bridge_type&>(other));
    return *this;
  }

  JASL_CONSTEXPR_CXX14 void swap(basic_static_string& other) noexcept(
      JASL_is_nothrow_swappable_value(bridge_type)) {
    bridge_type::swap(other);
  }

  constexpr basic_static_string substr(
      typename bridge_type::size_type pos,
      typename bridge_type::size_type count = bridge_type::npos) const {
    return basic_static_string(bridge_type::substr(pos, count));
  }
};

template <typename CharT, typename Traits>
void swap(
    basic_static_string<CharT, Traits>& lhs,
    basic_static_string<CharT, Traits>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}

typedef basic_static_string<char> static_string;
typedef basic_static_string<wchar_t> static_wstring;
typedef basic_static_string<char16_t> static_u16string;
typedef basic_static_string<char32_t> static_u32string;

}  // namespace jasl
