# ExecptCXX

[![CxxExecpt](https://github.com/voldien/execptcxx/actions/workflows/linux-build.yml/badge.svg)](https://github.com/voldien/execptcxx/actions/workflows/linux-build.yml)
[![GitHub release](https://img.shields.io/github/release/voldien/execptcxx.svg)](https://github.com/voldien/execptcxx/releases)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/voldien/execptcxx.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/voldien/execptcxx/context:cpp)

A simple library for adding exception with additional meta than the common std::exception while preserving the code.

## Features

* StackTrace - Support to get the stack tree of when it was invoked.
* Unicode - Allow support for both ASCII, Unicode8 and unicode 16.

## Motivation

Attempt to create a dedicated Exception library with useful debug info and stack trace info while maintaining the standard c++ exception. Allowing to reuse the code in multiple projects with a good level of information when the exception is thrown.


# Installation
The library can be built simply by the following commands.

```bash
mkdir build
cd build
cmake ..
make
```


### Integration with CMake

The idea is to be able integrate this library with other project easily. With CMake, it basically requires 2 lines. On for adding the project and second for adding it as a dependent linked library target.

```cmake
ADD_SUBDIRECTORY(exceptCXX EXCLUDE_FROM_ALL)
```

```cmake
TARGET_LINK_LIBRARIES(myTarget PUBLIC cxxexcept)
```


## Dependices

binutils-dev


# Examples

```cpp
try {
	throw cxxexcept::DivideByZeroException();
} catch (cxxexcept::CaptureException &ex) {
	cxxexcept::printStackMessage(ex);
}

```
