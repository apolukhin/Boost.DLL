// detail/windows/library_info.hpp ---------------------------------------------
// -----------------------------------------------------------------------------

// Copyright 2014 Renato Tegon Forti, Antony Polukhin.

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 20-08-2014 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_PLUGIN_DETAIL_WINDOWS_LIBRARY_INFO_HPP
#define BOOST_PLUGIN_DETAIL_WINDOWS_LIBRARY_INFO_HPP

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
#include <boost/plugin/shared_library.hpp>

namespace boost { namespace plugin {

// work in progress
// this shoud be an impl class??
class library_info : shared_library {

public:
    explicit library_info(const boost::filesystem::path &sl)
        : shared_library(sl)
    {}

    std::vector<std::string> sections() {
        std::vector<std::string> ret;

        // TODO:

        return ret;
    }

    std::vector<std::string> symbols(boost::system::error_code &ec) BOOST_NOEXCEPT {
        std::vector<std::string> ret;

        IMAGE_DOS_HEADER* image_dos_header = (IMAGE_DOS_HEADER*)native();
        if(!image_dos_header) {
            // ERROR_BAD_EXE_FORMAT 
            ec = boost::system::error_code(
                 boost::system::errc::executable_format_error,
                 boost::system::generic_category()
                 );

            return ret;
        }

        /*
        IMAGE_NT_HEADERS* image_nt_headers = (IMAGE_NT_HEADERS*)&((BYTE*)native())[image_dos_header->e_lfanew];
        if (image_nt_headers->Signature != IMAGE_NT_SIGNATURE) 
        {
           // ERROR_BAD_EXE_FORMAT
        }
        */

        IMAGE_OPTIONAL_HEADER* image_optional_header = (IMAGE_OPTIONAL_HEADER*)((BYTE*)native() + image_dos_header->e_lfanew + 24);
        if(!image_optional_header) {
            // ERROR_BAD_EXE_FORMAT 
            ec = boost::system::error_code(
                 boost::system::errc::executable_format_error,
                 boost::system::generic_category()
                 );

            return ret;
        }

        IMAGE_EXPORT_DIRECTORY* image_export_directory = (IMAGE_EXPORT_DIRECTORY*)((BYTE*)native() + image_optional_header->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

        if (image_export_directory->NumberOfNames == 0 || image_export_directory->NumberOfFunctions == 0) {
            // DLL doesn't export anything
            // ERROR_PROC_NOT_FOUND
            // TODO: what error we should return here?
            return ret;
        }

        boost::detail::winapi::DWORD_ exported_symbols = image_export_directory->NumberOfFunctions;
        boost::detail::winapi::DWORD_ symbol_name_size = image_export_directory->Name;

        for(boost::detail::winapi::DWORD_ i=0; i<=exported_symbols; i++) {
            boost::detail::winapi::DWORD_ dwData = image_export_directory->AddressOfNameOrdinals;

            if( image_export_directory->NumberOfNames >= i ) {
                std::string symbol_name;
                if( FALSE == IsBadReadPtr((boost::detail::winapi::BYTE_*)native() +  image_export_directory->Name, 1)) { 
                    symbol_name = reinterpret_cast<char*>((boost::detail::winapi::BYTE_*)native() + symbol_name_size);
                }

                // adjust size
                symbol_name_size = symbol_name_size  + symbol_name.length() + 1;

                if(symbol_name.size()) {
                    boost::system::error_code ec;
                    if(symbol_addr(symbol_name, ec)) // check if this is a valid symbol
                        ret.push_back(symbol_name);
                }
            }
        }

        return ret;
    }
    /*

    -- start of execution --
    ??0CConsoleApplication2@@QAE@XZ
    ??4CConsoleApplication2@@QAEAAV0@ABV0@@Z
    ?fnConsoleApplication2@@YAHXZ
    ?nConsoleApplication2@@3HA
    ?teste_dllexport@@YAHXZ
    f
    func
    gx 
    Pressione qualquer tecla para continuar. . .
    -- end of execution --

    gx is a variable all other are function symbols

    */

    std::vector<std::string> symbols(boost::string_ref section_name) {
        std::vector<std::string> ret;

         // TODO:

        return ret;
    }


};

}} // namespace boost::plugin

#endif // BOOST_PLUGIN_DETAIL_WINDOWS_LIBRARY_INFO_HPP
