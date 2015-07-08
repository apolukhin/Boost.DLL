// Copyright 2014-2015 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_DLL_DETAIL_POSIX_PATH_FROM_HANDLE_HPP
#define BOOST_DLL_DETAIL_POSIX_PATH_FROM_HANDLE_HPP

#include <boost/config.hpp>
#include <boost/dll/detail/system_error.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/predef/os.h>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

// for dlinfo
#include <dlfcn.h>

#if BOOST_OS_MACOS
#   include <mach-o/dyld.h>
#   include <mach-o/nlist.h>
#   include <cstddef> // for std::ptrdiff_t

namespace boost { namespace dll { namespace detail {
    inline void* strip_handle(void* handle) BOOST_NOEXCEPT {
        return reinterpret_cast<void*>(
            (reinterpret_cast<std::ptrdiff_t>(handle) >> 2) << 2
        );
    }

    inline boost::filesystem::path path_from_handle(void* handle, boost::system::error_code &ec) {
        handle = strip_handle(handle);

        // Iterate through all images currently in memory
        for (std::size_t i = _dyld_image_count(); i >= 0; --i) {
            // dlopen() each image, check handle
            const char *image_name = _dyld_get_image_name(i);
            void* probe_handle = dlopen(image_name, RTLD_LAZY);
            dlclose(probe_handle);

            // If the handle is the same as what was passed in (modulo mode bits), return this image name
            if (handle == strip_handle(probe_handle))
                return image_name;
        }

        ec = boost::system::error_code(
            boost::system::errc::bad_file_descriptor,
            boost::system::generic_category()
        );

        return boost::filesystem::path();
    }

}}} // namespace boost::dll::detail

#else // #if BOOST_OS_MACOS
#   include <link.h>

namespace boost { namespace dll { namespace detail {

    inline boost::filesystem::path path_from_handle(void* handle, boost::system::error_code &ec) {
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

#endif // #if BOOST_OS_MACOS

#endif // BOOST_DLL_DETAIL_POSIX_PATH_FROM_HANDLE_HPP


