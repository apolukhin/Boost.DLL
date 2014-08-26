// library_info.hpp ------------------------------------------------------------
// -----------------------------------------------------------------------------

// Copyright 2014 Renato Tegon Forti, Antony Polukhin.

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 20-08-2014 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_PLUGIN_LIBRARY_INFO_HPP
#define BOOST_PLUGIN_LIBRARY_INFO_HPP

#include <boost/config.hpp>
#include <boost/predef/os.h>

#if BOOST_OS_WINDOWS
#   include <boost/plugin/detail/windows/library_info.hpp>
#else
#   include <boost/plugin/detail/posix/library_info.hpp>
#endif

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

#endif // BOOST_PLUGIN_LIBRARY_INFO_HPP
