// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Copyright 2015 Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// MinGW related workaround
#define BOOST_DLL_FORCE_ALIAS_INSTANTIATION

#include <boost/dll.hpp>
#include <string>

//[getting_started_exports_c_function
extern "C" int BOOST_SYMBOL_EXPORT c_func_name(int);
//]

//[getting_started_exports_c_variable
extern "C" int BOOST_SYMBOL_EXPORT c_variable_name;
//]

//[getting_started_exports_cpp_function
namespace some_namespace {

    std::string cpp_function_name(const std::string& param);
    BOOST_DLL_AUTO_ALIAS(cpp_function_name)

}
//]

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
//[getting_started_exports_cpp11_function
namespace some_namespace {

    // C++ Standard does not affect semantics
    int cpp11_function(std::string&& param);
    BOOST_DLL_AUTO_ALIAS(cpp11_function)

}

//]
#endif

//[getting_started_exports_cpp_variable
namespace some_namespace {

    std::string cpp_variable_name = "some value";
    BOOST_DLL_AUTO_ALIAS(cpp_variable_name)

}
//]



int c_func_name(int i) { return ++i; }
int c_variable_name = 1;

namespace some_namespace {
    std::string cpp_function_name(const std::string& param) {
        return param + " Hello from lib!";
    }

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    int cpp11_function(std::string&& param) {
        return static_cast<int>(param.size());
    }
#endif
}

