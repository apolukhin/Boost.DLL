// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "../shared_lib_path.hpp"

//[callplugcpp_tutorial2
#include <boost/dll/import_function.hpp> // for import_function_alias
#include <iostream>
#include "../tutorial_common/plugin_api.hpp"

int main(int argc, char* argv[]) { 
    // argv[1] contains path to our plugin library 
    BOOST_ASSERT(argc >= 2);
    boost::filesystem::path shared_library_path = shared_lib_path(argv[1], L"my_plugin_aggregator");

    typedef boost::shared_ptr<my_plugin_api> (pluginapi_create_t)();
    boost::function<pluginapi_create_t> creator
        = boost::dll::import_function_alias<pluginapi_create_t>(shared_library_path, "create_plugin");

    boost::shared_ptr<my_plugin_api> plugin = creator();

    std::cout << "Plugin Version: " << plugin->version() << std::endl;
    std::cout << "Plugin Method:  " << plugin->calculate(1.5, 1.5) << std::endl;
    std::cout << "Plugin Method second call:  " << plugin->calculate(1.5, 1.5) << std::endl;
    std::cout << "Plugin Name:  " << plugin->name() << std::endl;
}
//]
