// detail/posix/library_info.hpp ------------------------------------------------------------
// -----------------------------------------------------------------------------

// Copyright 2014 Renato Tegon Forti, Antony Polukhin.

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 20-08-2014 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_DLL_DETAIL_POSIX_ELF_INFO_HPP
#define BOOST_DLL_DETAIL_POSIX_ELF_INFO_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

#include <boost/filesystem/fstream.hpp>
#include <boost/utility/string_ref.hpp>
#include <boost/cstdint.hpp>
#include <string>
#include <vector>

namespace boost { namespace dll { namespace detail {

template <class AddressOffsetT>
struct Elf_Ehdr_template {
  unsigned char     e_ident[16];    /* Magic number and other info */
  boost::uint16_t   e_type;         /* Object file type */
  boost::uint16_t   e_machine;      /* Architecture */
  boost::uint32_t   e_version;      /* Object file version */
  AddressOffsetT    e_entry;        /* Entry point virtual address */
  AddressOffsetT    e_phoff;        /* Program header table file offset */
  AddressOffsetT    e_shoff;        /* Section header table file offset */
  boost::uint32_t   e_flags;        /* Processor-specific flags */
  boost::uint16_t   e_ehsize;       /* ELF header size in bytes */
  boost::uint16_t   e_phentsize;    /* Program header table entry size */
  boost::uint16_t   e_phnum;        /* Program header table entry count */
  boost::uint16_t   e_shentsize;    /* Section header table entry size */
  boost::uint16_t   e_shnum;        /* Section header table entry count */
  boost::uint16_t   e_shstrndx;     /* Section header string table index */
};

typedef Elf_Ehdr_template<boost::uint32_t> Elf32_Ehdr_;
typedef Elf_Ehdr_template<boost::uint64_t> Elf64_Ehdr_;

template <class AddressOffsetT>
struct Elf_Shdr_template {
  boost::uint32_t   sh_name;        /* Section name (string tbl index) */
  boost::uint32_t   sh_type;        /* Section type */
  AddressOffsetT   sh_flags;        /* Section flags */
  AddressOffsetT    sh_addr;        /* Section virtual addr at execution */
  AddressOffsetT    sh_offset;      /* Section file offset */
  AddressOffsetT   sh_size;         /* Section size in bytes */
  boost::uint32_t   sh_link;        /* Link to another section */
  boost::uint32_t   sh_info;        /* Additional section information */
  AddressOffsetT   sh_addralign;    /* Section alignment */
  AddressOffsetT   sh_entsize;      /* Entry size if section holds table */
};

typedef Elf_Shdr_template<boost::uint32_t> Elf32_Shdr_;
typedef Elf_Shdr_template<boost::uint64_t> Elf64_Shdr_;

template <class AddressOffsetT>
struct Elf_Sym_template;

template <>
struct Elf_Sym_template<boost::uint32_t> {
  typedef boost::uint32_t AddressOffsetT;

  boost::uint32_t   st_name;    /* Symbol name (string tbl index) */
  AddressOffsetT    st_value;   /* Symbol value */
  AddressOffsetT    st_size;    /* Symbol size */
  unsigned char     st_info;    /* Symbol type and binding */
  unsigned char     st_other;   /* Symbol visibility */
  boost::uint16_t   st_shndx;   /* Section index */
};

template <>
struct Elf_Sym_template<boost::uint64_t> {
  typedef boost::uint64_t AddressOffsetT;

  boost::uint32_t   st_name;    /* Symbol name (string tbl index) */
  unsigned char     st_info;    /* Symbol type and binding */
  unsigned char     st_other;   /* Symbol visibility */
  boost::uint16_t   st_shndx;   /* Section index */
  AddressOffsetT    st_value;   /* Symbol value */
  AddressOffsetT    st_size;    /* Symbol size */
};


typedef Elf_Sym_template<boost::uint32_t> Elf32_Sym_;
typedef Elf_Sym_template<boost::uint64_t> Elf64_Sym_;



class elf_info {
    boost::filesystem::ifstream& f_;

#if BOOST_ARCH_X86_64
    typedef boost::dll::detail::Elf64_Ehdr_  header_t;
    typedef boost::dll::detail::Elf64_Shdr_  section_t;
    typedef boost::dll::detail::Elf64_Sym_   symbol_t;
#else
    typedef boost::dll::detail::Elf32_Ehdr_  header_t;
    typedef boost::dll::detail::Elf32_Shdr_  section_t;
    typedef boost::dll::detail::Elf32_Sym_   symbol_t;
#endif

    BOOST_STATIC_CONSTANT(boost::uint32_t, SHT_SYMTAB_ = 2);
    BOOST_STATIC_CONSTANT(boost::uint32_t, SHT_STRTAB_ = 3);

    /* Symbol visibility specification encoded in the st_other field.  */
    BOOST_STATIC_CONSTANT(unsigned char, STV_DEFAULT_ = 0);      /* Default symbol visibility rules */
    BOOST_STATIC_CONSTANT(unsigned char, STV_INTERNAL_ = 1);     /* Processor specific hidden class */
    BOOST_STATIC_CONSTANT(unsigned char, STV_HIDDEN_ = 2);       /* Sym unavailable in other modules */
    BOOST_STATIC_CONSTANT(unsigned char, STV_PROTECTED_ = 3);    /* Not preemptible, not exported */

public:
    explicit elf_info(boost::filesystem::ifstream& f) BOOST_NOEXCEPT
        : f_(f)
    {}

    std::vector<std::string> sections() {
        std::vector<std::string> ret;
        std::vector<char> names;
        sections_names_raw(names);
        
        const char* name_begin = &names[0];
        const char* const name_end = name_begin + names.size();
        ret.reserve(header().e_shnum);
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

        section_t section_names_section;
        f_.seekg(elf.e_shoff + elf.e_shstrndx * sizeof(section_t));
        f_.read((char*)&section_names_section, sizeof(section_names_section));

        sections.resize(section_names_section.sh_size);
        f_.seekg(section_names_section.sh_offset);
        f_.read(&sections[0], section_names_section.sh_size);
    }

    void symbols_text(std::vector<symbol_t>& symbols, std::vector<char>& text) {
        const header_t elf = header();
        f_.seekg(elf.e_shoff);

        for (std::size_t i = 0; i < elf.e_shnum; ++i) {
            section_t section;
            f_.read((char*)&section, sizeof(section));

            if (section.sh_type == SHT_SYMTAB_) {
                symbols.resize(section.sh_size / section.sh_entsize);

                const std::size_t pos = f_.tellg();
                f_.seekg(section.sh_offset);
                f_.read((char*)&symbols[0], section.sh_size);
                f_.seekg(pos);
            } else if (section.sh_type == SHT_STRTAB_) {
                text.resize(section.sh_size);

                const std::size_t pos = f_.tellg();
                f_.seekg(section.sh_offset);
                f_.read(&text[0], section.sh_size);
                f_.seekg(pos);
            }
        }
    }

    static bool is_visible(const symbol_t& sym) BOOST_NOEXCEPT {
        return (sym.st_other & 0x03) == STV_DEFAULT_;
    }

public:
    std::vector<std::string> symbols() {
        std::vector<std::string> ret;

        std::vector<symbol_t> symbols;
        std::vector<char>   text;
        symbols_text(symbols, text);

        ret.reserve(symbols.size());
        for (std::size_t i = 0; i < symbols.size(); ++i) {
            if (is_visible(symbols[i])) {
                ret.push_back(&text[0] + symbols[i].st_name);
                if (ret.back().empty()) {
                    ret.pop_back(); // Do not show empty names
                }
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
                section_t section;
                f_.seekg(elf.e_shoff + index * sizeof(section_t));
                f_.read((char*)&section, sizeof(section));
            
                if (&names[0] + section.sh_name == section_name) {
                    ptrs_in_section_count = section.sh_size / sizeof(void*);
                    break;
                }
            }                        
        }

        std::vector<symbol_t> symbols;
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
                if (ret.back().empty()) {
                    ret.pop_back(); // Do not show empty names
                }
            }
        }

        return ret;
    }
};

}}} // namespace boost::dll::detail

#endif // BOOST_DLL_DETAIL_POSIX_ELF_INFO_HPP
