// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/dll/library_info.hpp>
#include <boost/test/minimal.hpp>
#include "../example/tutorial4/static_plugin.hpp"

#include "../example/shared_lib_path.hpp"
// Unit Tests

#include <iterator>

int test_main(int argc, char* argv[])
{
    BOOST_CHECK(argc >= 2);
    boost::filesystem::path shared_library_path = shared_lib_path(argv[1], L"test_library");
    std::cout << "Library: " << shared_library_path;

    boost::dll::library_info lib_info(shared_library_path);
    std::vector<std::string> sec = lib_info.sections();
    //std::copy(sec.begin(), sec.end(), std::ostream_iterator<std::string>(std::cout, ",  "));
    BOOST_CHECK(std::find(sec.begin(), sec.end(), "boostdll") != sec.end());


    std::cout << "\n\n\n";
    std::vector<std::string> symb = lib_info.symbols();
    std::copy(symb.begin(), symb.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
    BOOST_CHECK(std::find(symb.begin(), symb.end(), "const_integer_g") != symb.end());
    BOOST_CHECK(std::find(symb.begin(), symb.end(), "say_hello") != symb.end());
    
    symb = lib_info.symbols("boostdll");
    std::copy(symb.begin(), symb.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
    BOOST_CHECK(std::find(symb.begin(), symb.end(), "const_integer_g_alias") != symb.end());
    BOOST_CHECK(std::find(symb.begin(), symb.end(), "foo_variable") != symb.end());
    BOOST_CHECK(std::find(symb.begin(), symb.end(), "const_integer_g") == symb.end());
    BOOST_CHECK(std::find(symb.begin(), symb.end(), "say_hello") == symb.end());


    // Self testing
    std::cout << "Self: " << argv[0];
    boost::dll::library_info self_info(argv[0]);

    sec = self_info.sections();
    //std::copy(sec.begin(), sec.end(), std::ostream_iterator<std::string>(std::cout, ",  "));
    BOOST_CHECK(std::find(sec.begin(), sec.end(), "boostdll") != sec.end());

    symb = self_info.symbols("boostdll");
    std::copy(symb.begin(), symb.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
    BOOST_CHECK(std::find(symb.begin(), symb.end(), "create_plugin") != symb.end());

    return 0;
}
