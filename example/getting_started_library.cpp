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
extern "C" BOOST_SYMBOL_EXPORT int c_func_name(int);
//]

//[getting_started_exports_c_variable
extern "C" BOOST_SYMBOL_EXPORT int c_variable_name;
//]

//[getting_started_exports_cpp_function
namespace some_namespace {
    extern "C" BOOST_SYMBOL_EXPORT std::string i_am_cpp_function(const std::string& param);
}
//]

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
//[getting_started_exports_cpp11_function
namespace some_namespace {
    // C++ Standard does not affect semantics
    extern "C" BOOST_SYMBOL_EXPORT int cpp11_function(std::string&& param);
}

//]
#endif

//[getting_started_exports_cpp_variable
namespace some_namespace {
    extern "C" BOOST_SYMBOL_EXPORT std::string cpp_variable_name;
}
//]



int c_func_name(int i) { return ++i; }
int c_variable_name = 1;
std::string some_namespace::cpp_variable_name = "some value";

namespace some_namespace {
    std::string i_am_cpp_function(const std::string& param) {
        return param + " Hello from lib!";
    }

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    int cpp11_function(std::string&& param) {
        return static_cast<int>(param.size());
    }
#endif
}

