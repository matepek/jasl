JASL (Just Another String Library)
==================================

[![Build Status](https://travis-ci.org/matepek/jasl.svg?branch=master)](https://travis-ci.org/matepek/jasl)
[![Build status](https://ci.appveyor.com/api/projects/status/jmfo82d6oumjpjxf/branch/master?svg=true)](https://ci.appveyor.com/project/matepek/jasl/branch/master)
[![Coverage Status](https://coveralls.io/repos/github/matepek/jasl/badge.svg)](https://coveralls.io/github/matepek/jasl)

[std::string_view]: http://en.cppreference.com/w/cpp/string/basic_string_view
[std::string]: http://en.cppreference.com/w/cpp/string/basic_string
[jasl::string]: include/string.hpp
[jasl::string_view]: include/string_view.hpp
[jasl::static_string]: include/static_string.hpp
[ninja]: https://github.com/ninja-build/ninja
[gn]: https://chromium.googlesource.com/chromium/src/+/master/tools/gn/README.md

This repository contains **C++** string classes with **static character array/C-string** support.

# Motivation

Sometimes we would like to use hardwired text resources. For example throwing an exception where the message is user specified. Just like in case of [std::runtime_error](http://en.cppreference.com/w/cpp/error/runtime_error) the message (_const char*_ parameter named *what_arg*) is "typically stored internally", which means the parameter will be copied. But usually users use it like `std::runtime_error("Coyote catched the Roadrunner!")` or `std::runtime_error(__FILE__)` where the *what_arg* is actually a a hardwired text. In this cases the copying seems unnecessary.

Creating your own exception and using this library's [jasl::static_string] class will eliminate the unnecessary copy in a type-safe way. For example the compiler will generate an error in case of the following situation:
```c++
jasl::static_string s("Bear trap was activated!"); // ok
s = "Bear trap was activated!"; // ok

// this is ok too, becasue []
const char exception_message[] = "Bear trap was activated!";
jasl::static_string s(exception_message); // ok
s = exception_message; // ok

const char* exception_message_ptr = "Bear trap was activated!";
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
 * clang, gcc or msvc
 * Minimum C++11:
   * _constexpr_ specifier
   * _noexcept_ specifier.
   * [std::hash](http://en.cppreference.com/w/cpp/utility/hash)
   * static_assert

# Installation

This is a header-only library. Add the ```include``` directory to your include directories.

Note: For building and running tests see [Tests, Test Coverage and Performance Tests](#tests-test-coverage-and-performance-tests)

# [Documentation](https://matepek.github.io/jasl/index.html)

This is a header-only library containing
 * [jasl::string_view],
 * [jasl::static_string],
 * [jasl::string]

classes.

Location: ```include``` directory.

## For documentation go [HERE](https://matepek.github.io/jasl/index.html)

### Pros and Cons comparing to [std::string]

Pros:
 * It can hold static string using less resources or it can hold a dynamically allocated string.
 * In case of static string:
    * No memory allocation -> no exception.
    * No memory copy.

Cons:
 * Only the underlying (string view) type's functionality can be used.

# Configuration

[See documentation here!](https://matepek.github.io/jasl)

[And here!](https://matepek.github.io/jasl/globals.html)

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

There is a ```build/build_helper.py``` script. If it is called with ```--install``` parameter, it will download the gn and ninja.

**Once again**: One doesn't have to deal with this. It is only for those who would like to see the test results themselves. (I've chosen [gn] because I wanted to understand and learn the technology a bit.)

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

[![Build Status](https://travis-ci.org/matepek/jasl.svg?branch=master)](https://travis-ci.org/matepek/jasl)
[![Build status](https://ci.appveyor.com/api/projects/status/jmfo82d6oumjpjxf/branch/master?svg=true)](https://ci.appveyor.com/project/matepek/jasl/branch/master)

Following the previous steps will also run the tests. The test binaries are in the out directory (example: out/default) if someone would like to rerun them.

Source location: ```test/*.test.cpp```

#### Semantic Error Tests

The project contains semantic error tests too. (For examples see section [Motivation](#motivation))
These tests are also run by the building process.

Source location: ```test/*.test_build_err.cpp.c```.

__Note__: This files have a _.c_ extension. This is how gn can differentiate between the building commands. It is necessary because we expect a building error in case of these files.

### Coverage

[![Coverage Status](https://coveralls.io/repos/github/matepek/jasl/badge.svg)](https://coveralls.io/github/matepek/jasl)

In case of ```is_generate_test_coverage=true``` gn argument the building will generate html code coverage reports. Works with clang.

Location of the test coverage results: ```out/<outdir>/*.coverage``` directories.

__Note__: The coverage report is not perfect.

For example in case of the [jasl::static_string]'s coverage report the [jasl::string_view] file's coverage is irrelevant.
Also because of some function is ```constexpr``` the compiler does its work and executes the code at compile time. In this case the method may seems uncovered.
Another problem with the coverage is that the library's classes contain templates and the the C++ compilers only instantiate those which are used, the unused ones are not instatiated so those won't be seen in the report as an uncovered code.

__Note__: Generating coverage report with gcc is also works, but it only generates raw data.

### Performance Tests

Performance tests are compiled by the building process, but it won't run them.

After building, the performance binaries are in the out directory and they are ready to be run.

Source location: ```test/*.performance.cpp```

#### (Generated) Performance Test Results

##### ./comparison_with_std_string.performance

JASL version: 1.0.0-beta (2018-07-19)

This performance test compares construction and destruction of
the jasl::static_string and the [jasl::string] to [std::string].

Parameters:
 - ( ItemCount, IterCount ) := ( 100000, 5 )
 - ( sizeof(short_string_array), sizeof(long_string_array) ) := ( 6, 150 )

   **Remarks**: The results depend on the size of the character array given to the std::string's constructor.

Hardware:
 - Processor (type, frequency): 2,6 GHz Intel Core i5
 - Memory: 8 GB 1600 MHz DDR3
 - Operating System: macOS 10.13.4

**NOTE**: Base type of the [jasl::static_string] and [jasl::string] is [jasl::string_view].

 - short [std::string] / short [jasl::static_string] = **208 %**
   - short [jasl::static_string]: 1.54476e-08 sec / unit
   - short [std::string]: 3.22507e-08 sec / unit

 - short [std::string] / short static [jasl::string] = **186 %**
   - short static [jasl::string]: 1.7339e-08 sec / unit
   - short [std::string]: 3.22507e-08 sec / unit

 - long [std::string] / long [jasl::static_string] = **4083 %**
   - long [jasl::static_string]: 1.29123e-08 sec / unit
   - long [std::string]: 5.27216e-07 sec / unit

 - long [std::string] / long static [jasl::string] = **3480 %**
   - long static [jasl::string]: 1.5146e-08 sec / unit
   - long [std::string]: 5.27216e-07 sec / unit

 - long [std::string] / long dynamic [jasl::string] = **199 %**
   - long dynamic [jasl::string]: 2.63628e-07 sec / unit
   - long [std::string]: 5.27216e-07 sec / unit

### Tested on

[Travis](https://travis-ci.org/matepek/jasl) and [Appveyor](https://ci.appveyor.com/api/projects/status/jmfo82d6oumjpjxf/branch/master?svg=true) build and run the tests with more compiler than the following list.

 * OSX 10.13.4 - Apple LLVM version 9.1.0 (clang-902.0.39.1)
 * Ubuntu 16.04 - g++ (Ubuntu 4.8.5-4ubuntu2) 4.8.5
 * Ubuntu 16.04 - g++ (Ubuntu 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609
 * Ubuntu 16.04 - clang version 6.0.1-svn329487-1~exp1~20180408093033.68 (branches/release_60)
 * MS Windows 10 - Microsoft (R) C/C++ Optimizing Compiler Version 19.13.26132 for x64
 * MS Windows 10 - clang version 6.0.1 & Microsoft (R) C/C++ Optimizing Compiler Version 19.13.26132 for x64

# Licence and third-parties

JASL library uses the [MIT](LICENSE) license.

# Thanks and Stuffs I used
 * https://github.com/Microsoft/vswhere

# TODOs
 * jasl::string_view could have more methods like [find_first, etc..](http://en.cppreference.com/w/cpp/header/string_view)
 * support for [literals](http://en.cppreference.com/w/cpp/language/user_literal)
 * std_string support (to_std_string, jasl string view compare operator std, ?)
 * constexpr hash
