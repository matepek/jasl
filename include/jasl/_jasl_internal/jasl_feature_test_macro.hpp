// JASL: For more information see https://github.com/matepek/jasl
//
// Copyright (c) 2018 Mate Pek
//
// This code is licensed under the MIT License (MIT).

#pragma once

/*
 * JASL_cpp_constexpr
 * JASL library will try to figure out the correct value of __cpp_constexpr and
 * will define JASL_cpp_constexpr to it. JASL_cpp_constexpr also can be manually
 * defined.
 */

/*
 * JASL_cpp_lib_string_view
 * JASL library will try to figure out the correct value of
 * __cpp_lib_string_view and will define JASL_cpp_lib_string_view to it.
 * JASL_cpp_lib_string_view also can be manually defined.
 */

#if defined(__GNUG__) && !defined(__clang__)  // gcc
/*
 * https://gcc.gnu.org/projects/cxx-status.html
 * https://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html
 * http://nadeausoftware.com/articles/2012/10/c_c_tip_how_detect_compiler_name_and_version_using_compiler_predefined_macros
 */

#  ifndef JASL_cpp_constexpr
#    ifdef __cpp_constexpr
#      define JASL_cpp_constexpr __cpp_constexpr
#    else
#      define JASL_cpp_constexpr                                        \
        (__GNUC__ >= 7L                                                 \
             ? 201603L                                                  \
             : (__GNUC__ >= 5L                                          \
                    ? 201304L                                           \
                    : (__GNUC__ == 4L && __GNUC_MINOR__ >= 6L ? 200704L \
                                                              : 0L)))
#    endif  // __cpp_constexpr
#  endif    // JASL_cpp_constexpr
#  ifndef JASL_cpp_lib_string_view
#    ifdef __cpp_lib_string_view
#      define JASL_cpp_lib_string_view __cpp_lib_string_view
#    else
#      define JASL_cpp_lib_string_view                                       \
        (__cplusplus >= 201703 &&                                            \
                 (__GNUC__ > 7L || (__GNUC__ == 7L && __GNUC_MINOR__ >= 1L)) \
             ? 201606L                                                       \
             : 0L)
#    endif  // __cpp_lib_string_view
#  endif    // JASL_cpp_lib_string_view

#elif defined(__clang__)  // clang

#  ifndef JASL_cpp_constexpr
#    ifdef __cpp_constexpr
#      define JASL_cpp_constexpr __cpp_constexpr
#    else
#      error "Probably unsupported compiler version!"
#    endif
#  endif  // JASL_cpp_constexpr
#  ifndef JASL_cpp_lib_string_view
#    ifdef __cpp_lib_string_view
#      define JASL_cpp_lib_string_view __cpp_lib_string_view
#    elif __cplusplus >= 201703
#      define JASL_cpp_lib_string_view (__has_include(<string_view>) ? 201606L : 0L)
#    else
#      define JASL_cpp_lib_string_view 0L
#    endif
#  endif  // JASL_cpp_lib_string_view

#elif defined(_MSC_VER)  // msvc
/*
 * https://msdn.microsoft.com/en-us/library/b0084kay.aspx
 * https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B
 * https://docs.microsoft.com/en-us/cpp/visual-cpp-language-conformance
 */

#  ifndef JASL_cpp_constexpr
#    ifndef __cpp_constexpr
#      define JASL_cpp_constexpr                           \
        (_MSC_VER >= 1911L && _MSVC_LANG >= 201703L        \
             ? 201603L                                     \
             : (_MSC_VER >= 1910L && _MSVC_LANG >= 201402L \
                    ? 201304L                              \
                    : (_MSC_VER >= 1900L ? 200704L : 0L)))
#    else
#      error \
          "Compiler behaviour has been changed. Review the change! " ## #_MSC_VER
#    endif  // __cpp_constexpr
#  endif    // JASL_cpp_constexpr
#  ifndef JASL_cpp_lib_string_view
#    ifndef __cpp_lib_string_view
#      define JASL_cpp_lib_string_view                                       \
        (__cplusplus >= 201703 && _MSC_VER >= 1910L && _MSVC_LANG >= 201703L \
             ? 201606L                                                       \
             : 0L)
#    else
#      error \
          "Compiler behaviour has been changed. Review the change! " ## #_MSC_VER
#    endif  // __cpp_lib_string_view
#  endif    // JASL_cpp_lib_string_view

#else

#  if !defined(JASL_cpp_constexpr) || !defined(JASL_cpp_lib_string_view)
#    error \
        "Unsupported compiler! In case of an unsupported compiler JASL_cpp_constexpr and JASL_cpp_lib_string_view macros should be defined manually according to the used compiler (constexpr) and the used std library (std::string_view)."
#  endif

#endif  // <compiler types>

#if !JASL_cpp_constexpr
#  undef JASL_cpp_constexpr
#endif  // !JASL_cpp_constexpr

#if !JASL_cpp_lib_string_view
#  undef JASL_cpp_lib_string_view
#endif  // !JASL_cpp_lib_string_view

/*
 * http://en.cppreference.com/w/User:D41D8CD98F/feature_testing_macros
 * http://en.cppreference.com/w/cpp/language/constexpr
 */
#ifdef JASL_cpp_constexpr
#  if JASL_cpp_constexpr >= 201304L
#    define JASL_CONSTEXPR_CXX14 constexpr
#  elif JASL_cpp_constexpr >= 200704L
#    define JASL_CONSTEXPR_CXX14
#  else
// JASL_cpp_constexpr should not be defined.
#    error "Something wrong with the defines."
#  endif
#else  // ifdef JASL_cpp_constexpr
// There is no reason to support it without supporting noexcept.
#  error "Unsupported C++ standard!"
#endif  // ifdef JASL_cpp_constexpr

#ifdef __cpp_lib_is_swappable
#  define JASL_is_nothrow_swappable_value(type) \
    (std::is_nothrow_swappable<type>::value)
#else
// http://en.cppreference.com/w/cpp/algorithm/swap
#  define JASL_is_nothrow_swappable_value(type)         \
    (std::is_nothrow_move_constructible<type>::value && \
     std::is_nothrow_move_assignable<type>::value)
#endif
