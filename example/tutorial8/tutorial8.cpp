// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// -------------------------------------------------------------------------------------
// This example shows how to import_variable_alias function to load a plugin of a DSO.
// -------------------------------------------------------------------------------------

#include "../shared_lib_path.hpp"

//[callplugcpp_tutorial8
#include <boost/dll.hpp>
#include <iostream>
#include "../tutorial_common/plugin_api.hpp"
#include <boost/thread.hpp>

int main(int argc, char* argv[]) { 
    {
        // argv[1] contains path to our plugin library 
        BOOST_ASSERT(argc >= 2);
        typedef boost::shared_ptr<my_plugin_api>(func_t)();

        boost::shared_ptr<my_plugin_api> plugin_instance;

        {
            boost::function<func_t> f = boost::dll::import_function_alias<func_t>(
                shared_lib_path(argv[1], L"refcounting_plugin"), "create_refc_plugin"
            );


            std::cout << "location() " 
                      << boost::dll::shared_library(shared_lib_path(argv[1], L"refcounting_plugin")).location()
                      << std::endl;

            plugin_instance = f();

            // `f` goes out of scope here and will be destroyed.
            // `plugin_instance` holds a reference to shared_library and it is safe to
            // use it.
        }

        std::cout << "Plugin name: " << plugin_instance->name() << std::endl;
        plugin_instance.reset();
        std::cout << "released" << std::endl;
    }

    boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
    std::cout << "after exit" << std::endl;
}
