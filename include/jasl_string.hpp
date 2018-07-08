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
#include "jasl_static_string.hpp"
#include "jasl_string_view.hpp"
#include "jasl_string_view_bridge.hpp"

namespace jasl {

template <typename CharT,
          typename Traits = std::char_traits<CharT>,
          typename AllocatorT = std::allocator<CharT>>
class basic_string
    : public inner::string_view_bridge<basic_string_view<CharT, Traits>> {
 public:
  typedef inner::string_view_bridge<basic_string_view<CharT, Traits>>
      bridge_type;
  typedef typename bridge_type::bridge_to_type base_type;
  typedef AllocatorT allocator_type;

 private:
  typedef std::allocator_traits<AllocatorT> alloc_traits;

  // https://stackoverflow.com/questions/12332772/why-arent-container-move-assignment-operators-noexcept
  constexpr static bool is_noexcept_assign_rvalue =
      alloc_traits::propagate_on_container_move_assignment::value &&
      std::is_nothrow_move_assignable<AllocatorT>::value &&
      JASL_is_nothrow_swappable_value(bridge_type);

 private:
  allocator_type _alloc;
  size_t _cap;  // capacity

 private:
  struct CStr {
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
    JASL_TEMP_CONSTEXPR_FOR_PW CStr(const CharT* p) noexcept
        : size(Traits::length(p)), ptr(p) {}
#undef JASL_TEMP_CONSTEXPR_FOR_PW
  };

  inline void init(const CharT* ptr, size_t size) {
    JASL_ASSERT(_cap == 0, "_cap == 0");
    static_assert(
        std::is_standard_layout<CharT>::value && std::is_trivial<CharT>::value,
        "Unsupported CharT");
    if (size == 0) {
      bridge_type::set(nullptr, 0);
      return;
    }
    std::unique_ptr<CharT, std::function<void(CharT*)>> begin(
        alloc_traits::allocate(_alloc, size), [&](CharT * ptd) noexcept {
          alloc_traits::deallocate(_alloc, ptd, size);
        });
    bridge_type::set(begin.get(), size);
    _cap = size;
    auto raw_begin = begin.release();
    Traits::copy(raw_begin, ptr, size);
  }

  inline void dispose() noexcept {
    if (_cap > 0) {
      const auto begin = const_cast<CharT*>(bridge_type::data());
      alloc_traits::deallocate(_alloc, begin, _cap);
      _cap = 0;
    }
  }

  basic_string(const bridge_type& other, const AllocatorT& alloc)
      : bridge_type(), _alloc(alloc), _cap(0) {
    init(other.data(), other.size());
  }

 public:
  ~basic_string() {
    if (_cap > 0) {
      alloc_traits::deallocate(_alloc, const_cast<CharT*>(bridge_type::data()),
                               _cap);
    }
  }

  basic_string() noexcept(
      std::is_nothrow_constructible<bridge_type>::value&&
          std::is_nothrow_default_constructible<AllocatorT>::value)
      : basic_string(AllocatorT()) {}

  basic_string(const AllocatorT& a) noexcept(
      std::is_nothrow_constructible<bridge_type>::value&&
          std::is_nothrow_copy_constructible<AllocatorT>::value)
      : bridge_type(), _alloc(a), _cap(0) {}

  basic_string(const CStr& cstr, const AllocatorT& alloc = AllocatorT())
      : bridge_type(), _alloc(alloc), _cap(0) {
    JASL_ASSERT(cstr.ptr != nullptr, "cstr != nullptr");
    init(cstr.ptr, cstr.size);
  }

  basic_string(const CharT* ptr,
               size_t size,
               const AllocatorT& alloc = AllocatorT())
      : bridge_type(), _alloc(alloc), _cap(0) {
    JASL_ASSERT(ptr != nullptr, "ptr != nullptr");
    init(ptr, size);
  }

  template <size_t N>
  basic_string(const CharT (&str)[N]) noexcept(
      std::is_nothrow_constructible<bridge_type, const CharT*, size_t>::value&&
          std::is_nothrow_default_constructible<AllocatorT>::value)
      : bridge_type(str, str[N - 1] == 0 ? N - 1 : N), _alloc(), _cap(0) {}

  template <size_t N>
  basic_string(const CharT (&str)[N], const AllocatorT& alloc) noexcept(
      std::is_nothrow_constructible<bridge_type, const CharT*, size_t>::value&&
          std::is_nothrow_copy_constructible<AllocatorT>::value)
      : bridge_type(str, str[N - 1] == 0 ? N - 1 : N), _alloc(alloc), _cap(0) {}

  basic_string(const basic_string& other)
      : basic_string(
            other,
            alloc_traits::select_on_container_copy_construction(other._alloc)) {
  }

  basic_string(const basic_string& other, const AllocatorT& alloc)
      : bridge_type(other), _alloc(alloc), _cap(0) {
    if (other._cap > 0) {
      init(other.data(), other.size());
    }
  }

  basic_string(basic_string&& other) noexcept(
      std::is_nothrow_move_constructible<bridge_type>::value&&
          std::is_nothrow_move_constructible<AllocatorT>::value)
      : bridge_type(std::move(other)),
        _alloc(std::move(other._alloc)),
        _cap(other._cap) {
    other._cap = 0;
  }

  basic_string(basic_string&& other, const AllocatorT& alloc)
      : bridge_type(), _alloc(alloc), _cap(0) {
    if (_alloc == other._alloc) {
      bridge_type::set(other.data(), other.size());
      std::swap(_cap, other._cap);
    } else {
      init(other.data(), other.size());
    }
  }

  basic_string(const basic_static_string<CharT, Traits>& ss) noexcept(
      std::is_nothrow_constructible<bridge_type, const CharT*, size_t>::value&&
          std::is_nothrow_default_constructible<AllocatorT>::value)
      : bridge_type(ss.data(), ss.size()), _alloc(), _cap(0) {}

  basic_string(
      const basic_static_string<CharT, Traits>& ss,
      const AllocatorT&
          alloc) noexcept(std::is_nothrow_constructible<bridge_type,
                                                        const CharT*,
                                                        size_t>::value&& std::
                              is_nothrow_copy_constructible<AllocatorT>::value)
      : bridge_type(ss.data(), ss.size()), _alloc(alloc), _cap(0) {}

#if defined(JASL_SUPPORT_STD_TO_JASL)
#if defined(JASL_cpp_lib_string_view)
  template <
      typename T,
      typename = typename std::enable_if<
          std::is_convertible<const T&,
                              std::basic_string_view<CharT, Traits>>::value &&
          !std::is_convertible<const T&, const CharT*>::value>::type>
  explicit basic_string(const T& s, const AllocatorT& alloc = AllocatorT())
      : bridge_type(), _alloc(alloc), _cap(0) {
    std::basic_string_view<CharT, Traits> sv(s);
    init(sv.data(), sv.size());
  }

  template <
      typename T,
      typename = typename std::enable_if<
          std::is_convertible<const T&,
                              std::basic_string_view<CharT, Traits>>::value &&
          !std::is_convertible<const T&, const CharT*>::value>::type>
  basic_string& operator=(const T& s) {
    dispose();
    std::basic_string_view<CharT, Traits> sv(s);
    init(sv.data(), sv.size());
    return *this;
  }
#else
  template <
      typename T,
      typename = typename std::enable_if<std::is_same<
          const T&,
          const std::basic_string<CharT, Traits, AllocatorT>&>::value>::type>
  explicit basic_string(const T& s, const AllocatorT& alloc = AllocatorT())
      : bridge_type(), _alloc(alloc), _cap(0) {
    init(s.data(), s.size());
  }

  template <
      typename T,
      typename = typename std::enable_if<std::is_same<
          const T&,
          const std::basic_string<CharT, Traits, AllocatorT>&>::value>::type>
  basic_string& operator=(const T& s) {
    dispose();
    init(s.data(), s.size());
    return *this;
  }
#endif
#endif

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

  template <size_t N>
  basic_string& assign(const CharT (&str)[N]) noexcept(
      bridge_type::is_nothrow_settable) {
    dispose();
    bridge_type::set(str, str[N - 1] == 0 ? N - 1 : N);
    return *this;
  }

  basic_string& assign(const CStr& cstr) {
    dispose();
    init(cstr.ptr, cstr.size);
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
        _alloc != other._alloc) {
      _alloc = other._alloc;
    }
    if (other._cap == 0) {
      bridge_type::operator=(other);
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
      _alloc = std::move(other._alloc);
    } else if (_alloc != other._alloc) {
      init(other.data(), other.size());
      return *this;
    }
    bridge_type::swap(other);
    std::swap(_cap, other._cap);
    return *this;
  }

  basic_string& assing(const basic_static_string<CharT, Traits>& ss) noexcept(
      bridge_type::is_nothrow_settable) {
    dispose();
    bridge_type::set(ss.data(), ss.size());
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

  basic_string&
  operator=(const basic_static_string<CharT, Traits>& other) noexcept(
      bridge_type::is_nothrow_settable) {
    return assign(other);
  }

  constexpr bool is_static() const noexcept { return _cap == 0; }
  AllocatorT get_alloc() const
      noexcept(std::is_nothrow_copy_constructible<AllocatorT>::value) {
    return _alloc;
  }

  void swap(basic_string& other) noexcept(
      (!alloc_traits::propagate_on_container_swap::value ||
       JASL_is_nothrow_swappable_value(AllocatorT)) &&
      JASL_is_nothrow_swappable_value(bridge_type)) {
    using std::swap;
    /*propagate_on_container_swap
     * true if the allocators of type A need to be swapped when two containers
     * that use them are swapped. If this member is false and the allocators of
     * the two containers do not compare equal, the behavior of container swap
     * is undefined.
     */
    if (alloc_traits::propagate_on_container_swap::value) {
      swap(_alloc, other._alloc);
    } else if (_alloc != other._alloc) {
      JASL_ASSERT(false, "Undefined behaviour");
      std::terminate();
    }
    bridge_type::swap(other);
    swap(_cap, other._cap);
  }

  basic_string substr(typename bridge_type::size_type pos) const {
    if (_cap == 0) {
      basic_string cpy(*this, _alloc);
      cpy.bridge_type::operator=(bridge_type::substr(pos));
      return cpy;
    } else {
      return basic_string(bridge_type::substr(pos), _alloc);
    }
  }

  basic_string substr(typename bridge_type::size_type pos,
                      typename bridge_type::size_type count) const {
    if (_cap == 0) {
      basic_string cpy(*this);
      cpy.bridge_type::operator=(bridge_type::substr(pos, count));
      return cpy;
    } else {
      return basic_string(bridge_type::substr(pos, count), _alloc);
    }
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
