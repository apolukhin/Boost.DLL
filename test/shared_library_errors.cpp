// Copyright 2011-2012 Renato Tegon Forti.
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/dll/shared_library.hpp>
#include <boost/dll/library_info.hpp>
#include <boost/test/minimal.hpp>
#include <boost/function.hpp>

#include "../example/shared_lib_path.hpp"


// Unit Tests
int test_main(int argc, char* argv[]) {
    using namespace boost::dll;

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

    try {
        shared_library lib;
        lib.path();
        BOOST_CHECK(false);
    } catch (const boost::system::system_error& e) {
        std::cout << e.what() << '\n';
    }

    try {
        shared_library lib;
        lib.load("\0\0", load_mode::rtld_global);
        BOOST_CHECK(false);
    } catch (const boost::system::system_error& e) {
        std::cout << e.what() << '\n';
    }

    shared_library sl(shared_library_path);
    try {
        sl.get<int>("variable_or_function_that_does_not_exist");
        BOOST_CHECK(false);
    } catch (const boost::system::system_error& e) {
        std::cout << e.what() << '\n';
    }

    try {
        library_info lib("\0");
        BOOST_CHECK(false);
    } catch (const std::exception& e) {
        std::cout << e.what() << '\n';
    }

    try {
        std::string prog_name = argv[0];
#if BOOST_OS_WINDOWS
        prog_name.erase(prog_name.rfind('.'));
#endif
        prog_name += ".output";
        std::cout << "Attempt to open " << prog_name << '\n';
        library_info lib(prog_name);
        BOOST_CHECK(false);
    } catch (const std::exception& e) {
        std::cout << e.what() << '\n';
    }
    return 0;
}

