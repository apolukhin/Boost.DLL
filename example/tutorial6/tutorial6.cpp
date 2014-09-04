// Copyright 2011-2012 Renato Tegon Forti.
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// -------------------------------------------------------------------------------------
// This example shows how to use load_self to load symbols direct on executable
// -------------------------------------------------------------------------------------


#include "../shared_lib_path.hpp"

//[callplugcpp_tutorial6
#include <boost/dll.hpp>
#include <boost/function.hpp>
#include <iostream>

typedef boost::function<void()> callback_t;

void print_unloaded() {
    std::cout << "unloaded" << std::endl;
}

int main(int argc, char* argv[]) {
    // argv[1] contains path to our plugin library
    BOOST_ASSERT(argc >= 2);
    boost::filesystem::path shared_library_path = shared_lib_path(argv[1], L"on_unload_lib");

    // loading library and getting a function from it
    boost::function<void(const callback_t&)> on_unload
        = boost::dll::import_function_alias<void(const callback_t&)>(
            shared_library_path, "on_unload"
        );

    on_unload(&print_unloaded); // adding a callback
    std::cout << "Before library unload." << std::endl;

    // Releasing last reference to the library, so that it gets unloaded
    on_unload.clear();
    std::cout << "After library unload." << std::endl;
}
//]

