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
#include <boost/plugin/shared_library.hpp>
#include "plugin_api.hpp"

using namespace boost;

typedef my_plugin_api* (pluginapi_create) (void);
typedef void (pluginapi_delete) (my_plugin_api* myplugin);

int main(int argc, char* argv[])
{ 
    // argv[1] must contain plugin path
    BOOST_ASSERT(argc >= 2);
    boost::filesystem::path shared_library_path = get_shared_lib(argv[1], L"plugin_library");

    application::shared_library sl(shared_library_path);

    if (!sl.search_symbol("create_my_plugin")) {
        return 2;
    }

    my_plugin_api* plugin = (sl.get<pluginapi_create>("create_my_plugin"))();

    if (plugin == NULL) {
        return 3;
    }

    std::cout << "Plugin Version: " << plugin->version() << std::endl;
    std::cout << "Plugin Method:  " << plugin->calculate(1.5, 1.5) << std::endl;

    (sl.get<pluginapi_delete>("delete_my_plugin"))(plugin);
    return 0;
}
//]
