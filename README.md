# ExecptCXX
[![ExceptCxx](https://github.com/voldien/exceptcxx/actions/workflows/ci.yml/badge.svg)](https://github.com/voldien/exceptcxx/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![GitHub release](https://img.shields.io/github/release/voldien/exceptcxx.svg)](https://github.com/voldien/exceptcxx/releases)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/voldien/exceptcxx.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/voldien/exceptcxx/context:cpp)

A simple library for adding universal exceptions more additional meta than the common _std::exception_ while preserving the original code when integrating.

## Features

* StackTrace - Support to get the stack tree of when it was invoked.
* Unicode - Allow support for both ASCII, Unicode 8, and unicode 16.
* Backward compadible with standard exception.

## Motivation

Attempt to create a dedicated Exception library with useful debug info and stack trace info while maintaining the standard c++ exception. Allowing to reuse the code in multiple projects with a good level of information when the exception is thrown.

## Installation

The library can be built simply by the following commands.

```bash
mkdir build
cd build
cmake ..
make
```

A note, this exception library is using external submodule git. Use the following command to download all the dependent git repositories.

```bash
git submodule update --init --recursive 
```

## Integration with CMake

The idea is to be able to integrate this library with another project easily. With CMake, it basically requires 2 lines. One for adding the project and the second for adding it as a dependent linked library target.

```cmake
ADD_SUBDIRECTORY(exceptCXX EXCLUDE_FROM_ALL)
```

```cmake
TARGET_LINK_LIBRARIES(myTarget PUBLIC cxxexcept)
```

## Dependencies

The dependices currently is related to backward-cpp

```bash
apt install binutils-dev
```

## Examples

The following is a simple example for throwing an exception, followed by printing a formated
error message to stdout.

```cpp
try {
	throw cxxexcept::DivideByZeroException();
} catch (const std::exception &ex) {
	cxxexcept::printStackMessage(ex);
}

```

Another example, using a more common exception type, runtime exception.

```cpp
try {
	throw cxxexcept::RuntimeException();
} catch (const std::exception &ex) {
	cxxexcept::printStackMessage(ex);
}

```

Getting a comprehensive string of both the stack as well the cause of exception can be extracted with the following method.

```cpp
std::cerr << cxxexcept::getStackMessage(ex);
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
