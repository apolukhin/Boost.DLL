// Copyright 2014 Renato Tegon Forti, Antony Polukhin.

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 15-09-2014 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_DLL_DETAIL_MACHO_INFO_HPP
#define BOOST_DLL_DETAIL_MACHO_INFO_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

#include <boost/filesystem/fstream.hpp>
#include <boost/dll/detail/x_info_interface.hpp>

namespace boost { namespace dll { namespace detail {

typedef int integer_t;
typedef int vm_prot_t;
typedef integer_t cpu_type_t;
typedef integer_t cpu_subtype_t;

template <class AddressOffsetT>
struct mach_header_template {
    uint32_t        magic;
    cpu_type_t      cputype;
    cpu_subtype_t   cpusubtype;
    uint32_t        filetype;
    uint32_t        ncmds;
    uint32_t        sizeofcmds;
    uint32_t        flags[sizeof(AddressOffsetT) / sizeof(uint32_t)]; // Flags and reserved
};

typedef mach_header_template<boost::uint32_t> mach_header_32_;
typedef mach_header_template<boost::uint64_t> mach_header_64_;

struct load_command_ {
    uint32_t        cmd;        /* type of command */
    uint32_t        cmdsize;
};

struct load_command_types {
    BOOST_STATIC_CONSTANT(uint32_t, LC_SEGMENT          = 0x1);   /* segment of this file to be mapped */
    BOOST_STATIC_CONSTANT(uint32_t, LC_SYMTAB           = 0x2);   /* link-edit stab symbol table info */
    BOOST_STATIC_CONSTANT(uint32_t, LC_SYMSEG           = 0x3);   /* link-edit gdb symbol table info (obsolete) */
    BOOST_STATIC_CONSTANT(uint32_t, LC_THREAD           = 0x4);   /* thread */
    BOOST_STATIC_CONSTANT(uint32_t, LC_UNIXTHREAD       = 0x5);   /* unix thread (includes a stack) */
    BOOST_STATIC_CONSTANT(uint32_t, LC_LOADFVMLIB       = 0x6);   /* load a specified fixed VM shared library */
    BOOST_STATIC_CONSTANT(uint32_t, LC_IDFVMLIB         = 0x7);   /* fixed VM shared library identification */
    BOOST_STATIC_CONSTANT(uint32_t, LC_IDENT            = 0x8);   /* object identification info (obsolete) */
    BOOST_STATIC_CONSTANT(uint32_t, LC_FVMFILE          = 0x9);   /* fixed VM file inclusion (internal use) */
    BOOST_STATIC_CONSTANT(uint32_t, LC_PREPAGE          = 0xa);   /* prepage command (internal use) */
    BOOST_STATIC_CONSTANT(uint32_t, LC_DYSYMTAB         = 0xb);   /* dynamic link-edit symbol table info */
    BOOST_STATIC_CONSTANT(uint32_t, LC_LOAD_DYLIB       = 0xc);   /* load a dynamically linked shared library */
    BOOST_STATIC_CONSTANT(uint32_t, LC_ID_DYLIB         = 0xd);   /* dynamically linked shared lib ident */
    BOOST_STATIC_CONSTANT(uint32_t, LC_LOAD_DYLINKER    = 0xe);   /* load a dynamic linker */
    BOOST_STATIC_CONSTANT(uint32_t, LC_ID_DYLINKER      = 0xf);   /* dynamic linker identification */
    BOOST_STATIC_CONSTANT(uint32_t, LC_PREBOUND_DYLIB   = 0x10);  /* modules prebound for a dynamically linked shared library */
    BOOST_STATIC_CONSTANT(uint32_t, LC_ROUTINES         = 0x11);  /* image routines */
    BOOST_STATIC_CONSTANT(uint32_t, LC_SUB_FRAMEWORK    = 0x12);  /* sub framework */
    BOOST_STATIC_CONSTANT(uint32_t, LC_SUB_UMBRELLA     = 0x13);  /* sub umbrella */
    BOOST_STATIC_CONSTANT(uint32_t, LC_SUB_CLIENT       = 0x14);  /* sub client */
    BOOST_STATIC_CONSTANT(uint32_t, LC_SUB_LIBRARY      = 0x15);  /* sub library */
    BOOST_STATIC_CONSTANT(uint32_t, LC_TWOLEVEL_HINTS   = 0x16);  /* two-level namespace lookup hints */
    BOOST_STATIC_CONSTANT(uint32_t, LC_PREBIND_CKSUM    = 0x17);  /* prebind checksum */
};

template <class AddressOffsetT>
struct segment_command_template {
    uint32_t        cmd;            /* LC_SEGMENT */
    uint32_t        cmdsize;        /* includes sizeof section structs */
    char            segname[16];    /* segment name */
    AddressOffsetT  vmaddr;         /* memory address of this segment */
    AddressOffsetT  vmsize;         /* memory size of this segment */
    AddressOffsetT  fileoff;        /* file offset of this segment */
    AddressOffsetT  filesize;       /* amount to map from the file */
    vm_prot_t       maxprot;        /* maximum VM protection */
    vm_prot_t       initprot;       /* initial VM protection */
    uint32_t        nsects;         /* number of sections in segment */
    uint32_t        flags;          /* flags */
};

typedef segment_command_template<boost::uint32_t> segment_command_32_;
typedef segment_command_template<boost::uint64_t> segment_command_64_;

template <class AddressOffsetT>
struct section_template { /* for 32-bit architectures */
    char                sectname[16];   /* name of this section */
    char                segname[16];    /* segment this section goes in */
    AddressOffsetT      addr;           /* memory address of this section */
    AddressOffsetT      size;           /* size in bytes of this section */
    uint32_t            offset;         /* file offset of this section */
    uint32_t            align;          /* section alignment (power of 2) */
    uint32_t            reloff;         /* file offset of relocation entries */
    uint32_t            nreloc;         /* number of relocation entries */
    uint32_t            flags;          /* flags (section type and attributes)*/
    uint32_t            reserved[1 + sizeof(AddressOffsetT) / sizeof(uint32_t)];
};

typedef section_template<boost::uint32_t> section_32_;
typedef section_template<boost::uint64_t> section_64_;

struct symtab_command {
    uint32_t    cmd;        /* LC_SYMTAB */
    uint32_t    cmdsize;    /* sizeof(struct symtab_command) */
    uint32_t    symoff;     /* symbol table offset */
    uint32_t    nsyms;      /* number of symbol table entries */
    uint32_t    stroff;     /* string table offset */
    uint32_t    strsize;    /* string table size in bytes */
};

template <class AddressOffsetT>
class macho_info: public x_info_interface {
    boost::filesystem::ifstream& f_;

    typedef boost::dll::detail::mach_header_template<AddressOffsetT>        header_t;
    typedef boost::dll::detail::load_command_                               load_command_t;
    typedef boost::dll::detail::segment_command_template<AddressOffsetT>    segment_t;
    typedef boost::dll::detail::section_template<AddressOffsetT>            section_t;

public:
    static bool parsing_supported(boost::filesystem::ifstream& f) {
        static const uint32_t magic_bytes = (sizeof(AddressOffsetT) <= sizeof(uint32_t) ? 0xfeedface : 0xfeedfacf);

        uint32_t magic;
        f.seekg(0);
        f.read((char*)&magic, sizeof(magic));
        return (magic_bytes == magic);
    }

    explicit macho_info(boost::filesystem::ifstream& f) BOOST_NOEXCEPT
        : f_(f)
    {}

    std::vector<std::string> sections() {
        std::vector<std::string> ret;

        const header_t h = header();
        load_command_t command;
        f_.seekg(sizeof(header_t));
        for (std::size_t i = 0; i < h.ncmds; ++i) {
            f_.read((char*)&command, sizeof(command));
            if (command.cmd != load_command_types::LC_SEGMENT) {
                f_.seekg(f_.tellg() + static_cast<boost::filesystem::ifstream::pos_type>(command.cmdsize));
                continue;
            }

            f_.seekg(static_cast<boost::filesystem::ifstream::pos_type>(
                f_.tellg() - static_cast<boost::filesystem::ifstream::pos_type>(sizeof(command))
            ));
            segment_t segment;
            f_.read((char*)&segment, sizeof(segment));

            section_t section;
            ret.reserve(ret.size() + segment.nsects);
            for (std::size_t j = 0; j < segment.nsects; ++j) {
                f_.read((char*)&section, sizeof(section));
                // `segname` goes right after the `sectname`.
                // Forsing `sectname` to end on '\0'
                section.segname[0] = '\0';  
                ret.push_back(section.sectname);
                if (ret.back().empty()) {
                    ret.pop_back(); // Do not show empty names
                }
            }
        }

        return ret;
    }

private:
    inline header_t header() {
        header_t h;

        f_.seekg(0);
        f_.read((char*)&h, sizeof(h));

        return h;
    }

public:
    std::vector<std::string> symbols() {
        std::vector<std::string> ret;
        boost::throw_exception(std::runtime_error("macho_info::symbols() is not implemented"));
        return ret;
    }

    std::vector<std::string> symbols(boost::string_ref section_name) {
        std::vector<std::string> ret;
        boost::throw_exception(std::runtime_error("macho_info::symbols(boost::string_ref) is not implemented"));
        return ret;
    }
};

typedef macho_info<boost::uint32_t> macho_info32;
typedef macho_info<boost::uint64_t> macho_info64;

}}} // namespace boost::dll::detail

#endif // BOOST_DLL_DETAIL_MACHO_INFO_HPP
