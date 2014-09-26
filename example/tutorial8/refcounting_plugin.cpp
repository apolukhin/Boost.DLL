// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <boost/make_shared.hpp>

//[plugcpp_my_plugin_refcounting
   
#include "../tutorial_common/plugin_api.hpp"
#include <boost/dll.hpp>

namespace my_namespace {

class my_plugin_refcounting : public my_plugin_api {
    boost::dll::shared_library ref_holder_;

    my_plugin_refcounting()
        : ref_holder_(boost::dll::this_line_location())
    {
        std::cout << "Refc constructed, holding a reference to " << boost::dll::this_line_location() << std::endl;
    }

public:
    ~my_plugin_refcounting() BOOST_NOEXCEPT_IF(false) {
        std::cout << "Refc destructing, holding a reference to " << boost::dll::this_line_location() << std::endl;
        // FUCK!!! Deleting the last instance
        // of shared_library causes call to global destructors and unmapping of the code page that we are currently using.
        // Return address from dlclose() becomes invalid.
        //
        // BAD IDEA: In other words we need a dlclose() that returns directly to the program code instead of the next instruction.
        // But such approcah will cause memory leaks and
        // even unclosed resources, because in that case all the code after this point won't be executed.
        //
        // BETTER IDEA: We need to use the `shared_library::unload()` instance from out main program. In that case
        // return address will be in main program. But this also requires all the code of create() function to be in
        // main program, which does not look good.
    }

    float version() const {
        return 1.0;
    }

    std::string name() const {
        return "refcounting";
    }

    float calculate(float x, float y) {
        return 0;
    }

    // Factory method
    static boost::shared_ptr<my_plugin_refcounting> create() {
        return boost::shared_ptr<my_plugin_refcounting>(
            new my_plugin_refcounting()
        );
    }
};

//struct throw_on_closing {
//    ~throw_on_closing() BOOST_NOEXCEPT_IF(false) {
//        throw std::logic_error("Avoiding incorrect return address");
//    }
//};

//throw_on_closing insane_guard;

BOOST_DLL_ALIAS(
    my_namespace::my_plugin_refcounting::create,
    create_refc_plugin
)

} // namespace my_namespace

//]



