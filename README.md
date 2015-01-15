Boost Dynamic Library Load (Boost.DLL)
============

Library for comfortable work with DLL and DSO.

### Docs
[Documentation of Boost.DLL](http://apolukhin.github.io/Boost.DLL/index.html)

### Hello World (importing a "C" function)

```cpp
#include <boost/dll.hpp>

...
auto cpp11_func = dll::import_alias<std::string(int)>(
        path_to_shared_library, "cpp11_function_alias_name"
    );
std::string s = cpp11_func(0);


boost::function<int(int)> c_func = boost::dll::import<int(int)>(
        path_to_shared_library, "c_func_name"
    );
int i = c_func(1);
```

### Installation

* Boost 1.56 and later:
    * enter the Boost root directory: `cd ~/boost_1_56_0/`
    * go to `libs/` folder: `cd libs`
    * clone Boost.DLL repository: `git clone https://github.com/apolukhin/Boost.DLL dll`
    * enter the `dll/test` folder and run `./b2`: `cd dll/test; ../../../b2`


* Boost 1.53 -  Boost 1.55:
    * enter the Boost root directory: `cd ~/boost_1_53_0/`
    * go to `libs/` folder: `cd libs`
    * clone Boost.DLL repository: `git clone https://github.com/apolukhin/Boost.DLL dll`
    * clone the Boost.Predef repository: `git clone https://github.com/boostorg/predef`
    * copy headers from `dll/include/boost` and `predef/include/boost` to the boost folder with headers:
        `cp -rf dll/include/boost/* ~/boost_1_53_0/boost/; cp -rf predef/include/boost/* ~/boost_1_53_0/boost/;`

### Platforms tested

Library was tested on Windows (MinGW and MSVC compilers) and Linux(GCC and CLANG compilers).  MacOS is upcoming.

### Caution
This is not an official Boost C++ library. It wasn't reviewed and can't be downloaded from www.boost.org. This beta is available to boost community to know real interest and get comments for refinement. The intention is to submit library to formal review, if community think that it is interesting!

### About
This library was derived from the [Boost.Application](https://github.com/retf/Boost.Application) library.

### Test results

Branches        | Build         | Tests coverage
----------------|-------------- | -------------
Develop:        | [![Build Status](https://travis-ci.org/apolukhin/Boost.DLL.svg?branch=develop)](https://travis-ci.org/apolukhin/Boost.DLL)  | [![Coverage Status](https://coveralls.io/repos/apolukhin/Boost.DLL/badge.png?branch=develop)](https://coveralls.io/r/apolukhin/Boost.DLL?branch=develop)
Master:         | [![Build Status](https://travis-ci.org/apolukhin/Boost.DLL.svg?branch=master)](https://travis-ci.org/apolukhin/Boost.DLL)  | [![Coverage Status](https://coveralls.io/repos/apolukhin/Boost.DLL/badge.png?branch=master)](https://coveralls.io/r/apolukhin/Boost.DLL?branch=master)

Static analisis (Coverity): [![Coverity Scan Build Status](https://scan.coverity.com/projects/3911/badge.svg)](https://scan.coverity.com/projects/3911/view_defects)

### License
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/LICENSE_1_0.txt).
