// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org


//[plugcpp_tutorial7_library2
#include <iostream>
#include <boost/dll.hpp>

void print_howdy(const std::string& s) {
    std::cout << "How're you doing, " << s << '?' << std::endl;
}

void print_bored(const std::string& s) {
    std::cout << "Are you bored, " << s << '?' << std::endl;
}

BOOST_DLL_ALIAS_SECTIONED(print_howdy, howdy, Anna)
BOOST_DLL_ALIAS_SECTIONED(print_bored, are_you_bored, Anna)
//]
