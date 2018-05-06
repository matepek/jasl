// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#pragma once

#include "jasl_common.hpp"

#include <memory>
#include <string>
#include "jasl_string_view.hpp"

namespace jasl {

template <typename CharT, class Traits = std::char_traits<CharT>>
class basic_string : public basic_string_view<CharT, Traits> {
 public:
  typedef basic_string_view<CharT, Traits> base_type;

 private:
  static const CharT empty_string[1];
  typedef std::unique_ptr<const CharT[]> dyn_type;
  dyn_type _dyn;
  bool _is_null_terminated;

 private:
  struct ParamterWrapper {
    typename base_type::size_type _size;
    const CharT* _ptr;

// constexpr: http://en.cppreference.com/w/cpp/string/char_traits/length
// char_traits:
// https://docs.microsoft.com/en-us/cpp/visual-cpp-language-conformance
#ifdef _MSC_VER
#define JASL_TEMP_CONSTEXPR_FOR_PW
#else
#define JASL_TEMP_CONSTEXPR_FOR_PW constexpr
#endif
    JASL_TEMP_CONSTEXPR_FOR_PW ParamterWrapper(const CharT* ptr) noexcept
        : _size(Traits::length(ptr)), _ptr(ptr) {}
#undef JASL_TEMP_CONSTEXPR_FOR_PW
    constexpr ParamterWrapper(const base_type& other) noexcept
        : _size(other.size()), _ptr(other.data()) {}
  };

 public:
  JASL_CONSTEXPR_FROM_14 basic_string() noexcept(
      std::is_nothrow_constructible<base_type, const CharT*, size_t>::value)
      : base_type(empty_string, 0), _is_null_terminated(true) {}

  JASL_CONSTEXPR_FROM_14 basic_string(const ParamterWrapper& paramW)
      : _is_null_terminated(true) {
    std::unique_ptr<CharT[]> dyn(new CharT[paramW._size + 1]());
    Traits::copy(dyn.get(), paramW._ptr, paramW._size);
    // base_type(std::string_view) may not null terminated
    dyn[paramW._size] = 0;
    _dyn = std::move(dyn);
    base_type::operator=(base_type(_dyn.get(), paramW._size));
  }

  template <std::size_t N>
  constexpr basic_string(const CharT (&str)[N]) noexcept(
      std::is_nothrow_constructible<base_type, const CharT*, std::size_t>::
          value)
      : base_type(str, str[N - 1] == 0 ? N - 1 : N),
        _is_null_terminated(str[N - 1] == 0) {}

  JASL_CONSTEXPR_FROM_14 basic_string(const basic_string& other)
      : base_type(other.data(), other.size()),
        _is_null_terminated(other._is_null_terminated) {
    if (other._dyn != nullptr) {
      std::unique_ptr<CharT[]> dyn(new CharT[other.size() + 1]());
      Traits::copy(dyn.get(), other.data(), other.size());
      dyn[other.size()] = 0;
      _dyn = std::move(dyn);
      base_type::operator=(base_type(_dyn.get(), other.size()));
    }
  }

  JASL_CONSTEXPR_FROM_14 basic_string(basic_string&& other) noexcept(
      std::is_nothrow_move_assignable<base_type>::value&&
          std::is_nothrow_move_assignable<dyn_type>::value) {
    _dyn = std::move(other._dyn);
    base_type::operator=(std::move(other));
    _is_null_terminated = other._is_null_terminated;
  }

  template <std::size_t N>
  JASL_CONSTEXPR_FROM_14 basic_string& assign(const CharT (&str)[N]) noexcept(
      std::is_nothrow_assignable<base_type, base_type>::value&&
          std::is_nothrow_destructible<dyn_type>::value) {
    _dyn.reset();
    base_type::operator=(base_type(str, str[N - 1] == 0 ? N - 1 : N));
    _is_null_terminated = str[N - 1] == 0;
    return *this;
  }

  JASL_CONSTEXPR_FROM_14 basic_string& assign(const ParamterWrapper& paramW) {
    std::unique_ptr<CharT[]> dyn(new CharT[paramW._size + 1]());
    Traits::copy(dyn.get(), paramW._ptr, paramW._size);
    dyn[paramW._size] = 0;
    _dyn = std::move(dyn);
    base_type::operator=(base_type(_dyn.get(), paramW._size));
    _is_null_terminated = true;
    return *this;
  }

  JASL_CONSTEXPR_FROM_14 basic_string& assign(const basic_string& other) {
    if (other._dyn == nullptr) {
      _dyn.reset();
      base_type::operator=(other);
      _is_null_terminated = other._is_null_terminated;
    } else {
      std::unique_ptr<CharT[]> dyn(new CharT[other.size() + 1]());
      Traits::copy(dyn.get(), other.data(), other.size());
      dyn[other.size()] = 0;
      _dyn = std::move(dyn);
      base_type::operator=(base_type(_dyn.get(), other.size()));
      _is_null_terminated = true;
    }
    return *this;
  }

  JASL_CONSTEXPR_FROM_14 basic_string& assign(basic_string&& other) noexcept(
      std::is_nothrow_move_assignable<dyn_type>::value&&
          std::is_nothrow_move_assignable<base_type>::value) {
    _dyn = std::move(other._dyn);
    base_type::operator=(std::move(other));
    _is_null_terminated = other._is_null_terminated;
    return *this;
  }

  template <std::size_t N>
  JASL_CONSTEXPR_FROM_14 basic_string&
  operator=(const CharT (&str)[N]) noexcept(
      std::is_nothrow_assignable<base_type, base_type>::value) {
    return assign(str);
  }

  JASL_CONSTEXPR_FROM_14 basic_string& operator=(const basic_string& other) {
    return assign(other);
  }

  JASL_CONSTEXPR_FROM_14 basic_string& operator=(basic_string&& other) {
    return assign(std::move(other));
  }

  constexpr bool is_static() const noexcept { return _dyn == nullptr; }
  constexpr bool is_null_terminated() const noexcept {
    return _is_null_terminated;
  }

  JASL_CONSTEXPR_FROM_14 void swap(basic_string& other) noexcept(
#ifdef __cpp_lib_is_swappable
      std::is_nothrow_swappable<base_type>::value&&
          std::is_nothrow_swappable<dyn_type>::value
#else
      // http://en.cppreference.com/w/cpp/algorithm/swap
      std::is_nothrow_move_constructible<base_type>::value&&
          std::is_nothrow_move_assignable<base_type>::value&&
              std::is_nothrow_move_constructible<dyn_type>::value&&
                  std::is_nothrow_move_assignable<dyn_type>::value
#endif
  ) {
    std::swap(_dyn, other._dyn);
    std::swap(*this, other);
  }

  JASL_CONSTEXPR_FROM_14 basic_string
  substr(typename base_type::size_type pos) const {
    if (_dyn == nullptr) {
      basic_string cpy(*this);
      cpy.base_type::operator=(base_type::substr(pos));
      return cpy;
    } else {
      return basic_string(base_type::substr(pos));
    }
  }

  constexpr base_type substr(typename base_type::size_type pos,
                             typename base_type::size_type count) const {
    return base_type::substr(pos, count);
  }
};

template <typename CharT, typename Traits>
const CharT basic_string<CharT, Traits>::empty_string[1] = {0};

typedef basic_string<char> string;
typedef basic_string<wchar_t> wstring;
typedef basic_string<char16_t> u16string;
typedef basic_string<char32_t> u32string;

}  // namespace jasl
