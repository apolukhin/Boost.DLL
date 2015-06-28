// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "../shared_lib_path.hpp" // contains BOOST_B2_LIBRARY_DECORATIONS macro to workaround --layout=X

//[callplugcpp_tutorial8
#include <iostream>
#include "refcounting_api.hpp"

int main(int argc, char* argv[]) { 
    //<-
    // This block is invisible for Quickbook documentation
    // argv[1] contains path to our plugin library 
    BOOST_ASSERT(argc >= 2);
    //->
    boost::shared_ptr<my_refcounting_api> plugin = get_plugin(
        boost::filesystem::path(argv[1]) / "refcounting_plugin"/*<-*/ BOOST_B2_LIBRARY_DECORATIONS /*->*/,
        "create_refc_plugin"
    );

    std::cout << "Plugin name: " << plugin->name() 
              << ", \nlocation: " << plugin->location() 
              << std::endl;
}
//]
