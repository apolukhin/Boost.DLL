// detail/windows/library_info.hpp ---------------------------------------------
// -----------------------------------------------------------------------------

// Copyright 2014 Renato Tegon Forti, Antony Polukhin.

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 20-08-2014 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_DLL_DETAIL_WINDOWS_LIBRARY_INFO_HPP
#define BOOST_DLL_DETAIL_WINDOWS_LIBRARY_INFO_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/utility/string_ref.hpp>
#include <string>
#include <vector>

// TODO: add to dll2.hpp
#include <windows.h>

// first try using ifstream, working

namespace boost { namespace dll {

class library_info {
    boost::filesystem::ifstream f_;
    typedef IMAGE_DOS_HEADER header_t;

public:
    explicit library_info(const boost::filesystem::path& library_path)
        : f_(library_path, std::ios_base::in | std::ios_base::binary)
    {}


    std::vector<std::string> sections() {
        std::vector<std::string> ret;
        
        IMAGE_NT_HEADERS image_nt_headers;

        f_.seekg(header().e_lfanew);
        f_.read((char*)&image_nt_headers, sizeof(image_nt_headers));

        // todo

        return ret;
    }

private:

    // return image_dos_header
    inline header_t header() {
        header_t image_dos_header; 

        f_.seekg(0);
        f_.read((char*)&image_dos_header, sizeof(image_dos_header));

        return image_dos_header;
    }
 

public:
    std::vector<std::string> symbols() {
        std::vector<std::string> ret;

        return ret;
    }

    std::vector<std::string> symbols(boost::string_ref section_name) {
        std::vector<std::string> ret;
        
        return ret;
    }
};

}} // namespace boost::dll

#endif // BOOST_DLL_DETAIL_WINDOWS_LIBRARY_INFO_HPP
