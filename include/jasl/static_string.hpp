// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

/** @file */

#pragma once

#include <string>

#include "jasl/_jasl_internal/jasl_common.hpp"
#include "jasl/_jasl_internal/jasl_feature_test_macro.hpp"
#include "jasl/_jasl_internal/jasl_string_view_bridge.hpp"
#include "jasl/string_view.hpp"

namespace jasl {

/**
 * This class derives from [std::string_view] (or optionally from
 * [jasl::string_view]). What is special about it is that you can only assign
 * (or construct with) character arrays to it but not pointers. This makes it
 * safer and faster comparing to string_view-s.
 *
 * Remarks:
 * Why is there no c_str method? While jasl::static_string("one") is null
 * terminated, jasl::static_string({'t', 'w', 'o'}) is not. In both cases
 * the size()/length() of them will be 3, so the terminating null character will
 * be here in the first case but it won't be counted. If you want the null
 * terminator as a part of it, write jasl::static_string("one\0") or
 * jasl::static_string({'t', 'w', 'o', '\0', '\0'}).
 */

template <typename CharT, class Traits = std::char_traits<CharT>>
class basic_static_string
    : public inner::string_view_bridge<basic_string_view<CharT, Traits>> {
 public:
  typedef inner::string_view_bridge<basic_string_view<CharT, Traits>>
      bridge_type;
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

#if defined(JASL_SUPPORT_JASL_TO_STD)
#if defined(JASL_cpp_lib_string_view)
  operator std::basic_string_view<CharT, Traits>() const noexcept(
      std::is_nothrow_constructible<std::basic_string_view<CharT, Traits>,
                                    const CharT*,
                                    size_t>::value) {
    return std::basic_string_view<CharT, Traits>(bridge_type::data(),
                                                 bridge_type::size());
  }
#else
  template <typename AllocatorT>
  operator std::basic_string<CharT, Traits, AllocatorT>() const
      noexcept(std::is_nothrow_constructible<
               std::basic_string<CharT, Traits, AllocatorT>,
               const CharT*,
               size_t>::value) {
    return std::basic_string<CharT, Traits, AllocatorT>(bridge_type::data(),
                                                        bridge_type::size());
  }
#endif
#endif

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
