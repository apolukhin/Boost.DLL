// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Copyright 2015 Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "../shared_lib_path.hpp" // contains BOOST_B2_LIBRARY_DECORATIONS macro to workaround --layout=X

//[callplugcpp_tutorial3
#include <boost/dll/import.hpp> // for import_alias
#include <boost/make_shared.hpp>
#include <iostream>
#include "../tutorial_common/my_plugin_api.hpp"

namespace dll = boost::dll;

std::size_t search_for_symbols(const std::vector<boost::filesystem::path>& plugins) {
    std::size_t plugins_found = 0;
    boost::shared_ptr<dll::shared_library> lib = boost::make_shared<dll::shared_library>();

    for (std::size_t i = 0; i < plugins.size(); ++i) {
        std::cout << "Loading plugin: " << plugins[i] << '\n';
        lib->load(plugins[i], dll::load_mode::append_decorations);
        if (!lib->has_symbol("create_plugin")) {
            // no such symbol
            continue;
        }

        // library has symbol, importing...
        typedef boost::shared_ptr<my_plugin_api> (pluginapi_create_t)();
        boost::function<pluginapi_create_t> creator
            = dll::import_alias<pluginapi_create_t>(lib, "create_plugin");

        std::cout << "Matching plugin name: " << creator()->name() << std::endl;
        ++ plugins_found;
    }

    return plugins_found;
}

//]

int main(int argc, char* argv[]) { 
    /*<-*/ BOOST_ASSERT(argc >= 2);    /*->*/
    // argv[1] contains path to our plugin library
    std::vector<boost::filesystem::path> plugins;
    boost::filesystem::path base_dir = argv[1];
    plugins.push_back(base_dir / "my_plugin_aggregator"/*<-*/ BOOST_B2_LIBRARY_DECORATIONS /*->*/);
    plugins.push_back(base_dir / "my_plugin_sum"/*<-*/ BOOST_B2_LIBRARY_DECORATIONS /*->*/);

    const std::size_t res = search_for_symbols(plugins);
    BOOST_ASSERT(res == 1);
}
