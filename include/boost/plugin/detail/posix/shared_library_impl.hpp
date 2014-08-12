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

#ifndef BOOST_PLUGIN_SHARED_LIBRARY_IMPL_HPP
#define BOOST_PLUGIN_SHARED_LIBRARY_IMPL_HPP

#include <boost/config.hpp>
#include <boost/plugin/shared_library_types.hpp>
#include <boost/plugin/shared_library_load_mode.hpp>

#include <boost/noncopyable.hpp>
#include <boost/move/move.hpp>
#include <boost/swap.hpp>

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <fcntl.h>
#include <sys/resource.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>

namespace boost { namespace plugin {

class shared_library_impl : noncopyable {

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

    void load(const library_path &sl, load_mode::type mode, boost::system::error_code &ec) BOOST_NOEXCEPT {
        unload();

        // Fixing modes
        if (!(mode & RTLD_LAZY || mode & RTLD_NOW)) {
            mode |= load_mode::rtld_lazy;
        }

        if (!(mode & RTLD_LOCAL || mode & RTLD_GLOBAL)) {
            mode |= load_mode::rtld_local;
        }

        handle_ = dlopen(sl.c_str(), static_cast<int>(mode));
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

        handle_ = dlopen(NULL, DLOPEN_LAZY | DLOPEN_GLOBAL | RTLD_DEEPBIND);

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

    static library_path suffix() {
        // https://sourceforge.net/p/predef/wiki/OperatingSystems/
#if defined(__APPLE__)
        return ".dylib";
#else
        return ".so";
#endif
    }

    void* symbol_addr(const symbol_type &sb, boost::system::error_code &ec) const BOOST_NOEXCEPT {
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

}} // boost::plugin

#endif // BOOST_PLUGIN_SHARED_LIBRARY_IMPL_HPP

