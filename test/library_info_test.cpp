// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/plugin/library_info.hpp>
#include <boost/test/minimal.hpp>

#include "../example/shared_lib_path.hpp"
// Unit Tests

#include <iterator>

int test_main(int argc, char* argv[])
{

    BOOST_CHECK(argc >= 2);
    boost::filesystem::path shared_library_path = shared_lib_path(argv[1], L"test_library");
    std::cout << "Library: " << shared_library_path;

    boost::plugin::library_info lib_info(shared_library_path);
    std::vector<std::string> sec = lib_info.sections();
    std::copy(sec.begin(), sec.end(), std::ostream_iterator<std::string>(std::cout, ",  "));

    std::cout << "\n\n\n";
    std::vector<std::string> symb = lib_info.symbols();
    std::copy(symb.begin(), symb.end(), std::ostream_iterator<std::string>(std::cout, "\n"));

    return 0;
}
