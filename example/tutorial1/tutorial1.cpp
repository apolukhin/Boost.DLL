// Copyright 2011-2012 Renato Tegon Forti.
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// -------------------------------------------------------------------------------------
// This example shows how to shared_variable_alias function to load a plugin of a DSO.
// -------------------------------------------------------------------------------------

#include "../shared_lib_path.hpp"

//[callplugcpp_tutorial1
#include <boost/plugin.hpp>
#include <iostream>
#include "../tutorial_common/plugin_api.hpp"

int main(int argc, char* argv[]) { 
    std::cout << "Application started" << std::endl;

    // argv[1] contains path to our plugin library 
    BOOST_ASSERT(argc >= 2);
    boost::filesystem::path shared_library_path = shared_lib_path(argv[1], L"my_plugin_sum");

    std::cout << "Loading the plugin" << std::endl;
    boost::shared_ptr<my_plugin_api> plugin
        = boost::dll::shared_variable_alias<my_plugin_api>(
            shared_library_path, "plugin"
        );

    std::cout << "Plugin Version: " << plugin->version() << std::endl;
    std::cout << "Plugin Method:  " << plugin->calculate(1.5, 1.5) << std::endl;
}
//]
