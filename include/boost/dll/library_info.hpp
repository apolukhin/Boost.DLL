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
#include <boost/predef/os.h>

#if BOOST_OS_WINDOWS
#   include <boost/dll/detail/windows/pe_info.hpp>
#else
#   include <boost/dll/detail/posix/elf_info.hpp>
#endif

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace dll {

/*!
* \brief Class that is capable of extracting different information from a library or binary file.
*/
class library_info {
    boost::filesystem::ifstream f_;
#if BOOST_OS_WINDOWS
    typedef boost::dll::detail::pe_info impl_t;
#else
    typedef boost::dll::detail::elf_info impl_t;
#endif

    impl_t  impl_;
public:
    /*!
    * Opens file with specified path and prepares for information extraction.
    * \param library_path Path to the binary file from wich the info must be extracted.
    */
    explicit library_info(const boost::filesystem::path& library_path)
        : f_(library_path, std::ios_base::in | std::ios_base::binary)
        , impl_(f_)
    {
        f_.exceptions(
            boost::filesystem::ifstream::failbit
            | boost::filesystem::ifstream::badbit
            | boost::filesystem::ifstream::eofbit
        );
    }

    /*!
    * \return List of sections that exist in binary file.
    */
    std::vector<std::string> sections() {
        return impl_.sections();
    }

    /*!
    * \return List of all the exportable symbols from all the sections that exist in binary file.
    */
    std::vector<std::string> symbols() {
        return impl_.symbols();
    }

    /*!
    * \param section_name Name of the section from which symbol names must be returned.
    * \return List of symbols from the specified section.
    */
    std::vector<std::string> symbols(boost::string_ref section_name) {
        return impl_.symbols(section_name);
    }
};

}} // namespace boost::dll
#endif // BOOST_DLL_LIBRARY_INFO_HPP
