// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_DLL_EXAMPLE_COMMON_SHARED_LIB_PATH_HPP
#define BOOST_DLL_EXAMPLE_COMMON_SHARED_LIB_PATH_HPP

#include <boost/filesystem.hpp>
boost::filesystem::path shared_lib_path(const boost::filesystem::path& root, const std::wstring& filename_part) {
    namespace fs = ::boost::filesystem;
    fs::directory_iterator endit;

    for (fs::directory_iterator it(root); it != endit; ++it) {
        if (fs::is_regular_file(*it)
            && it->path().filename().wstring().find(filename_part) != std::wstring::npos
            && (it->path().extension() == ".dll" || it->path().extension() == ".so" || it->path().extension() == ".dylib")) {
            return *it;
        }
    }

    throw std::runtime_error("Failed to find library");
}
   
#endif // BOOST_DLL_EXAMPLE_COMMON_SHARED_LIB_PATH_HPP

