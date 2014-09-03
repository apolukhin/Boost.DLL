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
#include <iostream> // TODO: remove me!

// TODO: add to dll2.hpp
#include <windows.h>

namespace boost { namespace dll {

// reference:
// http://www.joachim-bauch.de/tutorials/loading-a-dll-from-memory/
// http://msdn.microsoft.com/en-us/magazine/ms809762.aspx
// http://msdn.microsoft.com/en-us/magazine/cc301808.aspx
//

// work in progress
// this shoud be an impl class??

class library_info {
    boost::filesystem::ifstream f_;
    typedef IMAGE_NT_HEADERS        header_t;
    typedef IMAGE_EXPORT_DIRECTORY  exports_t;
    
public:
    explicit library_info(const boost::filesystem::path& library_path)
        : f_(library_path, std::ios_base::in | std::ios_base::binary)
    {
        f_.exceptions( boost::filesystem::ifstream::failbit | boost::filesystem::ifstream::badbit | boost::filesystem::ifstream::eofbit);
    }

private:
    inline header_t header() {
        header_t h;

        IMAGE_DOS_HEADER dos;
        f_.seekg(0);
        f_.read((char*)&dos, sizeof(dos));

        f_.seekg(dos.e_lfanew);
        f_.read((char*)&h, sizeof(h));
        
        return h;
    }
    
    inline exports_t exports() {
        exports_t exports;
        
        static const unsigned int IMAGE_DIRECTORY_ENTRY_EXPORT_ = 0;
        const header_t h = header();
        const std::size_t exp_virtual_address = h.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT_].VirtualAddress;
        
        const std::size_t real_offset = get_offset(exp_virtual_address);
        BOOST_ASSERT(real_offset);
        
        f_.seekg(real_offset);
        f_.read((char*)&exports, sizeof(exports));
        
        return exports;
    }

    std::size_t get_offset(std::size_t virtual_address) { // TODO: optimize me!
        const header_t h = header();
        
        IMAGE_SECTION_HEADER image_section_header;
        for (std::size_t i = 0; i < h.FileHeader.NumberOfSections; ++i) {
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
        IMAGE_SECTION_HEADER image_section_header;
        char name_helper[9];
        std::memset(name_helper, 0, sizeof(name_helper));
        for (std::size_t i = 0; i < h.FileHeader.NumberOfSections; ++i) {           
            // There is no terminating null character if the string is exactly eight characters long
            f_.read((char*)&image_section_header, sizeof(image_section_header));
            std::memcpy(name_helper, image_section_header.Name, 8);
            ret.push_back(name_helper);
        }

        return ret;
    }

    std::vector<std::string> symbols() {
        std::vector<std::string> ret;
        const exports_t exprt = exports();
        
        const std::size_t exported_symbols = exprt.NumberOfNames;
        const std::size_t fixed_names_addr = get_offset(exprt.AddressOfNames);
        
        ret.reserve(exported_symbols);
        DWORD name_offset;
        std::string symbol_name;
        for (std::size_t i = 0; i < exported_symbols; ++i) {
            f_.seekg(fixed_names_addr + i * sizeof(name_offset));
            f_.read((char*)&name_offset, sizeof(name_offset));
            f_.seekg(get_offset(name_offset));
            getline(f_, symbol_name, '\0');
            ret.push_back(symbol_name);
        }

        return ret;
    }
    
    std::vector<std::string> symbols(boost::string_ref section_name) {
        std::vector<std::string> ret;
        const exports_t exprt = exports();
        
        const std::size_t exported_symbols = exprt.NumberOfNames;
        const std::size_t fixed_names_addr = get_offset(exprt.AddressOfNames);
        const std::size_t fixed_ordinals_addr = get_offset(exprt.AddressOfNameOrdinals);
        
        ret.reserve(exported_symbols);
        DWORD name_offset;
        std::string symbol_name;
        for (std::size_t i = 0; i < exported_symbols; ++i) {
            // TODO: stopped here
        
            f_.seekg(fixed_names_addr + i * sizeof(name_offset));
            f_.read((char*)&name_offset, sizeof(name_offset));
            std::cerr << "\nnname_offset: " << name_offset;
            f_.seekg(get_offset(name_offset));
            
            
            getline(f_, symbol_name, '\0');
            ret.push_back(symbol_name);
            std::cerr << "\nExported: " << symbol_name;
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

    std::vector<std::string> symbols(boost::string_ref section_name) {
        std::vector<std::string> ret;

        // TODO:

        return ret;
    }*/
};

}} // namespace boost::dll

#endif // BOOST_DLL_DETAIL_WINDOWS_LIBRARY_INFO_HPP
