// Copyright 2011-2012 Renato Tegon Forti.
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// -------------------------------------------------------------------------------------
// This example shows how to use load_self to load symbols direct on executable
// -------------------------------------------------------------------------------------

//[plugcpp_load_all
#include "../tutorial4/static_plugin.hpp"
#include <boost/make_shared.hpp>
#include <boost/container/map.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

namespace pl = boost::plugin;

class plugins_collector {
    typedef boost::container::map<std::string, pl::shared_library> plugins_t;

    boost::filesystem::path         plugins_directory_;
    plugins_t                       plugins_;

    void load_all() {
        namespace fs = ::boost::filesystem;
        typedef fs::path::string_type string_type;
        const string_type extension = pl::shared_library::suffix().native();        

        fs::directory_iterator endit;        
        for (fs::directory_iterator it(plugins_directory_); it != endit; ++it) {
            if (!fs::is_regular_file(*it)) {
                continue;
            }

            const string_type filename = it->path().filename().native();
            if (filename.find(extension) != filename.size() - extension.size()) {
                // Does not ends on '.so' or '.dll'
                continue;
            }

            // We have a plugin
            pl::shared_library plugin(it->path());      
            std::cout << "Loading " << it->path() << "\nNative:" << plugin.native() << '\n';
            insert_plugin(boost::move(plugin));
        }
        
        std::cout << "Loading self\n";
        pl::shared_library plugin;
        plugin.load_self();      
        insert_plugin(boost::move(plugin));
    }

    void insert_plugin(BOOST_RV_REF(pl::shared_library) lib) {
        std::string plugin_name;
        if (lib.search_symbol("create_plugin")) {
            plugin_name = pl::alias<boost::shared_ptr<my_plugin_api>()>(lib, "create_plugin")()->name();
        } else if (lib.search_symbol("plugin")) {
            plugin_name = pl::alias<my_plugin_api>(lib, "plugin").name();
        } else {
            return;
        }

        if (plugins_.find(plugin_name) == plugins_.cend()) {
            plugins_[plugin_name] = boost::move(lib);
        }
    }

public:
    plugins_collector(const boost::filesystem::path& plugins_directory)
        : plugins_directory_(plugins_directory)
    {
        load_all();
    }

    void print_plugins() const {
        plugins_t::const_iterator const end = plugins_.cend();
        for (plugins_t::const_iterator it = plugins_.cbegin(); it != end; ++it) {
            std::cout << it->first << ": " << it->second.native() << '\n';
        }
    }

    std::size_t count() const {
        return plugins_.size();
    }

    // ...   
};

int main(int argc, char* argv[]) {
    plugins_collector plugins(argv[1]);

    std::cout << "\n\nUnique plugins (" << plugins.count() << ") :\n";
    plugins.print_plugins();
}

//]
