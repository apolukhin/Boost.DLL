// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_DLL_EXAMPLE_COMMON_SHARED_LIB_PATH_HPP
#define BOOST_DLL_EXAMPLE_COMMON_SHARED_LIB_PATH_HPP

#include <boost/filesystem.hpp>

namespace dll_test {

// This ugly struct is required to get the name of a library with bjam decorations.
struct argv_to_path {
    boost::filesystem::path whole_path;

    template <class Argv>
    argv_to_path(int argc, const Argv& argv) {
        BOOST_ASSERT(argc >= 2);
        whole_path = argv[1];
    }
};

boost::filesystem::path operator<<(const boost::filesystem::path& p, const argv_to_path& a) {
    BOOST_ASSERT(a.whole_path.string().find(p.filename().string().c_str()) != std::string::npos);
    return a.whole_path;
}

} // namespace dll_test

#endif // BOOST_DLL_EXAMPLE_COMMON_SHARED_LIB_PATH_HPP

