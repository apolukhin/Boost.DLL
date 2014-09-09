// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/dll/detail/posix/elf_info.hpp>
#include <boost/dll/detail/windows/pe_info.hpp>
#include <boost/static_assert.hpp>
#include <boost/test/minimal.hpp>

#if BOOST_OS_WINDOWS
    #include <windows.h>
#else
    #include <elf.h>
#endif

namespace dd = boost::dll::detail;

template <class T1, class T2>
inline std::size_t get_offset(const T1& v1, const T2& v2) {
    const unsigned char* p1 = reinterpret_cast<const unsigned char*>(&v1);
    const unsigned char* p2 = reinterpret_cast<const unsigned char*>(&v2);

    if (p1 < p2) {
        return static_cast<std::size_t>(p2 - p1);
    }

    return static_cast<std::size_t>(p1 - p2);
}

#define CHECK_FIELD(Field)                                              \
    BOOST_STATIC_ASSERT(sizeof(v1.Field) == sizeof(v2.Field));          \
    BOOST_CHECK(get_offset(v1, v1.Field) == get_offset(v2, v2.Field))   \
    /**/


// ELF structures
template <class T1, class T2>
void elf_header_checks(const T1& v1, const T2& v2) {
    BOOST_STATIC_ASSERT(sizeof(T1) == sizeof(T2));

    CHECK_FIELD(e_ident);
    CHECK_FIELD(e_type);
    CHECK_FIELD(e_machine);
    CHECK_FIELD(e_version);
    CHECK_FIELD(e_entry);
    CHECK_FIELD(e_phoff);
    CHECK_FIELD(e_shoff);
    CHECK_FIELD(e_flags);
    CHECK_FIELD(e_ehsize);
    CHECK_FIELD(e_phentsize);
    CHECK_FIELD(e_phnum);
    CHECK_FIELD(e_shentsize);
    CHECK_FIELD(e_shnum);
    CHECK_FIELD(e_shstrndx);
}

template <class T1, class T2>
void elf_sheader_checks(const T1& v1, const T2& v2) {
    BOOST_STATIC_ASSERT(sizeof(T1) == sizeof(T2));

    CHECK_FIELD(sh_name);
    CHECK_FIELD(sh_type);
    CHECK_FIELD(sh_flags);
    CHECK_FIELD(sh_addr);
    CHECK_FIELD(sh_offset);
    CHECK_FIELD(sh_size);
    CHECK_FIELD(sh_link);
    CHECK_FIELD(sh_info);
    CHECK_FIELD(sh_addralign);
    CHECK_FIELD(sh_entsize);
}

template <class T1, class T2>
void elf_sym_header_checks(const T1& v1, const T2& v2) {
    BOOST_STATIC_ASSERT(sizeof(T1) == sizeof(T2));

    CHECK_FIELD(st_name);
    CHECK_FIELD(st_value);
    CHECK_FIELD(st_size);
    CHECK_FIELD(st_info);
    CHECK_FIELD(st_other);
    CHECK_FIELD(st_shndx);
}


// PE structures
template <class T1, class T2>
void dos_header_checks(const T1& v1, const T2& v2) {
    BOOST_STATIC_ASSERT(sizeof(T1) == sizeof(T2));

/*
    boost::dll::detail::WORD_    e_magic;        // Magic number
    boost::dll::detail::WORD_    e_cblp;         // Bytes on last page of file
    boost::dll::detail::WORD_    e_cp;           // Pages in file
    boost::dll::detail::WORD_    e_crlc;         // Relocations
    boost::dll::detail::WORD_    e_cparhdr;      // Size of header in paragraphs
    boost::dll::detail::WORD_    e_minalloc;     // Minimum extra paragraphs needed
    boost::dll::detail::WORD_    e_maxalloc;     // Maximum extra paragraphs needed
    boost::dll::detail::WORD_    e_ss;           // Initial (relative) SS value
    boost::dll::detail::WORD_    e_sp;           // Initial SP value
    boost::dll::detail::WORD_    e_csum;         // Checksum
    boost::dll::detail::WORD_    e_ip;           // Initial IP value
    boost::dll::detail::WORD_    e_cs;           // Initial (relative) CS value
    boost::dll::detail::WORD_    e_lfarlc;       // File address of relocation table
    boost::dll::detail::WORD_    e_ovno;         // Overlay number
    boost::dll::detail::WORD_    e_res[4];       // Reserved words
    boost::dll::detail::WORD_    e_oemid;        // OEM identifier (for e_oeminfo)
    boost::dll::detail::WORD_    e_oeminfo;      // OEM information; e_oemid specific
    boost::dll::detail::WORD_    e_res2[10];     // Reserved words
    boost::dll::detail::LONG_    e_lfanew;       // File address of new exe header
    CHECK_FIELD(st_name);
*/
}

// Unit Tests
int test_main(int argc, char* argv[]) {

#if BOOST_OS_WINDOWS

#else
    elf_header_checks(::Elf32_Ehdr(), dd::Elf32_Ehdr_());
    elf_header_checks(::Elf64_Ehdr(), dd::Elf64_Ehdr_());

    elf_sheader_checks(::Elf32_Shdr(), dd::Elf32_Shdr_());
    elf_sheader_checks(::Elf64_Shdr(), dd::Elf64_Shdr_());

    elf_sym_header_checks(::Elf32_Sym(), dd::Elf32_Sym_());
    elf_sym_header_checks(::Elf64_Sym(), dd::Elf64_Sym_());
#endif 

    return 0;
}

