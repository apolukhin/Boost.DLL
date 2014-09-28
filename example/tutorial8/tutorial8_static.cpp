// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

//[callplugcpp_tutorial8_static
#include <boost/dll/import_function.hpp>
#include <boost/dll/runtime_symbol_info.hpp> // for program_location()
#include <iostream>
#include "refcounting_plugin.hpp"

int main() {
    typedef my_refcounting_api*(func_t)();

    boost::shared_ptr<my_refcounting_api> plugin_instance;
    {
        boost::function<func_t> f = boost::dll::import_function_alias<func_t>(
            boost::dll::program_location(), "create_refc_plugin"
        );

        plugin_instance = adopt( f() );

        // `f` goes out of scope here and will be destroyed.
        // `plugin_instance` holds a reference to shared_library and it is safe to
        // use it.
    }

    std::cout << "Plugin name: " << plugin_instance->name() << ", location: " << plugin_instance->location() << std::endl;
}
//]
