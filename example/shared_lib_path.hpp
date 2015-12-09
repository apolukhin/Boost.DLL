// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Copyright 2015 Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_DLL_EXAMPLE_COMMON_SHARED_LIB_PATH_HPP
#define BOOST_DLL_EXAMPLE_COMMON_SHARED_LIB_PATH_HPP

#include <boost/filesystem.hpp>
#include <boost/dll/shared_library.hpp>

namespace dll_test {

// This ugly struct is required to get the name of a library with bjam decorations.
struct replace_with_full_path {
    boost::filesystem::path full_path_;

    replace_with_full_path(const boost::filesystem::path& p)
        : full_path_(p)
    {}
};

boost::filesystem::path operator<<(const boost::filesystem::path& p, const replace_with_full_path& a) {
    BOOST_ASSERT(a.full_path_.string().find(p.filename().string().c_str()) != std::string::npos);
    (void)p;
    return a.full_path_;
}

inline bool is_shared_library(const boost::filesystem::path& p) {
    const std::string s = p.string();
    return s.find(boost::dll::shared_library::suffix().string()) != std::string::npos
            && s.find(".lib") == std::string::npos
            && s.find(".exp") == std::string::npos
            && s.find(".pdb") == std::string::npos
            && s.find(".manifest") == std::string::npos
            && s.find(".rsp") == std::string::npos
            && s.find(".obj") == std::string::npos
            && s.find(".a") == std::string::npos;
}

} // namespace dll_test

#endif // BOOST_DLL_EXAMPLE_COMMON_SHARED_LIB_PATH_HPP

