// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// -------------------------------------------------------------------------------------
// This example shows how to import_variable_alias function to load a plugin of a DSO.
// -------------------------------------------------------------------------------------

#include "../shared_lib_path.hpp"

//[callplugcpp_tutorial7
#include <boost/dll.hpp>
#include <iostream>

void load_and_execute(const boost::filesystem::path* libraries, std::size_t libs_count) {
    const std::string username = "User";

    for (std::size_t i = 0; i < libs_count; ++i) {
        // Class `library_info` can extract information from a library
        boost::dll::library_info inf(libraries[i]);

        // Getting symbols exported from 'Anna' section
        std::vector<std::string> exports = inf.symbols("Anna");

        // Loading library and importing symbols from it
        boost::dll::shared_library lib(libraries[i]);
        for (std::size_t j = 0; j < exports.size(); ++j) {
            std::cout << "\nFunction '" << exports[j] << "' prints:\n\t";
            lib.get_alias<void(const std::string&)>(exports[j]) // importing function
                (username);                                     // calling function
        }
    }
}
//]

int main(int argc, char* argv[]) { 
    // argv[1] contains path to our plugin library 
    BOOST_ASSERT(argc >= 2);
    const std::size_t libs_count = 2;
    boost::filesystem::path libraries[libs_count] = {
        shared_lib_path(argv[1], L"library1"),
        shared_lib_path(argv[1], L"library2")
    };

    load_and_execute(libraries, libs_count);
}
