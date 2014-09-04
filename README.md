Boost Dynamic Library Load (Boost.DLL)
============

Library for comfortable work with DLL and DSO.

### Caution

This is not an official Boost C++ library. It wasn't reviewed and can't be downloaded from www.boost.org. This beta is available to boost community to know real interest and get comments for refinement. The intention is to submit library to formal review, if community think that it is interesting!


### Docs
[Documentation of Boost.DLL](http://apolukhin.github.io/Boost.Plugin/index.html)


### Hello World (importing a "C" function)

```cpp
#include <boost/dll.hpp>

...

boost::function<int(int)> c_func
    = boost::dll::import_function<int(int)>(
        path_to_shared_library, "c_func_name"
    );

```
### About
This library was derived from the [Boost.Application](https://github.com/retf/Boost.Application) library.



### License
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/LICENSE_1_0.txt).
