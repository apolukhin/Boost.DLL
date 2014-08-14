// Copyright 2011-2012 Renato Tegon Forti
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org


//[plugcpp_my_plugin_staic_impl
#include "static_plugin.hpp"

#include <boost/make_shared.hpp>
#include <iostream>

namespace my_namespace {

class my_plugin_static : public my_plugin_api {
public:
    my_plugin_static() {
        std::cout << "Constructing my_plugin_static" << std::endl;
    }
   
    ~my_plugin_static() {
        std::cout << "Destructing my_plugin_static" << std::endl;
    }

    float version() const {
        return 1.0;
    }

    std::string name() const {
        return "static";
    }

    float calculate(float x, float y) {
        return x - y;
    }
};

boost::shared_ptr<my_plugin_api> create_plugin() {
    return boost::make_shared<my_plugin_static>();
}

} // namespace my_namespace

//]

