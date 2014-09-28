// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

//[plugcpp_my_plugin_refcounting
#include "refcounting_plugin.hpp"
#include <boost/dll/runtime_symbol_info.hpp> // for this_line_location()

namespace my_namespace {

class my_plugin_refcounting : public my_refcounting_api {
public:
    float version() const {
        return 1.0;
    }

    std::string name() const {
        return "refcounting";
    }

    float calculate(float x, float y) {
        return 0;
    }

    // Factory method. Returns *simple pointer*
    static my_plugin_refcounting* create() {
        return new my_plugin_refcounting();
    }
    
    // Must be instantiated in plugin
    boost::filesystem::path location() const {
        return boost::dll::this_line_location(); // location of this plugin
    }
};

} // namespace my_namespace

my_refcounting_api* create() {
    return new my_namespace::my_plugin_refcounting();
}

//]



