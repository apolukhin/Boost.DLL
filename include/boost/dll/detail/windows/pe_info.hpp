// detail/windows/library_info.hpp ---------------------------------------------
// -----------------------------------------------------------------------------

// Copyright 2014 Renato Tegon Forti, Antony Polukhin.

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 20-08-2014 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_DLL_DETAIL_WINDOWS_PE_INFO_HPP
#define BOOST_DLL_DETAIL_WINDOWS_PE_INFO_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

#include <boost/filesystem/fstream.hpp>
#include <boost/utility/string_ref.hpp>
#include <string>
#include <vector>

// TODO: add to dll2.hpp
#include <windows.h>

namespace boost { namespace dll { namespace detail {

// reference:
// http://www.joachim-bauch.de/tutorials/loading-a-dll-from-memory/
// http://msdn.microsoft.com/en-us/magazine/ms809762.aspx
// http://msdn.microsoft.com/en-us/magazine/cc301808.aspx
//

class pe_info {
    boost::filesystem::ifstream&    f_;

    typedef IMAGE_NT_HEADERS        header_t;
    typedef IMAGE_EXPORT_DIRECTORY  exports_t;
    typedef IMAGE_SECTION_HEADER    section_t;
    typedef IMAGE_DOS_HEADER        dos_t;

public:
    explicit pe_info(boost::filesystem::ifstream& f) BOOST_NOEXCEPT
        : f_(f)
    {}

private:
    inline header_t header() {
        header_t h;

        dos_t dos;
        f_.seekg(0);
        f_.read((char*)&dos, sizeof(dos));

        f_.seekg(dos.e_lfanew);
        f_.read((char*)&h, sizeof(h));

        return h;
    }
    
    inline exports_t exports(const header_t& h) {
        exports_t exports;

        static const unsigned int IMAGE_DIRECTORY_ENTRY_EXPORT_ = 0;
        const std::size_t exp_virtual_address = h.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT_].VirtualAddress;

        const std::size_t real_offset = get_file_offset(exp_virtual_address, h);
        BOOST_ASSERT(real_offset);

        f_.seekg(real_offset);
        f_.read((char*)&exports, sizeof(exports));

        return exports;
    }

    std::size_t get_file_offset(std::size_t virtual_address, const header_t& h) {
        section_t image_section_header;
        
        {   // f_.seekg to the beginning on section headers
            dos_t dos;
            f_.seekg(0);
            f_.read((char*)&dos, sizeof(dos));
            f_.seekg(dos.e_lfanew + sizeof(header_t));
        }

        for (std::size_t i = 0;i < h.FileHeader.NumberOfSections;++i) {
            f_.read((char*)&image_section_header, sizeof(image_section_header));
            if (virtual_address >= image_section_header.VirtualAddress 
                && virtual_address < image_section_header.VirtualAddress + image_section_header.SizeOfRawData) 
            {
                return image_section_header.PointerToRawData + virtual_address - image_section_header.VirtualAddress;
            }
        }

        return 0;
    }

public:
    std::vector<std::string> sections() {
        std::vector<std::string> ret;

        const header_t h = header();
        ret.reserve(h.FileHeader.NumberOfSections);

        // get names, e.g: .text .rdata .data .rsrc .reloc
        section_t image_section_header;
        char name_helper[9];
        std::memset(name_helper, 0, sizeof(name_helper));
        for (std::size_t i = 0;i < h.FileHeader.NumberOfSections;++i) {
            // There is no terminating null character if the string is exactly eight characters long
            f_.read((char*)&image_section_header, sizeof(image_section_header));
            std::memcpy(name_helper, image_section_header.Name, 8);
            ret.push_back(name_helper);
        }

        return ret;
    }

    std::vector<std::string> symbols() {
        std::vector<std::string> ret;

        const header_t h = header();
        const exports_t exprt = exports(h);
        const std::size_t exported_symbols = exprt.NumberOfNames;
        const std::size_t fixed_names_addr = get_file_offset(exprt.AddressOfNames, h);

        ret.reserve(exported_symbols);
        DWORD name_offset;
        std::string symbol_name;
        for (std::size_t i = 0;i < exported_symbols;++i) {
            f_.seekg(fixed_names_addr + i * sizeof(name_offset));
            f_.read((char*)&name_offset, sizeof(name_offset));
            f_.seekg(get_file_offset(name_offset, h));
            getline(f_, symbol_name, '\0');
            ret.push_back(symbol_name);
        }

        return ret;
    }

    std::vector<std::string> symbols(boost::string_ref section_name) {
        std::vector<std::string> ret;

        const header_t h = header();
        
        std::size_t section_begin_addr = 0;
        std::size_t section_end_addr = 0;
        
        {   // getting address range for the section
            section_t image_section_header;
            char name_helper[9];
            std::memset(name_helper, 0, sizeof(name_helper));
            for (std::size_t i = 0;i < h.FileHeader.NumberOfSections;++i) {
                // There is no terminating null character if the string is exactly eight characters long
                f_.read((char*)&image_section_header, sizeof(image_section_header));
                std::memcpy(name_helper, image_section_header.Name, 8);
                if (!std::strcmp(section_name.data(), name_helper)) {
                    section_begin_addr = image_section_header.PointerToRawData;
                    section_end_addr = section_begin_addr + image_section_header.SizeOfRawData;
                }
            }
            BOOST_ASSERT(section_begin_addr);
            BOOST_ASSERT(section_end_addr);
        }

        const exports_t exprt = exports(h);
        const std::size_t exported_symbols = exprt.NumberOfFunctions;
        const std::size_t fixed_names_addr = get_file_offset(exprt.AddressOfNames, h);
        const std::size_t fixed_ordinals_addr = get_file_offset(exprt.AddressOfNameOrdinals, h);
        const std::size_t fixed_functions_addr = get_file_offset(exprt.AddressOfFunctions, h);

        ret.reserve(exported_symbols);
        DWORD ptr;
        WORD ordinal;
        std::string symbol_name;
        for (std::size_t i = 0;i < exported_symbols;++i) {
            // getting ordinal
            f_.seekg(fixed_ordinals_addr + i * sizeof(ordinal));
            f_.read((char*)&ordinal, sizeof(ordinal));

            // getting function addr
            f_.seekg(fixed_functions_addr + ordinal * sizeof(ptr));
            f_.read((char*)&ptr, sizeof(ptr));
            ptr = get_file_offset(ptr, h);

            if (ptr >= section_end_addr || ptr < section_begin_addr) {
                continue;
            }

            f_.seekg(fixed_names_addr + i * sizeof(ptr));
            f_.read((char*)&ptr, sizeof(ptr));
            f_.seekg(get_file_offset(ptr, h));
            getline(f_, symbol_name, '\0');
            ret.push_back(symbol_name);
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
    /*
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
*/
};

}}} // namespace boost::dll::detail

#endif // BOOST_DLL_DETAIL_WINDOWS_PE_INFO_HPP
