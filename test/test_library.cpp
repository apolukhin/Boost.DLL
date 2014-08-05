// Copyright 2011-2012 Renato Tegon Forti
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/config.hpp>
#include <iostream>
#include <vector>

#define LIBRARY_API BOOST_SYMBOL_EXPORT

extern "C" void LIBRARY_API say_hello(void);
extern "C" float LIBRARY_API lib_version(void);
extern "C" int LIBRARY_API increment(int);

extern "C" int LIBRARY_API integer_g;
extern "C" const int LIBRARY_API const_integer_g = 777;

namespace foo {
    std::size_t bar(const std::vector<int>& v) {
        return v.size();
    }
}

extern "C" LIBRARY_API void alias() {
    reinterpret_cast<void (*)()>(foo::bar)();
}


namespace foo {
    extern "C" std::size_t variable;
    std::size_t variable = 42;
}

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

