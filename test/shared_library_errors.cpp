// Copyright 2011-2012 Renato Tegon Forti.
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/plugin/shared_library.hpp>
#include <boost/test/minimal.hpp>
#include <boost/function.hpp>

#include "../example/shared_lib_path.hpp"


// Unit Tests
int test_main(int argc, char* argv[]) {
    using namespace boost::plugin;

    BOOST_CHECK(argc >= 2);
    boost::filesystem::path shared_library_path = shared_lib_path(argv[1], L"test_library");
    boost::filesystem::path bad_path = shared_library_path / "directory_that_does_not_exist";

    try {
        shared_library lib(bad_path);
        BOOST_CHECK(false);
    } catch (const boost::system::system_error& e) {
        std::cout << e.what() << '\n';
    }

    try {
        shared_library lib;
        lib.get<int>("variable_or_function_that_does_not_exist");
        BOOST_CHECK(false);
    } catch (const boost::system::system_error& e) {
        std::cout << e.what() << '\n';
    }

/*  // Following two tests will fail on Linux. linux allows to open "" paths
    // and loads current executable as a DSO
    try {
        shared_library lib("");
        BOOST_CHECK(false);
    } catch (const boost::system::system_error& e) {
        std::cout << e.what() << '\n';
    }


    try {
        shared_library lib("\0\0");
        BOOST_CHECK(false);
    } catch (const boost::system::system_error& e) {
        std::cout << e.what() << '\n';
    }
*/
    shared_library sl(shared_library_path);
    try {
        sl.get<int>("variable_or_function_that_does_not_exist");
        BOOST_CHECK(false);
    } catch (const boost::system::system_error& e) {
        std::cout << e.what() << '\n';
    }

    return 0;
}

