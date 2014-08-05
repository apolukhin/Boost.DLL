// system_error.hpp ----------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See http://www.boost.org/libs/application for documentation.

// -----------------------------------------------------------------------------

// Revision History
// 18-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_PLUGIN_SYSTEM_ERROR_HPP
#define BOOST_PLUGIN_SYSTEM_ERROR_HPP

#include <boost/config.hpp>
#include <boost/predef/os.h>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/throw_exception.hpp>

#if BOOST_OS_WINDOWS
#   include <boost/detail/winapi/GetLastError.hpp>
#else
#   include <errno.h>
#endif

namespace boost { namespace plugin { namespace detail {

    inline boost::system::error_code last_error_code() BOOST_NOEXCEPT {
        // dl* functions report errors using NULL and dlerror()
        return boost::system::error_code(
#if BOOST_OS_WINDOWS
            boost::detail::winapi::GetLastError(),
            boost::system::system_category()
#else
            boost::system::errc::bad_file_descriptor, 
            boost::system::generic_category()
#endif
        );
    }

    inline void report_error(const boost::system::error_code& ec, const char* message) {
        // TODO: error category for DSO
        boost::throw_exception(
            boost::system::system_error(
                ec, 
#if BOOST_OS_WINDOWS
                message
#else
                message + std::string(". System message (dlerror): ") + dlerror() + std::string(". //")
#endif
            )
        );
    }

}}} // boost::plugin::detail

#endif // BOOST_PLUGIN_SYSTEM_ERROR_HPP

