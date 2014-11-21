// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "../shared_lib_path.hpp" // contains BJAM_LIBRARY_DECORATIONS macro to workaround --layout=X

//[callplugcpp_tutorial1
#include <boost/dll/import.hpp> // for import_alias
#include <iostream>
#include "../tutorial_common/plugin_api.hpp"

namespace dll = boost::dll;

int main(int argc, char* argv[]) { 
    std::cout << "Application started" << std::endl;
    /*<-*/ BOOST_ASSERT(argc >= 2);    /*->*/
    // argv[1] contains path to our plugin library
    boost::filesystem::path lib_path(argv[1]);
    boost::shared_ptr<my_plugin_api> plugin;            // variable to hold a pointer to plugin variable
    std::cout << "Loading the plugin" << std::endl;
     
    plugin = dll::import_alias<my_plugin_api>( // type of imported symbol is located between `<` and `>`
        lib_path / "my_plugin_sum"/*<-*/ BJAM_LIBRARY_DECORATIONS /*->*/,                     // path to the library and library name
        "plugin",                                       // name of the symbol to import
        dll::load_mode::append_decorations              // makes `libmy_plugin_sum.so` or `my_plugin_sum.dll` from `my_plugin_sum`
    );

    std::cout << "Plugin Version: " << plugin->version() << std::endl;
    std::cout << "Plugin Method:  " << plugin->calculate(1.5, 1.5) << std::endl;
}
//]
