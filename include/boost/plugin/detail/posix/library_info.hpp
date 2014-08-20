// detail/posix/library_info.hpp ------------------------------------------------------------
// -----------------------------------------------------------------------------

// Copyright 2014 Renato Tegon Forti, Antony Polukhin.

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 20-08-2014 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_PLUGIN_DETAIL_POSIX_LIBRARY_INFO_HPP
#define BOOST_PLUGIN_DETAIL_POSIX_LIBRARY_INFO_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <string>
#include <vector>

#include <link.h>
#include <elf.h>

namespace boost { namespace plugin {

class library_info {
    boost::filesystem::ifstream f_;

public:
    explicit library_info(const boost::filesystem::path& library_path)
        : f_(library_path, std::ios_base::in | std::ios_base::binary)
    {}

    std::vector<std::string> sections() {
        std::vector<std::string> ret;
        f_.seekg(0);

        ElfW(Ehdr) elf;
        f_.read((char*)&elf, sizeof(elf));

        ret.reserve(elf.e_shnum); // reserving for sections count

        ElfW(Shdr) section_names_section;
        f_.seekg(elf.e_shoff + elf.e_shstrndx * sizeof(ElfW(Shdr)));
        f_.read((char*)&section_names_section, sizeof(section_names_section));

        std::vector<char> names(section_names_section.sh_size);
        f_.seekg(section_names_section.sh_offset);

        f_.read(&names[0], section_names_section.sh_size);

        const char* name_begin = &names[0];
        const char* name_end = &names[0] + section_names_section.sh_size;
        do {
            ret.push_back(name_begin);
            name_begin += ret.back().size() + 1;
        } while (name_begin != name_end);

        return ret;
    }

    std::vector<std::string> symbols() {
        std::vector<std::string> ret;
        f_.seekg(0);

        ElfW(Ehdr) elf;
        f_.read((char*)&elf, sizeof(elf));

        ret.reserve(elf.e_shnum); // reserving for sections count

        ElfW(Shdr) section_names_section;
        f_.seekg(elf.e_shoff + elf.e_shstrndx * sizeof(ElfW(Shdr)));
        f_.read((char*)&section_names_section, sizeof(section_names_section));

        std::vector<char> names(section_names_section.sh_size);
        f_.seekg(section_names_section.sh_offset);

        f_.read(&names[0], section_names_section.sh_size);

        const char* name_begin = &names[0];
        const char* name_end = &names[0] + section_names_section.sh_size;

        f_.seekg(elf.e_shoff);

        std::vector<ElfW(Sym)> symbols;
        std::vector<char>   text;
        for (std::size_t i = 0; i < elf.e_shnum; ++i) {
            ElfW(Shdr) section;
            f_.read((char*)&section, sizeof(section));

            if (section.sh_type == SHT_SYMTAB) {
                symbols.resize(section.sh_size / section.sh_entsize);

                const std::size_t pos = f_.tellg();
                f_.seekg(section.sh_offset);
                f_.read((char*)&symbols[0], section.sh_size);
                f_.seekg(pos);
            } else if (section.sh_type == SHT_STRTAB) {
                text.resize(section.sh_size);

                const std::size_t pos = f_.tellg();
                f_.seekg(section.sh_offset);
                f_.read(&text[0], section.sh_size);
                f_.seekg(pos);
            }
        }


        for (std::size_t i = 0; i < symbols.size(); ++i) {
            ret.push_back(&text[0] + symbols[i].st_name);
        }

        return ret;
    }
};

}} // namespace boost::plugin

#endif // BOOST_PLUGIN_DETAIL_POSIX_LIBRARY_INFO_HPP
