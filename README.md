# ExecptCXX 
[![Actions Build Status](https://github.com/voldien/execptcxx/workflows/execptcxx/badge.svg?branch=master)](https://github.com/voldien/execptcxx/actions)
[![GitHub release](https://img.shields.io/github/release/voldien/execptcxx.svg)](https://github.com/voldien/execptcxx/releases)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/voldien/execptcxx.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/voldien/execptcxx/context:cpp)

A simple library for adding exception with additional features than the common std::exception while preserving the code.

A simple library of execptions

binutils-dev

## Features

* StackTrace
* Unicode
* 

## Motivation
Attempt to create a Exception library with useful debug info and stack trace info while maintining the code using the std::exception.


# Installation

```
mkdir build
cd build
cmake ..
make
```

### Integration with CMake

```cmake
ADD_SUBDIRECTORY(exceptCXX EXCLUDE_FROM_ALL)
```

```cmake
TARGET_LINK_LIBRARIES(myTarget PUBLIC cxxexcept)
```

# Examples

```
	try {
		throw cxxexcept::DivideByZeroException();
	} catch (cxxexcept::CaptureException &ex) {

		std::cout << ex.what();
		cxxexcept::printStackMessage(ex);
	}

```