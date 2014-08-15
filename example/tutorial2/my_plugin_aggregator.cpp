// Copyright 2011-2013 Renato Tegon Forti.
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <iostream>
#include <boost/make_shared.hpp>

//[plugcpp_my_plugin_aggregator
   
#include "../tutorial_common/plugin_api.hpp"
#include <boost/plugin.hpp>

namespace my_namespace {

class my_plugin_aggregator : public my_plugin_api {
    float aggr_;
public:
    my_plugin_aggregator() : aggr_(0) {}

    float version() const {
        return 1.0;
    }

    std::string name() const {
        return "aggregator";
    }

    float calculate(float x, float y) {
        aggr_ += x + y;
        return aggr_;
    }
};

boost::shared_ptr<my_plugin_aggregator> create_plugin_aggregator() {
    return boost::make_shared<my_plugin_aggregator>();
}

BOOST_PLUGIN_ALIAS(my_namespace::create_plugin_aggregator, create_plugin)

} // namespace my_namespace

//]



