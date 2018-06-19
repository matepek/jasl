// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

// from https://github.com/llvm-mirror/libcxx/blob/master/include/string_view

#pragma once

#include <iterator>

namespace jasl {

template <typename string_viewT>
class string_view_bridge {
 public:
  typedef string_viewT bridge_to_type;
  typedef typename string_viewT::traits_type traits_type;
  typedef typename string_viewT::value_type value_type;
  typedef typename string_viewT::pointer pointer;
  typedef typename string_viewT::const_pointer const_pointer;
  typedef typename string_viewT::reference reference;
  typedef typename string_viewT::const_reference const_reference;
  typedef typename string_viewT::const_iterator const_iterator;
  typedef typename string_viewT::iterator iterator;
  typedef typename string_viewT::const_reverse_iterator const_reverse_iterator;
  typedef typename string_viewT::reverse_iterator reverse_iterator;
  typedef typename string_viewT::size_type size_type;
  typedef typename string_viewT::difference_type difference_type;
  static constexpr size_type npos = string_viewT::npos;

 protected:
  constexpr string_view_bridge() noexcept(
      std::is_nothrow_constructible<string_viewT>::value) {}
  constexpr explicit string_view_bridge(string_viewT s) noexcept(
      std::is_nothrow_constructible<string_viewT, string_viewT>::value)
      : _sv(s) {}
  constexpr string_view_bridge(const string_view_bridge& s) noexcept(
      std::is_nothrow_copy_constructible<string_viewT>::value) = default;
  constexpr explicit string_view_bridge(const value_type* str) noexcept(
      std::is_nothrow_constructible<string_viewT, const value_type*>::value)
      : _sv(str) {}
  constexpr string_view_bridge(const value_type* str, size_type len) noexcept(
      std::is_nothrow_constructible<string_viewT,
                                    const value_type*,
                                    size_type>::value)
      : _sv(str, len) {}

  string_view_bridge& operator=(const string_view_bridge&) noexcept(
      std::is_nothrow_copy_assignable<string_viewT>::value) = default;

  JASL_CONSTEXPR_CXX14 void swap(string_view_bridge& s) noexcept(
      noexcept(std::declval<string_viewT>().swap(s._sv))) {
    _sv.swap(s._sv);
  }

  constexpr static bool is_nothrow_settable =
      std::is_nothrow_assignable<string_viewT, string_viewT>::value &&
      std::is_nothrow_constructible<string_viewT,
                                    const value_type*,
                                    size_type>::value;

  JASL_CONSTEXPR_CXX14 void set(const value_type* str,
                                size_type len) noexcept(is_nothrow_settable) {
    _sv = string_viewT(str, len);
  }

 public:
  operator string_viewT() const noexcept { return _sv; }

 public:
  constexpr const_iterator begin() const
      noexcept(noexcept(std::declval<string_viewT>().begin())) {
    return _sv.begin();
  }
  constexpr const_iterator end() const
      noexcept(noexcept(std::declval<string_viewT>().end())) {
    return _sv.end();
  }
  constexpr const_iterator cbegin() const
      noexcept(noexcept(std::declval<string_viewT>().cbegin())) {
    return _sv.cbegin();
  }
  constexpr const_iterator cend() const
      noexcept(noexcept(std::declval<string_viewT>().cend())) {
    return _sv.cend();
  }
  const_reverse_iterator rbegin() const
      noexcept(noexcept(std::declval<string_viewT>().rbegin())) {
    return _sv.rbegin();
  }
  const_reverse_iterator rend() const
      noexcept(noexcept(std::declval<string_viewT>().rend())) {
    return _sv.rend();
  }
  const_reverse_iterator crbegin() const
      noexcept(noexcept(std::declval<string_viewT>().crbegin())) {
    return _sv.crbegin();
  }
  const_reverse_iterator crend() const
      noexcept(noexcept(std::declval<string_viewT>().crend())) {
    return _sv.crend();
  }

  constexpr size_type size() const
      noexcept(noexcept(std::declval<string_viewT>().size())) {
    return _sv.size();
  }
  constexpr size_type length() const
      noexcept(noexcept(std::declval<string_viewT>().length())) {
    return _sv.length();
  }
  constexpr size_type max_size() const
      noexcept(noexcept(std::declval<string_viewT>().max_size())) {
    return _sv.max_size();
  }
  constexpr bool empty() const
      noexcept(noexcept(std::declval<string_viewT>().empty())) {
    return _sv.empty();
  }

  constexpr const_reference operator[](size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>()[pos])) {
    return _sv[pos];
  }
  constexpr const_reference at(size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().at(pos))) {
    return _sv.at(pos);
  }
  constexpr const_reference front() const
      noexcept(noexcept(std::declval<string_viewT>().front())) {
    return _sv.front();
  }
  constexpr const_reference back() const
      noexcept(noexcept(std::declval<string_viewT>().back())) {
    return _sv.back();
  }
  constexpr const_pointer data() const
      noexcept(noexcept(std::declval<string_viewT>().data())) {
    return _sv.data();
  }

  JASL_CONSTEXPR_CXX14 void remove_prefix(size_type n) noexcept(
      noexcept(std::declval<string_viewT>().remove_prefix(n))) {
    _sv.remove_prefix(n);
  }
  JASL_CONSTEXPR_CXX14 void remove_suffix(size_type n) noexcept(
      noexcept(std::declval<string_viewT>().remove_suffix(n))) {
    _sv.remove_suffix(n);
  }

  size_type copy(value_type* s, size_type n) const
      noexcept(noexcept(std::declval<string_viewT>().copy(s, n))) {
    return _sv.copy(s, n);
  }
  size_type copy(value_type* s, size_type n, size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().copy(s, n, pos))) {
    return _sv.copy(s, n, pos);
  }

  constexpr string_view_bridge substr(size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().substr(pos)) &&
               std::is_nothrow_copy_constructible<string_viewT>::value &&
               std::is_nothrow_copy_constructible<string_view_bridge>::value) {
    return string_view_bridge(string_viewT(_sv.substr(pos)));
  }
  constexpr string_view_bridge substr(size_type pos, size_type n) const
      noexcept(noexcept(std::declval<string_viewT>().substr(pos, n)) &&
               std::is_nothrow_copy_constructible<string_viewT>::value &&
               std::is_nothrow_copy_constructible<string_view_bridge>::value) {
    return string_view_bridge(string_viewT(_sv.substr(pos, n)));
  }
  constexpr int compare(string_view_bridge s) const
      noexcept(noexcept(std::declval<string_viewT>().compare(s._sv))) {
    return _sv.compare(s._sv);
  }
  constexpr int compare(size_type pos1,
                        size_type n1,
                        string_view_bridge s) const
      noexcept(
          noexcept(std::declval<string_viewT>().compare(pos1, n1, s._sv))) {
    return _sv.compare(pos1, n1, s._sv);
  }
  constexpr int compare(size_type pos1,
                        size_type n1,
                        string_view_bridge s,
                        size_type pos2,
                        size_type n2) const
      noexcept(noexcept(
          std::declval<string_viewT>().compare(pos1, n1, s._sv, pos2, n2))) {
    return _sv.compare(pos1, n1, s._sv, pos2, n2);
  }
  constexpr int compare(const value_type* s) const
      noexcept(noexcept(std::declval<string_viewT>().compare(s))) {
    return _sv.compare(s);
  }
  constexpr int compare(size_type pos1, size_type n1, const value_type* s) const
      noexcept(noexcept(std::declval<string_viewT>().compare(pos1, n1, s))) {
    return _sv.compare(pos1, n1, s);
  }
  constexpr int compare(size_type pos1,
                        size_type n1,
                        const value_type* s,
                        size_type n2) const
      noexcept(
          noexcept(std::declval<string_viewT>().compare(pos1, n1, s, n2))) {
    return _sv.compare(pos1, n1, s, n2);
  }
  constexpr size_type find(string_view_bridge s) const
      noexcept(noexcept(std::declval<string_viewT>().find(s._sv))) {
    return _sv.find(s._sv);
  }
  constexpr size_type find(string_view_bridge s, size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().find(s._sv, pos))) {
    return _sv.find(s._sv, pos);
  }
  constexpr size_type find(value_type c) const
      noexcept(noexcept(std::declval<string_viewT>().find(c))) {
    return _sv.find(c);
  }
  constexpr size_type find(value_type c, size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().find(c, pos))) {
    return _sv.find(c, pos);
  }
  constexpr size_type find(const value_type* s,
                           size_type pos,
                           size_type n) const
      noexcept(noexcept(std::declval<string_viewT>().find(s, pos, n))) {
    return _sv.find(s, pos, n);
  }
  constexpr size_type find(const value_type* s) const
      noexcept(noexcept(std::declval<string_viewT>().find(s))) {
    return _sv.find(s);
  }
  constexpr size_type find(const value_type* s, size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().find(s, pos))) {
    return _sv.find(s, pos);
  }
  constexpr size_type rfind(string_view_bridge s) const
      noexcept(noexcept(std::declval<string_viewT>().rfind(s._sv))) {
    return _sv.rfind(s._sv);
  }
  constexpr size_type rfind(string_view_bridge s, size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().rfind(s._sv, pos))) {
    return _sv.rfind(s._sv, pos);
  }
  constexpr size_type rfind(value_type c) const
      noexcept(noexcept(std::declval<string_viewT>().rfind(c))) {
    return _sv.rfind(c);
  }
  constexpr size_type rfind(value_type c, size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().rfind(c, pos))) {
    return _sv.rfind(c, pos);
  }
  constexpr size_type rfind(const value_type* s,
                            size_type pos,
                            size_type n) const
      noexcept(noexcept(std::declval<string_viewT>().rfind(s, pos, n))) {
    return _sv.rfind(s, pos, n);
  }
  constexpr size_type rfind(const value_type* s) const
      noexcept(noexcept(std::declval<string_viewT>().rfind(s))) {
    return _sv.rfind(s);
  }
  constexpr size_type rfind(const value_type* s, size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().rfind(s, pos))) {
    return _sv.rfind(s, pos);
  }
  constexpr size_type find_first_of(string_view_bridge s) const
      noexcept(noexcept(std::declval<string_viewT>().find_first_of(s._sv))) {
    return _sv.find_first_of(s._sv);
  }
  constexpr size_type find_first_of(string_view_bridge s, size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().find_first_of(s._sv,
                                                                   pos))) {
    return _sv.find_first_of(s._sv, pos);
  }
  constexpr size_type find_first_of(value_type c) const
      noexcept(noexcept(std::declval<string_viewT>().find_first_of(c))) {
    return _sv.find_first_of(c);
  }
  constexpr size_type find_first_of(value_type c, size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().find_first_of(c, pos))) {
    return _sv.find_first_of(c, pos);
  }
  constexpr size_type find_first_of(const value_type* s,
                                    size_type pos,
                                    size_type n) const
      noexcept(
          noexcept(std::declval<string_viewT>().find_first_of(s, pos, n))) {
    return _sv.find_first_of(s, pos, n);
  }
  constexpr size_type find_first_of(const value_type* s) const
      noexcept(noexcept(std::declval<string_viewT>().find_first_of(s))) {
    return _sv.find_first_of(s);
  }
  constexpr size_type find_first_of(const value_type* s, size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().find_first_of(s, pos))) {
    return _sv.find_first_of(s, pos);
  }
  constexpr size_type find_last_of(string_view_bridge s) const
      noexcept(noexcept(std::declval<string_viewT>().find_last_of(s._sv))) {
    return _sv.find_last_of(s._sv);
  }
  constexpr size_type find_last_of(string_view_bridge s, size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().find_last_of(s._sv,
                                                                  pos))) {
    return _sv.find_last_of(s._sv, pos);
  }
  constexpr size_type find_last_of(value_type c) const
      noexcept(noexcept(std::declval<string_viewT>().find_last_of(c))) {
    return _sv.find_last_of(c);
  }
  constexpr size_type find_last_of(value_type c, size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().find_last_of(c, pos))) {
    return _sv.find_last_of(c, pos);
  }
  constexpr size_type find_last_of(const value_type* s,
                                   size_type pos,
                                   size_type n) const
      noexcept(noexcept(std::declval<string_viewT>().find_last_of(s, pos, n))) {
    return _sv.find_last_of(s, pos, n);
  }
  constexpr size_type find_last_of(const value_type* s) const
      noexcept(noexcept(std::declval<string_viewT>().find_last_of(s))) {
    return _sv.find_last_of(s);
  }
  constexpr size_type find_last_of(const value_type* s, size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().find_last_of(s, pos))) {
    return _sv.find_last_of(s, pos);
  }
  constexpr size_type find_first_not_of(string_view_bridge s) const noexcept(
      noexcept(std::declval<string_viewT>().find_first_not_of(s._sv))) {
    return _sv.find_first_not_of(s._sv);
  }
  constexpr size_type find_first_not_of(string_view_bridge s,
                                        size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().find_first_not_of(s._sv,
                                                                       pos))) {
    return _sv.find_first_not_of(s._sv, pos);
  }
  constexpr size_type find_first_not_of(value_type c) const
      noexcept(noexcept(std::declval<string_viewT>().find_first_not_of(c))) {
    return _sv.find_first_not_of(c);
  }
  constexpr size_type find_first_not_of(value_type c, size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().find_first_not_of(c,
                                                                       pos))) {
    return _sv.find_first_not_of(c, pos);
  }
  constexpr size_type find_first_not_of(const value_type* s,
                                        size_type pos,
                                        size_type n) const
      noexcept(
          noexcept(std::declval<string_viewT>().find_first_not_of(s, pos, n))) {
    return _sv.find_first_not_of(s, pos, n);
  }
  constexpr size_type find_first_not_of(const value_type* s) const
      noexcept(noexcept(std::declval<string_viewT>().find_first_not_of(s))) {
    return _sv.find_first_not_of(s);
  }
  constexpr size_type find_first_not_of(const value_type* s,
                                        size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().find_first_not_of(s,
                                                                       pos))) {
    return _sv.find_first_not_of(s, pos);
  }
  constexpr size_type find_last_not_of(string_view_bridge s) const
      noexcept(noexcept(std::declval<string_viewT>().find_last_not_of(s._sv))) {
    return _sv.find_last_not_of(s._sv);
  }
  constexpr size_type find_last_not_of(string_view_bridge s,
                                       size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().find_last_not_of(s._sv,
                                                                      pos))) {
    return _sv.find_last_not_of(s._sv, pos);
  }
  constexpr size_type find_last_not_of(value_type c) const
      noexcept(noexcept(std::declval<string_viewT>().find_last_not_of(c))) {
    return _sv.find_last_not_of(c);
  }
  constexpr size_type find_last_not_of(value_type c, size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().find_last_not_of(c,
                                                                      pos))) {
    return _sv.find_last_not_of(c, pos);
  }
  constexpr size_type find_last_not_of(const value_type* s,
                                       size_type pos,
                                       size_type n) const
      noexcept(
          noexcept(std::declval<string_viewT>().find_last_not_of(s, pos, n))) {
    return _sv.find_last_not_of(s, pos, n);
  }
  constexpr size_type find_last_not_of(const value_type* s) const
      noexcept(noexcept(std::declval<string_viewT>().find_last_not_of(s))) {
    return _sv.find_last_not_of(s);
  }
  constexpr size_type find_last_not_of(const value_type* s, size_type pos) const
      noexcept(noexcept(std::declval<string_viewT>().find_last_not_of(s,
                                                                      pos))) {
    return _sv.find_last_not_of(s, pos);
  }
  constexpr bool starts_with(string_view_bridge s) const
      noexcept(noexcept(std::declval<string_viewT>().starts_with(s._sv))) {
    return _sv.starts_with(s._sv);
  }
  constexpr bool starts_with(value_type c) const
      noexcept(noexcept(std::declval<string_viewT>().starts_with(c))) {
    return _sv.starts_with(c);
  }
  constexpr bool starts_with(const value_type* s) const
      noexcept(noexcept(std::declval<string_viewT>().starts_with(s))) {
    return _sv.starts_with(s);
  }
  constexpr bool ends_with(string_view_bridge s) const
      noexcept(noexcept(std::declval<string_viewT>().ends_with(s._sv))) {
    return _sv.ends_with(s._sv);
  }
  constexpr bool ends_with(value_type c) const
      noexcept(noexcept(std::declval<string_viewT>().ends_with(c))) {
    return _sv.ends_with(c);
  }
  constexpr bool ends_with(const value_type* s) const
      noexcept(noexcept(std::declval<string_viewT>().ends_with(s))) {
    return _sv.ends_with(s);
  }

 protected:
  string_viewT _sv;

 public:
  friend JASL_CONSTEXPR_CXX14 bool operator==(
      const string_view_bridge<string_viewT>& x,
      const string_view_bridge<string_viewT>& y) noexcept(noexcept(x._sv ==
                                                                   y._sv)) {
    return x._sv == y._sv;
  }
  friend JASL_CONSTEXPR_CXX14 bool operator!=(
      const string_view_bridge<string_viewT>& x,
      const string_view_bridge<string_viewT>& y) noexcept(noexcept(x._sv !=
                                                                   y._sv)) {
    return x._sv != y._sv;
  }
  friend JASL_CONSTEXPR_CXX14 bool operator<(
      const string_view_bridge<string_viewT>& x,
      const string_view_bridge<string_viewT>& y) noexcept(noexcept(x._sv <
                                                                   y._sv)) {
    return x._sv < y._sv;
  }
  friend JASL_CONSTEXPR_CXX14 bool operator>(
      const string_view_bridge<string_viewT>& x,
      const string_view_bridge<string_viewT>& y) noexcept(noexcept(x._sv >
                                                                   y._sv)) {
    return x._sv > y._sv;
  }
  friend JASL_CONSTEXPR_CXX14 bool operator<=(
      const string_view_bridge<string_viewT>& x,
      const string_view_bridge<string_viewT>& y) noexcept(noexcept(x._sv <=
                                                                   y._sv)) {
    return x._sv <= y._sv;
  }
  friend JASL_CONSTEXPR_CXX14 bool operator>=(
      const string_view_bridge<string_viewT>& x,
      const string_view_bridge<string_viewT>& y) noexcept(noexcept(x._sv >=
                                                                   y._sv)) {
    return x._sv >= y._sv;
  }

  friend JASL_CONSTEXPR_CXX14 bool operator==(
      const string_view_bridge<string_viewT>& x,
      const string_viewT& y) noexcept(noexcept(x._sv == y)) {
    return x._sv == y;
  }
  friend JASL_CONSTEXPR_CXX14 bool operator!=(
      const string_view_bridge<string_viewT>& x,
      const string_viewT& y) noexcept(noexcept(x._sv != y)) {
    return x._sv != y;
  }
  friend JASL_CONSTEXPR_CXX14 bool operator<(
      const string_view_bridge<string_viewT>& x,
      const string_viewT& y) noexcept(noexcept(x._sv < y)) {
    return x._sv < y;
  }
  friend JASL_CONSTEXPR_CXX14 bool operator>(
      const string_view_bridge<string_viewT>& x,
      const string_viewT& y) noexcept(noexcept(x._sv > y)) {
    return x._sv > y;
  }
  friend JASL_CONSTEXPR_CXX14 bool operator<=(
      const string_view_bridge<string_viewT>& x,
      const string_viewT& y) noexcept(noexcept(x._sv <= y)) {
    return x._sv <= y;
  }
  friend JASL_CONSTEXPR_CXX14 bool operator>=(
      const string_view_bridge<string_viewT>& x,
      const string_viewT& y) noexcept(noexcept(x._sv >= y)) {
    return x._sv >= y;
  }

  friend JASL_CONSTEXPR_CXX14 bool operator==(
      const string_viewT& x,
      const string_view_bridge<string_viewT>& y) noexcept(noexcept(x ==
                                                                   y._sv)) {
    return x == y._sv;
  }
  friend JASL_CONSTEXPR_CXX14 bool operator!=(
      const string_viewT& x,
      const string_view_bridge<string_viewT>& y) noexcept(noexcept(x !=
                                                                   y._sv)) {
    return x != y._sv;
  }
  friend JASL_CONSTEXPR_CXX14 bool operator<(
      const string_viewT& x,
      const string_view_bridge<string_viewT>& y) noexcept(noexcept(x < y._sv)) {
    return x < y._sv;
  }
  friend JASL_CONSTEXPR_CXX14 bool operator>(
      const string_viewT& x,
      const string_view_bridge<string_viewT>& y) noexcept(noexcept(x > y._sv)) {
    return x > y._sv;
  }
  friend JASL_CONSTEXPR_CXX14 bool operator<=(
      const string_viewT& x,
      const string_view_bridge<string_viewT>& y) noexcept(noexcept(x <=
                                                                   y._sv)) {
    return x <= y._sv;
  }
  friend JASL_CONSTEXPR_CXX14 bool operator>=(
      const string_viewT& x,
      const string_view_bridge<string_viewT>& y) noexcept(noexcept(x >=
                                                                   y._sv)) {
    return x >= y._sv;
  }

  friend std::basic_ostream<value_type, traits_type>& operator<<(
      std::basic_ostream<value_type, traits_type>& os,
      const string_view_bridge<string_viewT>& str) {
    return os << str._sv;
  }

  friend struct std::hash<string_view_bridge>;
};

}  // namespace jasl

namespace std {
template <typename string_viewT>
struct hash<jasl::string_view_bridge<string_viewT>> {
  size_t operator()(jasl::string_view_bridge<string_viewT> x) const
      noexcept(noexcept(std::hash<string_viewT>{}(x._sv))) {
    return std::hash<string_viewT>{}(x._sv);
  }
};
}  // namespace std
