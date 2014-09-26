// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_DLL_DETAIL_POSIX_PATH_FROM_HANDLE_HPP
#define BOOST_DLL_DETAIL_POSIX_PATH_FROM_HANDLE_HPP

#include <boost/config.hpp>
#include <boost/dll/detail/system_error.hpp>
#include <boost/filesystem/path.hpp>

// for dlinfo
#include <dlfcn.h>
#include <link.h>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace dll { namespace detail {

    inline boost::filesystem::path path_from_handle(void* handle, boost::system::error_code &ec)  {
        // RTLD_DI_LINKMAP (RTLD_DI_ORIGIN returns only folder and is not suitable for this case)
        // Obtain the Link_map for the handle  that  is  specified.
        // The  p  argument  points to a Link_map pointer (Link_map
        // **p). The actual storage for the Link_map  structure  is
        // maintained by ld.so.1.
        const struct link_map * link_map;
        if (dlinfo(handle, RTLD_DI_LINKMAP, &link_map) < 0) {
            ec = boost::system::error_code(
                boost::system::errc::bad_file_descriptor,
                boost::system::generic_category()
            );

            return boost::filesystem::path();
        }

        return boost::filesystem::path(link_map->l_name);
    }

}}} // namespace boost::dll::detail

#endif // BOOST_DLL_DETAIL_POSIX_PATH_FROM_HANDLE_HPP
