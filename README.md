Boost Dynamic Library Load (Boost.DLL)
============

Library for comfortable work with DLL and DSO.

### Docs
[Documentation of Boost.DLL](http://apolukhin.github.io/Boost.DLL/index.html)

### Hello World (importing a "C" function)

```cpp
#include <boost/dll.hpp>

...

boost::function<int(int)> c_func
    = boost::dll::import_function<int(int)>(
        path_to_shared_library, "c_func_name"
    );

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

Library was tested on Windows and Linux (MacOS is upcomming) with GCC, Clang and MSVC compilers.

### Caution
This is not an official Boost C++ library. It wasn't reviewed and can't be downloaded from www.boost.org. This beta is available to boost community to know real interest and get comments for refinement. The intention is to submit library to formal review, if community think that it is interesting!

### About
This library was derived from the [Boost.Application](https://github.com/retf/Boost.Application) library.



### License
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/LICENSE_1_0.txt).
