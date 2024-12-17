// Copyright Antony Polukhin, 2018-2024.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/dll/config.hpp
/// \brief Imports filesystem, error_code, errc, system_error, make_error_code from Boost or C++17 into `boost::dll::fs` namespace.

#ifndef BOOST_DLL_DETAIL_CONFIG_HPP
#define BOOST_DLL_DETAIL_CONFIG_HPP

#include <boost/config.hpp>
#ifdef BOOST_HAS_PRAGMA_ONCE
#   pragma once
#endif

#ifdef BOOST_DLL_DOXYGEN

/// Define this macro to make Boost.DLL use C++17's std::filesystem::path and std::system_error.
#define BOOST_DLL_USE_STD_FS BOOST_DLL_USE_STD_FS

/// This namespace contains aliases to the Boost or C++17 classes. Aliases are configured using BOOST_DLL_USE_STD_FS macro.
namespace boost { namespace dll { namespace fs {

/// Alias to `std::filesystem::path` if \forcedmacrolink{BOOST_DLL_USE_STD_FS} is defined by user.
/// Alias to `boost::filesystem::path` otherwise.
using path = std::conditional_t<BOOST_DLL_USE_STD_FS, std::filesystem::path, boost::filesystem::path>;

/// Alias to `std::error_code` if \forcedmacrolink{BOOST_DLL_USE_STD_FS} is defined by user.
/// boost::system::error_code otherwise.
using error_code = std::conditional_t<BOOST_DLL_USE_STD_FS, std::error_code, boost::system::error_code>;

/// Alias to `std::system_error` if \forcedmacrolink{BOOST_DLL_USE_STD_FS} is defined by user.
/// Alias to `boost::system::system_error` otherwise.
using system_error = std::conditional_t<BOOST_DLL_USE_STD_FS, std::system_error, boost::system::system_error>;

}}}

#endif

<<<<<<< HEAD
=======

>>>>>>> b753854... fixes
#ifdef BOOST_DLL_USE_STD_FS
#include <filesystem>
#include <system_error>

#include <system_error>

namespace boost { namespace dll { namespace fs {

using namespace std::filesystem;
using std::error_code;
<<<<<<< HEAD
<<<<<<< HEAD
using std::system_error;
=======
>>>>>>> 7d8af2a... wip
=======
using std::system_error;
>>>>>>> af8fbe8... restore boost::system::system_error

}}}

#else // BOOST_DLL_USE_STD_FS

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
<<<<<<< HEAD
<<<<<<< HEAD
#include <boost/system/system_error.hpp>
=======
>>>>>>> 7d8af2a... wip
=======
#include <boost/system/system_error.hpp>
>>>>>>> b753854... fixes
#include <boost/system/error_code.hpp>

namespace boost { namespace dll { namespace fs {

using namespace boost::filesystem;
using boost::system::error_code;
<<<<<<< HEAD
<<<<<<< HEAD
using boost::system::system_error;
=======
>>>>>>> 7d8af2a... wip
=======
using boost::system::system_error;
>>>>>>> af8fbe8... restore boost::system::system_error

}}}

#endif // BOOST_DLL_USE_STD_FS

#endif // BOOST_DLL_DETAIL_PUSH_OPTIONS_HPP

