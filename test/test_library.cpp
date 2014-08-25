// Copyright 2011-2012 Renato Tegon Forti
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/config.hpp>
#include <boost/plugin/alias.hpp>
#include <iostream>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/fusion/container.hpp>

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

    std::size_t variable = 42;
}


// Make sure that aliases have no problems with memory allocations and different types of input parameters
namespace namespace1 { namespace namespace2 { namespace namespace3 {
    typedef
        boost::fusion::vector<std::vector<int>, std::vector<int>, std::vector<int>, const std::vector<int>*, std::vector<int>* >
    do_share_res_t;

    boost::shared_ptr<do_share_res_t> do_share(
            std::vector<int> v1,
            std::vector<int>& v2,
            const std::vector<int>& v3,
            const std::vector<int>* v4,
            std::vector<int>* v5
        )
    {
        v2.back() = 777;
        v5->back() = 9990;
        return boost::make_shared<do_share_res_t>(v1, v2, v3, v4, v5);
    }

    std::string info("I am a std::string from the test_library (Think of me as of 'Hello world'. Long 'Hello world').");
}}}


BOOST_PLUGIN_ALIAS(foo::bar, foo_bar)
BOOST_PLUGIN_ALIAS(foo::variable, foo_variable)
BOOST_PLUGIN_ALIAS(namespace1::namespace2::namespace3::do_share, do_share)
BOOST_PLUGIN_ALIAS(namespace1::namespace2::namespace3::info, info)
BOOST_PLUGIN_ALIAS(const_integer_g, const_integer_g_alias)


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







