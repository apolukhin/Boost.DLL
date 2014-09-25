// shared_library_impl.hpp ---------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2012 Renato Tegon Forti
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 05-04-2012 dd-mm-yyyy - Initial Release
// 16-09-2013 dd-mm-yyyy - Refatored
//
// -----------------------------------------------------------------------------

#ifndef BOOST_DLL_SHARED_LIBRARY_IMPL_HPP
#define BOOST_DLL_SHARED_LIBRARY_IMPL_HPP

#include <boost/config.hpp>
#include <boost/dll/shared_library_load_mode.hpp>

#include <boost/move/move.hpp>
#include <boost/swap.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/utility/string_ref.hpp>
#include "boost/detail/no_exceptions_support.hpp"

#include <dlfcn.h>

// for dlinfo
#include <dlfcn.h>
#include <link.h>
#include <limits.h>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace dll {

class shared_library_impl {

    BOOST_MOVABLE_BUT_NOT_COPYABLE(shared_library_impl)

public:
    typedef void* native_handle_t;

    shared_library_impl() BOOST_NOEXCEPT
        : handle_(NULL)
    {}

    ~shared_library_impl() BOOST_NOEXCEPT {
        unload();
    }
    
    shared_library_impl(BOOST_RV_REF(shared_library_impl) sl)
    {  
        handle_ = sl.handle_; sl.handle_ = NULL;  
    }

    shared_library_impl & operator=(BOOST_RV_REF(shared_library_impl) sl)
    {  
        handle_ = sl.handle_; sl.handle_ = NULL; return *this;  
    }

    void load(const boost::filesystem::path &sl, load_mode::type mode, boost::system::error_code &ec) BOOST_NOEXCEPT {
        unload();

        // Do not allow opening NULL paths. User must use load_self() instead
        if (sl.empty()) {
            ec = boost::system::error_code(
                boost::system::errc::bad_file_descriptor,
                boost::system::generic_category()
            );

            return;
        }

        // Fixing modes
        if (!(mode & RTLD_LAZY || mode & RTLD_NOW)) {
            mode |= load_mode::rtld_lazy;
        }

        if (!(mode & RTLD_LOCAL || mode & RTLD_GLOBAL)) {
            mode |= load_mode::rtld_local;
        }

        if (!(mode & load_mode::append_native_decorations)) {
            handle_ = dlopen(sl.c_str(), static_cast<int>(mode));
        } else {
            BOOST_TRY {
                handle_ = dlopen(
                    // Apple requires '.so' extension
                    ((sl.parent_path() / "lib").native() + sl.filename().native() + ".so").c_str(),
                    static_cast<int>(mode) & (~static_cast<int>(load_mode::append_native_decorations))
                );
            } BOOST_CATCH (...) {
                ec = boost::system::error_code(
                    boost::system::errc::not_enough_memory,
                    boost::system::generic_category()
                );

                return;
            } BOOST_CATCH_END
        }

        if (!handle_) {
            ec = boost::system::error_code(
                boost::system::errc::bad_file_descriptor,
                boost::system::generic_category()
            );
        }
    }

    void load_self(boost::system::error_code &ec) BOOST_NOEXCEPT {
        unload();

        // As is known the function dlopen() loads the dynamic library file 
        // named by the null-terminated string filename and returns an opaque 
        // "handle" for the dynamic library. If filename is NULL, then the 
        // returned handle is for the main program.

        handle_ = dlopen(NULL, RTLD_LAZY | RTLD_LOCAL);

        if (!handle_) {
            ec = boost::system::error_code(
                boost::system::errc::bad_file_descriptor,
                boost::system::generic_category()
            );
        }
    }

    bool is_loaded() const BOOST_NOEXCEPT {
        return (handle_ != 0);
    }

    void unload() BOOST_NOEXCEPT {
        if (!is_loaded()) {
            return;
        }

        dlclose(handle_);
        handle_ = 0;
    }

    void swap(shared_library_impl& rhs) BOOST_NOEXCEPT {
        boost::swap(handle_, rhs.handle_);
    }

    boost::filesystem::path full_module_path(boost::system::error_code &ec) const BOOST_NOEXCEPT {
        // RTLD_DI_LINKMAP (RTLD_DI_ORIGIN returns only folder and is not suitable for this case)
        // Obtain the Link_map for the handle  that  is  specified.
        // The  p  argument  points to a Link_map pointer (Link_map
        // **p). The actual storage for the Link_map  structure  is
        // maintained by ld.so.1.
        const struct link_map * link_map;
        if (dlinfo(handle_, RTLD_DI_LINKMAP, &link_map) < 0) {
            ec = boost::system::error_code(
                boost::system::errc::bad_file_descriptor,
                boost::system::generic_category()
            );

            return boost::filesystem::path();
        }

        return boost::filesystem::path(link_map->l_name);
    }

    static boost::filesystem::path suffix() {
        // https://sourceforge.net/p/predef/wiki/OperatingSystems/
#if defined(__APPLE__)
        return ".dylib";
#else
        return ".so";
#endif
    }

    void* symbol_addr(boost::string_ref sb, boost::system::error_code &ec) const BOOST_NOEXCEPT {
        // dlsym - obtain the address of a symbol from a dlopen object
        void* const symbol = dlsym(handle_, sb.data());
        if (symbol == NULL) {
            ec = boost::system::error_code(
                boost::system::errc::invalid_seek,
                boost::system::generic_category()
            );
        }

        // If handle does not refer to a valid object opened by dlopen(),
        // or if the named symbol cannot be found within any of the objects
        // associated with handle, dlsym() shall return NULL.
        // More detailed diagnostic information shall be available through dlerror().

        return symbol;
    }

    native_handle_t native() const BOOST_NOEXCEPT {
        return handle_;
    }

private:
    native_handle_t handle_;
};

}} // boost::dll

#endif // BOOST_DLL_SHARED_LIBRARY_IMPL_HPP

