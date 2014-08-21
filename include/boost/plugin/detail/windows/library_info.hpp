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

// reference:
// http://msdn.microsoft.com/en-us/library/ms809762.aspx
class library_info : shared_library {

public:
    explicit library_info(const boost::filesystem::path &sl, boost::system::error_code &ec)
        : shared_library(sl, load_mode::dont_resolve_dll_references, ec) {}

    // load self test
    library_info(boost::system::error_code &ec) BOOST_NOEXCEPT {
       load_self(ec);
    }

    std::vector<std::string> sections(boost::system::error_code &ec) BOOST_NOEXCEPT {
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

        IMAGE_NT_HEADERS* image_nt_headers = (IMAGE_NT_HEADERS*)&((boost::detail::winapi::BYTE_*)native())[image_dos_header->e_lfanew];
        if (image_nt_headers->Signature != IMAGE_NT_SIGNATURE) {
            // ERROR_BAD_EXE_FORMAT
            ec = boost::system::error_code(
                 boost::system::errc::executable_format_error,
                 boost::system::generic_category()
                 );

            return ret;
        }

        IMAGE_SECTION_HEADER * image_section_header;
        image_section_header = IMAGE_FIRST_SECTION (image_nt_headers);

        // get names, e.g: .text .rdata .data .rsrc .reloc
        for (int i = 0; i < image_nt_headers->FileHeader.NumberOfSections; i++) { 
           
           // There is no terminating null character if the string is exactly eight characters long
           ret.push_back(std::string(reinterpret_cast<char*>(image_section_header->Name), 8));

           image_section_header++;
        }

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

        IMAGE_OPTIONAL_HEADER* image_optional_header = (IMAGE_OPTIONAL_HEADER*)((boost::detail::winapi::BYTE_*)native() + image_dos_header->e_lfanew + 24);
        if(!image_optional_header) {
            // ERROR_BAD_EXE_FORMAT 
            ec = boost::system::error_code(
                 boost::system::errc::executable_format_error,
                 boost::system::generic_category()
                 );

            return ret;
        }

        IMAGE_EXPORT_DIRECTORY* image_export_directory = (IMAGE_EXPORT_DIRECTORY*)((boost::detail::winapi::BYTE_*)native() + image_optional_header->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

        if (image_export_directory->NumberOfNames == 0 || image_export_directory->NumberOfFunctions == 0) {
            // we should return nay error here? 
            // or empty vector is self explicatory, that DLL doesn't export anything.
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

    // a test method to get dependents modules,
    // who my plugin imports (1st level only)
    /*
    e.g. for myself I get:
      KERNEL32.dll
      MSVCP110D.dll
      boost_system-vc-mt-gd-1_56.dll
      MSVCR110D.dll
    */
    std::vector<std::string> depend_of(boost::system::error_code &ec) BOOST_NOEXCEPT {
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

        IMAGE_OPTIONAL_HEADER* image_optional_header = (IMAGE_OPTIONAL_HEADER*)((boost::detail::winapi::BYTE_*)native() + image_dos_header->e_lfanew + 24);
        if(!image_optional_header) {
            // ERROR_BAD_EXE_FORMAT 
            ec = boost::system::error_code(
                 boost::system::errc::executable_format_error,
                 boost::system::generic_category()
                 );

            return ret;
        }

        IMAGE_IMPORT_DESCRIPTOR* image_import_descriptor =  (IMAGE_IMPORT_DESCRIPTOR*)((boost::detail::winapi::BYTE_*)native() + image_optional_header->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
        if(!image_import_descriptor) {
            // ERROR_BAD_EXE_FORMAT 
            ec = boost::system::error_code(
                 boost::system::errc::executable_format_error,
                 boost::system::generic_category()
                 );

            return ret;
        }

        while(image_import_descriptor->FirstThunk) {
           std::string module_name = reinterpret_cast<char*>((boost::detail::winapi::BYTE_*)native() + image_import_descriptor->Name);

           if(module_name.size()) {
              ret.push_back(module_name);
           }
                
           image_import_descriptor++;
        }

        return ret;
    }

    std::vector<std::string> symbols(boost::string_ref section_name) {
        std::vector<std::string> ret;

        // TODO:

        return ret;
    }
};

}} // namespace boost::plugin

#endif // BOOST_PLUGIN_DETAIL_WINDOWS_LIBRARY_INFO_HPP
