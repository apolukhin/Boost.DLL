// Copyright 2011-2012 Renato Tegon Forti.
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// -------------------------------------------------------------------------------------
// This example shows how to use load_self to load symbols direct on executable
// -------------------------------------------------------------------------------------

//[plugcpp_my_plugin_load_self

#define SUMEXE_PLUGIN_LINK_IN
#include "static_plugin.hpp"
#include <iostream>

namespace pl = boost::plugin;

int main() {
    std::cout << "Application started" << std::endl;
    
    pl::shared_library self;
    self.load_self();

    std::cout << "Call function" << std::endl;
    boost::function<boost::shared_ptr<my_plugin_api>()> creator 
        = pl::alias<boost::shared_ptr<my_plugin_api>()>(self, "create_plugin");

    std::cout << "Computed Value: " << creator()->calculate(2, 2) << std::endl;
}

//]
