Boost.Plugin
============

Library for comfortable work with DLL and DSO

### Caution

This is not yet an official Boost C++ library. It wasn't reviewed and can't be downloaded from www.boost.org. This beta is available to boost community to know real interest and get comments for refinement.The intention is to submit library to formal review, if community think that it is interesting!


### Docs
[Documentation of Boost.Plugin](http://apolukhin.github.io/Boost.Plugin/index.html)


### Hello World (common application and plug-in system)

```cpp
// mylib.so/.dll
//

#include <boost/config.hpp>
#include <iostream>

#define LIBRARY_API BOOST_SYMBOL_EXPORT

extern "C" void LIBRARY_API say_hello(void);
extern "C" float LIBRARY_API lib_version(void);
extern "C" int LIBRARY_API increment(int);

extern "C" int integer_g;
extern "C" const int const_integer_g = 777;

int integer_g = 100;

void say_hello(void)
{
   std::cout << "Hello, Boost.Application!" << std::endl;
}

float lib_version(void)
{
   return 1.0;
}

int increment(int n)
{
   return ++n;
}

// application
//

#include <boost/application.hpp>

using namespace boost::application;

class myapp
{
public:

   myapp(context& cxt)
      : context_(cxt) {}

   int operator()()
   {
      shared_library sl(
	     path->executable_path().string() + "/plugin/mylib" + shared_library::suffix());
     
	  // shared_library::suffix():
	  // the suffix of shared module, like:
      // 
      // .dll (windows)
      // .so (unix)
      // .dylib (mac)
	  
      boost::function<int(int)> inc = sl.get<int(int)>("increment");

      BOOST_CHECK(inc(1) == 2);
      BOOST_CHECK(inc(2) == 3);
      BOOST_CHECK(inc(3) == 4);

      return 0;
   }

private:
   context& context_;

};

// main

int main(int argc, char *argv[])
{  
   context app_context;
   myapp app(app_context);

   // add 'path' aspect to context pool of application, it will be available for future use
   app_context.insert<path>(
      boost::make_shared<path_default_behaviour>(argc, argv));
	 
   return launch<common>(app, app_context);
}

```
### About
This library was derived from the [Boost.Application](https://github.com/retf/Boost.Application) library.



### License
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/LICENSE_1_0.txt).
