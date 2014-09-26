// Copyright 2011-2012 Renato Tegon Forti.
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// -------------------------------------------------------------------------------------
// This example shows how to use load_self to load symbols direct on executable
// -------------------------------------------------------------------------------------


#include "../tutorial4/static_plugin.hpp"
#include <boost/make_shared.hpp>
#include <boost/container/map.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

//[plugcpp_plugins_collector_def
namespace dll = boost::dll;

class plugins_collector {
    // Name => plugin
    typedef boost::container::map<std::string, dll::shared_library> plugins_t;

    boost::filesystem::path         plugins_directory_;
    plugins_t                       plugins_;

    // loads all plugins in plugins_directory_
    void load_all();

    // Gets `my_plugin_api` instance using "create_plugin" or "plugin" imports,
    // stores plugin with its name in the `plugins_` map.
    void insert_plugin(BOOST_RV_REF(dll::shared_library) lib);

public:
    plugins_collector(const boost::filesystem::path& plugins_directory)
        : plugins_directory_(plugins_directory)
    {
        load_all();
    }

    void print_plugins() const;

    std::size_t count() const;
    // ...   
};
//]


//[plugcpp_plugins_collector_load_all
void plugins_collector::load_all() {
    namespace fs = ::boost::filesystem;
    typedef fs::path::string_type string_type;
    const string_type extension = dll::shared_library::suffix().native();        

    // Searching a folder for files with '.so' or '.dll' extension
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

        // We found a file. Trying to load it
        dll::shared_library plugin(it->path());      
        std::cout << "Loaded (" << plugin.native() << "):" << it->path() << '\n';

        // Gets plugin using "create_plugin" or "plugin" function
        insert_plugin(boost::move(plugin));
    }
    
    dll::shared_library plugin(dll::program_location());
    std::cout << "Loaded self\n";
    insert_plugin(boost::move(plugin));
}
//]

//[plugcpp_plugins_collector_insert_plugin
void plugins_collector::insert_plugin(BOOST_RV_REF(dll::shared_library) lib) {
    std::string plugin_name;
    if (lib.search_symbol("create_plugin")) {
        plugin_name = lib.get_alias<boost::shared_ptr<my_plugin_api>()>("create_plugin")()->name();
    } else if (lib.search_symbol("plugin")) {
        plugin_name = lib.get_alias<my_plugin_api>("plugin").name();
    } else {
        return;
    }

    if (plugins_.find(plugin_name) == plugins_.cend()) {
        plugins_[plugin_name] = boost::move(lib);
    }
}
//]

void plugins_collector::print_plugins() const {
    plugins_t::const_iterator const end = plugins_.cend();
    for (plugins_t::const_iterator it = plugins_.cbegin(); it != end; ++it) {
        std::cout << '(' << it->second.native() << "): " << it->first << '\n';
    }
}

std::size_t plugins_collector::count() const {
    return plugins_.size();
}


//[plugcpp_load_all
int main(int argc, char* argv[]) {
    BOOST_ASSERT(argc == 2);
    plugins_collector plugins(argv[1]);

    std::cout << "\n\nUnique plugins " << plugins.count() << ":\n";
    plugins.print_plugins();
    // ...
//]
    BOOST_ASSERT(plugins.count() == 3);
}

