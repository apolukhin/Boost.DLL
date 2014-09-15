// library_info.hpp ------------------------------------------------------------
// -----------------------------------------------------------------------------

// Copyright 2014 Renato Tegon Forti, Antony Polukhin.

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 20-08-2014 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_DLL_LIBRARY_INFO_HPP
#define BOOST_DLL_LIBRARY_INFO_HPP

#include <boost/config.hpp>
#include <boost/mpl/max_element.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/aligned_storage.hpp>
#include <boost/predef/os.h>
#include <boost/predef/architecture.h>

#include <boost/dll/detail/pe_info.hpp>
#include <boost/dll/detail/elf_info.hpp>
#include <boost/dll/detail/macho_info.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace dll {

/*!
* \brief Class that is capable of extracting different information from a library or binary file.
* Currently understands ELF and PE formats on all the platforms.
*/
class library_info {
private:
    boost::filesystem::ifstream f_;

    boost::aligned_storage< // making my own std::aligned_union from scratch. TODO: move to TypeTraits
        boost::mpl::deref<
            boost::mpl::max_element<
                boost::mpl::vector_c<std::size_t,
                    sizeof(boost::dll::detail::elf_info32),
                    sizeof(boost::dll::detail::elf_info64),
                    sizeof(boost::dll::detail::pe_info32),
                    sizeof(boost::dll::detail::pe_info64),
                    sizeof(boost::dll::detail::macho_info32),
                    sizeof(boost::dll::detail::macho_info64)                    
                >
            >::type
        >::type::value
    >::type impl_;

    boost::dll::detail::x_info_interface& impl() BOOST_NOEXCEPT {
        return *reinterpret_cast<boost::dll::detail::x_info_interface*>(impl_.address());
    }

    static void throw_if_in_32bit() {
#if !BOOST_ARCH_X86_64
        boost::throw_exception(std::runtime_error("Not native format: 64bit binary"));
#endif
    }

    static void throw_if_in_windows() {
#if BOOST_OS_WINDOWS
        boost::throw_exception(std::runtime_error("Not native format: not a PE binary"));
#endif
    }

    static void throw_if_in_posix() {
#if !BOOST_OS_WINDOWS
        boost::throw_exception(std::runtime_error("Not native format: not an ELF binary"));
#endif
    }

    static void throw_if_in_macos() {
#if BOOST_OS_MACOS || BOOST_OS_IOS
        boost::throw_exception(std::runtime_error("Not native format: not an Mach-O binary"));
#endif
    }

    void init(bool throw_if_not_native) {

        if (boost::dll::detail::elf_info32::parsing_supported(f_)) {
            if (throw_if_not_native) { throw_if_in_windows(); throw_if_in_macos(); }

            new (impl_.address()) boost::dll::detail::elf_info32(f_);
        } else if (boost::dll::detail::elf_info64::parsing_supported(f_)) {
            if (throw_if_not_native) { throw_if_in_windows(); throw_if_in_macos(); throw_if_in_32bit(); }

            new (impl_.address()) boost::dll::detail::elf_info64(f_);
        } else if (boost::dll::detail::pe_info32::parsing_supported(f_)) {
            if (throw_if_not_native) { throw_if_in_posix(); throw_if_in_macos(); }

            new (impl_.address()) boost::dll::detail::pe_info32(f_);
        } else if (boost::dll::detail::pe_info64::parsing_supported(f_)) {
            if (throw_if_not_native) { throw_if_in_posix(); throw_if_in_macos(); throw_if_in_32bit(); }

            new (impl_.address()) boost::dll::detail::pe_info64(f_);
        } else if (boost::dll::detail::macho_info32::parsing_supported(f_)) {
            if (throw_if_not_native) { throw_if_in_posix(); throw_if_in_windows(); }

            new (impl_.address()) boost::dll::detail::macho_info32(f_);
        } else if (boost::dll::detail::macho_info64::parsing_supported(f_)) {
            if (throw_if_not_native) { throw_if_in_posix(); throw_if_in_windows(); throw_if_in_32bit(); }

            new (impl_.address()) boost::dll::detail::macho_info64(f_);
        } else {
            boost::throw_exception(std::runtime_error("Unsupported binary format"));
        }
    }

public:
    /*!
    * Opens file with specified path and prepares for information extraction.
    * \param library_path Path to the binary file from wich the info must be extracted.
    * \param throw_if_not_native_format Throw an exception if this file format is not
    * supported by OS.
    */
    explicit library_info(const boost::filesystem::path& library_path, bool throw_if_not_native_format = true)
        : f_(library_path, std::ios_base::in | std::ios_base::binary)
        , impl_()
    {
        f_.exceptions(
            boost::filesystem::ifstream::failbit
            | boost::filesystem::ifstream::badbit
            | boost::filesystem::ifstream::eofbit
        );

        init(throw_if_not_native_format);
    }

    /*!
    * \return List of sections that exist in binary file.
    */
    std::vector<std::string> sections() {
        return impl().sections();
    }

    /*!
    * \return List of all the exportable symbols from all the sections that exist in binary file.
    */
    std::vector<std::string> symbols() {
        return impl().symbols();
    }

    /*!
    * \param section_name Name of the section from which symbol names must be returned.
    * \return List of symbols from the specified section.
    */
    std::vector<std::string> symbols(boost::string_ref section_name) {
        return impl().symbols(section_name);
    }

    ~library_info() BOOST_NOEXCEPT {
        typedef boost::dll::detail::x_info_interface T;
        impl().~T();
    }
};

}} // namespace boost::dll
#endif // BOOST_DLL_LIBRARY_INFO_HPP
