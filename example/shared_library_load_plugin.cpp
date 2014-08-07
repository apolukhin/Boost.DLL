// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// -----------------------------------------------------------------------------
// This example shows how to use shared_library class to load a plugin of a DSO.
// -----------------------------------------------------------------------------

#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

#include <iostream>
#include <boost/filesystem.hpp>
namespace fs = ::boost::filesystem;
fs::path get_shared_lib(const fs::path& root, const std::wstring& filename_part) {  
    fs::directory_iterator it(root);
    fs::directory_iterator endit;

    while (it != endit) {
        if (fs::is_regular_file(*it) && it->path().filename().wstring().find(filename_part) != std::wstring::npos) {
            return *it;
        }
        ++it;
    }

    throw std::runtime_error("Failed to find library");
}

//[callplugcpp
#include <boost/plugin.hpp>
#include "plugin_api.hpp"


int main(int argc, char* argv[]) { 
    // argv[1] contains path to our plugin library 
    BOOST_ASSERT(argc >= 2);
    boost::filesystem::path shared_library_path = get_shared_lib(argv[1], L"plugin_library");

    typedef boost::shared_ptr<my_plugin_api> (pluginapi_create_t)();
    boost::function<pluginapi_create_t> creator
        = boost::plugin::shared_function_alias<pluginapi_create_t>(shared_library_path, "create_plugin");

    boost::shared_ptr<my_plugin_api> plugin = creator();

    std::cout << "Plugin Version: " << plugin->version() << std::endl;
    std::cout << "Plugin Method:  " << plugin->calculate(1.5, 1.5) << std::endl;
}
//]
