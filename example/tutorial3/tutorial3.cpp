// Copyright 2011-2012 Renato Tegon Forti.
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// -------------------------------------------------------------------------------------
// This example shows how to import_variable_alias function to load a plugin of a DSO.
// -------------------------------------------------------------------------------------

#include "../shared_lib_path.hpp"

//[callplugcpp_tutorial3

#include <boost/dll.hpp>
#include <boost/make_shared.hpp>

#include <iostream>

#include "../tutorial_common/plugin_api.hpp"

namespace dll = boost::dll;

std::size_t search_for_symbols(const std::vector<boost::filesystem::path>& plugins) {
    std::size_t plugins_found = 0;
    boost::shared_ptr<dll::shared_library> lib = boost::make_shared<dll::shared_library>();

    for (std::size_t i = 0; i < plugins.size(); ++i) {
        std::cout << "Loading plugin: " << plugins[i] << '\n';
        lib->load(plugins[i]);
        if (!lib->search_symbol("create_plugin")) {
            // no such symbol
            continue;
        }

        // library has symbol, importing...
        typedef boost::shared_ptr<my_plugin_api> (pluginapi_create_t)();
        boost::function<pluginapi_create_t> creator
            = dll::import_function_alias<pluginapi_create_t>(lib, "create_plugin");

        std::cout << "Matching plugin name: " << creator()->name() << std::endl;
        ++ plugins_found;
    }

    return plugins_found;
}

//]

int main(int argc, char* argv[]) { 
    // argv[1] contains path to our plugin library 
    BOOST_ASSERT(argc >= 2);
    std::vector<boost::filesystem::path> plugins;
    plugins.push_back(shared_lib_path(argv[1], L"my_plugin_aggregator"));
    plugins.push_back(shared_lib_path(argv[1], L"my_plugin_sum"));

    const std::size_t res = search_for_symbols(plugins);
    BOOST_ASSERT(res == 1);
}
