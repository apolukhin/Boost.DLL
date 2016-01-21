// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Copyright 2015 Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

//[plugcpp_my_plugin_refcounting_api
#include "../tutorial_common/my_plugin_api.hpp"
#include <boost/filesystem/path.hpp>

class my_refcounting_api: public my_plugin_api {
public:
    // Returns path to shared object that holds a plugin.
    // Must be instantiated in plugin.
    virtual boost::filesystem::path location() const = 0;
};
//]


//[plugcpp_library_holding_deleter_api_bind
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/dll/shared_library.hpp>

inline boost::shared_ptr<my_refcounting_api> bind(my_refcounting_api* plugin) {
    using namespace boost;

    shared_ptr<dll::shared_library> lib = make_shared<dll::shared_library>( // `make_shared` is an efficient way to create shared pointer
        plugin->location()                                                  // getting location fo the shared library that holds the plugin
    );

    // `boost::shared_ptr` aliasing constructor.
    return shared_ptr<my_refcounting_api>(lib, plugin);
}
//]

//[plugcpp_get_plugin_refcounting
#include <boost/dll/import.hpp>
#include <boost/function.hpp>
inline boost::shared_ptr<my_refcounting_api> get_plugin(
    boost::filesystem::path path, const char* func_name)
{
    typedef my_refcounting_api*(func_t)();
    boost::function<func_t> creator = boost::dll::import_alias<func_t>(
        path, 
        func_name, 
        boost::dll::load_mode::append_decorations   // will be ignored for executable
    );

    // `plugin` does not hold a reference to shared library. If `creator` will go out of scope, 
    // then `plugin` can not be used.
    my_refcounting_api* plugin = creator();

    // Returned variable holds a reference to 
    // shared_library and it is safe to use it.
    return bind( plugin );

    // `creator` goes out of scope here and will be destroyed.
}

//]


