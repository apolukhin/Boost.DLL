// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

//[plugcpp_my_plugin_refcounting_api
#include "../tutorial_common/plugin_api.hpp"
#include <boost/filesystem/path.hpp>

class my_refcounting_api : public my_plugin_api {
public:
    // Must be instantiated in plugin
    virtual boost::filesystem::path location() const = 0;
};
//]


//[plugcpp_library_holding_deleter_api
#include <boost/dll/shared_library.hpp>
#include <boost/make_shared.hpp>

namespace detail {
    struct library_holding_deleter {
        boost::shared_ptr<boost::dll::shared_library> lib_;

        library_holding_deleter(my_refcounting_api* ptr)
            : lib_(boost::make_shared<boost::dll::shared_library>(ptr->location()))
        {}  

        void operator()(my_refcounting_api* p) const {
            delete p;
        }
    };
}

inline boost::shared_ptr<my_refcounting_api> adopt(my_refcounting_api* ptr) {
    return boost::shared_ptr<my_refcounting_api>(
        ptr,
        detail::library_holding_deleter(ptr)
    );
}

//]



