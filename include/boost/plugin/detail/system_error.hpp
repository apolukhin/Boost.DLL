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

    inline void report_error(const boost::system::error_code& ec, const char* message) {
#if !BOOST_OS_WINDOWS
        const char* const error_txt = dlerror();
        if (error_txt) {
            boost::throw_exception(
                boost::system::system_error(
                    ec,
                    message + std::string(" (dlerror system message: ") + error_txt + std::string(")")
                )
            );
        }
#endif

        boost::throw_exception(
            boost::system::system_error(
                ec, message
            )
        );
    }

}}} // boost::plugin::detail

#endif // BOOST_PLUGIN_SYSTEM_ERROR_HPP

