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
#include <boost/utility/string_ref.hpp>
#include <string>
#include <vector>

#include <link.h>
#include <elf.h>

namespace boost { namespace plugin {

class library_info {
    boost::filesystem::ifstream f_;
    typedef ElfW(Ehdr) header_t;
public:
    explicit library_info(const boost::filesystem::path& library_path)
        : f_(library_path, std::ios_base::in | std::ios_base::binary)
    {}

    std::vector<std::string> sections() {
        std::vector<std::string> ret;
        std::vector<char> names;
        sections_names_raw(names);
        
        const char* name_begin = &names[0];
        const char* const name_end = name_begin + names.size();
        do {
            ret.push_back(name_begin);
            name_begin += ret.back().size() + 1;
        } while (name_begin != name_end);

        return ret;
    }

private:
    inline header_t header() {
        header_t elf;

        f_.seekg(0);
        f_.read((char*)&elf, sizeof(elf));

        return elf;
    }

    void sections_names_raw(std::vector<char>& sections) {
        const header_t elf = header();

        ElfW(Shdr) section_names_section;
        f_.seekg(elf.e_shoff + elf.e_shstrndx * sizeof(ElfW(Shdr)));
        f_.read((char*)&section_names_section, sizeof(section_names_section));

        sections.resize(section_names_section.sh_size);
        f_.seekg(section_names_section.sh_offset);
        f_.read(&sections[0], section_names_section.sh_size);
    }

    void symbols_text(std::vector<ElfW(Sym)>& symbols, std::vector<char>& text) {
        const header_t elf = header();
        f_.seekg(elf.e_shoff);

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
    }

    static bool is_visible(const ElfW(Sym)& sym) BOOST_NOEXCEPT {
        return (sym.st_other & 0x03) == STV_DEFAULT;
    }

public:
    std::vector<std::string> symbols() {
        std::vector<std::string> ret;

        std::vector<ElfW(Sym)> symbols;
        std::vector<char>   text;
        symbols_text(symbols, text);

        ret.reserve(symbols.size());
        for (std::size_t i = 0; i < symbols.size(); ++i) {
            if (is_visible(symbols[i])) {
                ret.push_back(&text[0] + symbols[i].st_name);
            }
        }

        return ret;
    }

    std::vector<std::string> symbols(boost::string_ref section_name) {
        std::vector<std::string> ret;
        
        std::size_t index = 0;
        std::size_t ptrs_in_section_count = 0;
        {
            std::vector<char> names;
            sections_names_raw(names);

            const header_t elf = header();

            for (; index < elf.e_shnum; ++index) {
                ElfW(Shdr) section;
                f_.seekg(elf.e_shoff + index * sizeof(ElfW(Shdr)));
                f_.read((char*)&section, sizeof(section));
            
                if (&names[0] + section.sh_name == section_name) {
                    ptrs_in_section_count = section.sh_size / sizeof(void*);
                    break;
                }
            }                        
        }

        std::vector<ElfW(Sym)> symbols;
        std::vector<char>   text;
        symbols_text(symbols, text);
    
        if (ptrs_in_section_count < symbols.size()) {
            ret.reserve(ptrs_in_section_count);
        } else {
            ret.reserve(symbols.size());
        }

        for (std::size_t i = 0; i < symbols.size(); ++i) {
            if (symbols[i].st_shndx == index && is_visible(symbols[i])) {
                ret.push_back(&text[0] + symbols[i].st_name);
            }
        }

        return ret;
    }
};

}} // namespace boost::plugin

#endif // BOOST_PLUGIN_DETAIL_POSIX_LIBRARY_INFO_HPP
