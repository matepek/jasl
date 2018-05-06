JASL (Just Another String Library)
==================================

[![Build Status](https://travis-ci.org/matepek/jasl.svg?branch=master)](https://travis-ci.org/matepek/jasl)

[std::string_view]: http://en.cppreference.com/w/cpp/string/basic_string_view
[std::string]: http://en.cppreference.com/w/cpp/string/basic_string
[jasl::string]: include/jasl_string.hpp
[jasl::string_view]: include/jasl_string_view.hpp
[jasl::static_string]: include/jasl_static_string.hpp
[ninja]: https://github.com/ninja-build/ninja
[gn]: https://chromium.googlesource.com/chromium/src/+/master/tools/gn/README.md

This repository contains **C++** string classes with **static character array/C-string** support.

# Motivation

Sometimes we would like to use hardwired text resources. For example throwing an exception where the message is user specified. Just like in case of [std::runtime_error](http://en.cppreference.com/w/cpp/error/runtime_error) the message (_const char*_ parameter named *what_arg*) is "typically stored internally", which means the parameter will be copied. But usually users use it like `std::runtime_error("Coyote catched the Roadrunner!")` or `std::runtime_error(__FILE__)` where the *what_arg* is actually a a hardwired text. In this cases the copying seems unnecessary.

Creating your own exception and using this library's [jasl::static_string] class will eliminate the unnecessary copy in a type-safe way. For example the compiler will generate an error in case of the following situation:
```c++
const char exception_message[] = "Bear trap was activated!";
jasl::static_string s(exception_message); // ok
s = exception_message; // ok

const char* exception_message_ptr = exception_message;
jasl::static_string s(exception_message_ptr); // compilation error!!
s = exception_message_ptr;  // compilation error!!
```

Continuing the previous example: What if we occasionally want to use the dynamic string, but in the majority of the cases we would like to use static string. The [jasl::string] class supposed to give a solution for this situation. Check the example:
```c++
const char exception_message[] = "Bear trap was activated!";
jasl::string s(exception_message); // ok: A pointer to the original charater array is copied.
s = exception_message; // ok: A pointer to the original charater array is copied.
s.assign(exception_message); // ok: A pointer to the original charater array is copied.

const char* exception_message_ptr = exception_message;
jasl::string s(exception_message_ptr); // ok: Memory is allocated and the message is copied.
s = exception_message_ptr; // compilation error: can't do this, use assign instead!
s.assign(exception_message_ptr); // ok: Memory is allocated and the message is copied.
```

# Requirements

 * [C++ standard library](http://en.cppreference.com/w/cpp/header)
 * Minimum C++11 (Reason: Mainly because of _constexpr_ and _noexcept_ specifier. It is not necessary, but this could be easily fixed.)

# Installation

This is a header-only library. Add the ```include``` directory to your include directories.

Note: For building and running tests see [Tests, Test Coverage and Performance Tests](#tests-test-coverage-and-performance-tests)

# Documentation

This is a header-only library containing
 * [jasl::string_view],
 * [jasl::static_string],
 * [jasl::string]

classes.

Location: ```include``` directory.

C++17 introduced [std::string_view], and this library uses it as a public base class of the [jasl::static_string] and the [jasl::string] classes. In this way all the methods are accessible. Some of the methods are special and because of this they are overwritten.

## Class [jasl::string_view]

This class is an optinal substitution of the [std::string_view] for those who have no C++17. To use this as the base class, define ```JASL_USE_JASL_STRING_VIEW_TYPE_AS_BASE```.

The class only has basic functionality. (Please open an issue or endorse an existing if you would like to add some functionality.)

## Class [jasl::static_string]

This class derives from [std::string_view] (or optionally from [jasl::string_view]). What is special about it is that you can only assign (or construct with) character arrays to it but not pointers. This makes it safer comparing to string_view-s.
See [Motivation](#motivation) section for examples.
See [Configuration](#configuration) section for more details.

Remarks:
Why is there no c_str method? While ```jasl::static_string("one")``` is null terminated, ```jasl::static_string({'t', 'w', 'o'})``` is not. In both cases the size()/length() of them will be 3, so the terminating null character will be here in the first case but it won't be counted. If you want the null terminator as a part of it, write ```jasl::static_string("one\0")``` or ```jasl::static_string({'t', 'w', 'o', '\0', '\0'})```.

## Class [jasl::string]

This class derives from [std::string_view] (or optionally from [jasl::string_view]). What is special about it is that if you assign (or construct with) character array to it, it won't allocate, copy and deallocate. In other cases it will allocate, copy and deallocate.
See [Motivation](#motivation) section for examples.
See [Configuration](#configuration) section for more details.

It has the functionality of its base class ([std::string_view]) only, which means that the content is immutable. This was the convenient implementation because the static string is immutable anyway.

### Pros and Cons comparing to [std::string]

Pros:
 * It can hold static string using less resources or it can hold a dynamically allocated string.
 * In case of static string:
    * No memory allocation -> no exception.
    * No memory copy.

Cons:
 * The container is immutable.
 * Only the underlying (string view) type's functionality can be used.

# Configuration

##### ```JASL_USE_JASL_STRING_VIEW_TYPE_AS_BASE```

If you have no [std::string_view], ```JASL_USE_JASL_STRING_VIEW_TYPE_AS_BASE``` will be defined automatically and [jasl::string_view] will be used.

If you just want to use [jasl::string_view] (in case of C++17 too), define ```JASL_USE_JASL_STRING_VIEW_TYPE_AS_BASE```.
About jasl::string_view: [here](#class-jaslstring_view).

##### ```JASL_ABORT_ON_EXCEPTION_ON```

There are a few functions which can throw. (The design reason is the similarity to __std__.)
The exceptions are easily avoidable. Defining this macro will result of a call of [std::abort](http://en.cppreference.com/w/cpp/utility/program/abort) instead of throwing an exception.

__Note__: If this is defined, the tests will abort when they test the exceptions.

##### ```JASL_ASSERT_ON```

This macro is recommended to be defined during development and testing. It helps identify unproper use of the library.

# Changelog

[Here: CHANGELOG.md](CHANGELOG.md)

# Correctness

>Evaluating a string-literal results in a string literal object with static storage duration, initialized from the given characters as specified above.
[Source](https://stackoverflow.com/questions/32872465/const-char-value-lifetime)

Note: [cppreference - literal types](http://en.cppreference.com/w/cpp/concept/LiteralType)

# Tests, Test Coverage and Performance Tests

## Requirements

 * [gn]: This tool is developed by the chromium project. [Download it!](https://refi64.com/gn-builds/) or build yourself.
 * [ninja]: A pretty fast build tool. [Install it](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages) or [download](https://github.com/ninja-build/ninja/releases)
 * [python](https://www.python.org/downloads/): It is required by [gn]. Note: Make sure your python directory is contained by the ```PATH``` environment variable.

Once again: One doesn't have to deal with this. It is only for those who would like to see the test results themselves. (I've chosen [gn] because I wanted to understand and learn the technology a bit.)

## Building, testing and generating coverage report

After [gn] and [ninja] is installed it is easy (hopefully).
Typically the output directory is inside the ```out``` directory (which is part of the repo, but empty now). From command line from the project's root directory:

```bash
gn gen out/default
```

This will create a directory inside ```out``` and generate the [ninja] files.
It's time to build:

```bash
ninja -C out/default
```

And we are done. This built and run the tests too.

There is a more build options for example:
 * std_version
 * is_asan
 * is_generate_test_coverage
 * compiler_type

These can be set by calling
```bash
gn args out/default
```
and then editing the file which was opened.

For detailed options:
```bash
gn args out/default --list
```

### Tests

Following the previous steps will also run the tests. The test binaries are in the out directory (example: out/default) if someone would like to rerun them.

Source location: ```test/*.test.cpp```

#### Semantic Error Tests

The project contains semantic error tests too. (For examples see section [Motivation](#motivation))
These tests are also run by the building process.

Source location: ```test/*.test_build_err.cpp.c```.

__Note__: This files have a _.c_ extension. This is how gn can differentiate between the building commands. It is necessary because we expect a building error in case of these files.

### Coverage

In case of ```is_generate_test_coverage=true``` gn argument the building will generate html code coverage reports. This is a clang-only feature.

Location of the test coverage results: ```out/<outdir>/*.coverage``` directories.

__Note__: The coverage report is not perfect.

For example in case of the [jasl::static_string]'s coverage report the [jasl::string_view] file's coverage is irrelevant. Also because of some function is ```constexpr``` and contains template the coverage report shows them false-negative.

### Performance Tests

Performance tests are compiled by the building process, but it won't run them. Those results would be irrelevant anyway because the building uses a lot of resources.

After building, the performance binaries are in the out directory and they are ready to run.

Source location: ```performance/*.performance.cpp```

### Tested on

 * OSX 10.13.4 - Apple LLVM version 9.1.0 (clang-902.0.39.1)
 * Ubuntu 16.04 - g++ (Ubuntu 4.8.5-4ubuntu2) 4.8.5
 * Ubuntu 16.04 - g++ (Ubuntu 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609
 * Ubuntu 16.04 - clang version 6.0.1-svn329487-1~exp1~20180408093033.68 (branches/release_60)
 * MS Windows 10 - Microsoft (R) C/C++ Optimizing Compiler Version 19.13.26132 for x64

# Licence

JASL library uses the [MIT](LICENSE) license.

# Thanks and Stuffs I used
 * https://github.com/Microsoft/vswhere

# TODO
 * lsan, msan
 * better hash for jasl::string_view
 * jasl::string_view could have more methods like [find_first, etc..](http://en.cppreference.com/w/cpp/header/string_view)
 * smart_string(shared_ptr and appending)
 * support for [literals](http://en.cppreference.com/w/cpp/language/user_literal)
 * document jasl assert and jasl exception macros
 * macro stacking?
 * std_string support (to_std_string, ?)
 * ngg comments
 * shield.io and releases
