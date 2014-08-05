// shared_library_types.hpp --------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2012 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 05-04-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_SHARED_LIBRARY_TYPES_HPP
#define BOOST_APPLICATION_SHARED_LIBRARY_TYPES_HPP

#include <boost/application/config.hpp>
#include <boost/application/base_type.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/utility/string_ref.hpp>

namespace boost { namespace application {

   /*!
    * \brief This is a path.
    *
    * It is used as param on shared_library
    * class, and using it many types can be
    * handled, e.g. wide string, string, paths
    * and so on.
    *
    */
   typedef boost::filesystem::path library_path;

   /*!
    * \brief This is a symbol_type.
    *
    * It is used as param on shared_library
    * class, and using it many types can be
    * handled, e.g. wide string, string
    * and so on.
    *
    */
   typedef boost::string_ref symbol_type;

}} // boost::application

#endif // BOOST_APPLICATION_SHARED_LIBRARY_TYPES_HPP

