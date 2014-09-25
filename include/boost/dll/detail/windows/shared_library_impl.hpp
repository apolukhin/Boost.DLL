// shared_library_impl.hpp ---------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2012 Renato Tegon Forti
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 05-04-2012 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_DLL_SHARED_LIBRARY_IMPL_HPP
#define BOOST_DLL_SHARED_LIBRARY_IMPL_HPP

#include <boost/config.hpp>
#include <boost/dll/shared_library_load_mode.hpp>
#include <boost/dll/detail/aggressive_ptr_cast.hpp>
#include <boost/dll/detail/full_module_path_impl.hpp>

#include <boost/move/move.hpp>
#include <boost/swap.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/utility/string_ref.hpp>
#include "boost/detail/no_exceptions_support.hpp"

#include <boost/detail/winapi/dll2.hpp> // TODO: FIXME
#include <boost/detail/winapi/GetLastError.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace dll {

class shared_library_impl {

    BOOST_MOVABLE_BUT_NOT_COPYABLE(shared_library_impl)

    static inline boost::system::error_code last_error_code() BOOST_NOEXCEPT {
        return boost::system::error_code(
            boost::detail::winapi::GetLastError(),
            boost::system::system_category()
        );
    }

public:
    typedef boost::detail::winapi::HMODULE_ native_handle_t;

    shared_library_impl() BOOST_NOEXCEPT
        : handle_(NULL)
    {}

    ~shared_library_impl() BOOST_NOEXCEPT {
        unload();
    }
    
    shared_library_impl(BOOST_RV_REF(shared_library_impl) sl)
    {  
        handle_ = sl.handle_;   sl.handle_ = NULL;  
    }

    shared_library_impl & operator=(BOOST_RV_REF(shared_library_impl) sl)
    {  
        handle_ = sl.handle_; sl.handle_ = NULL; return *this;  
    }

    void load(const boost::filesystem::path &sl, load_mode::type mode, boost::system::error_code &ec) BOOST_NOEXCEPT {
        unload();

        boost::detail::winapi::DWORD_ flags = static_cast<boost::detail::winapi::DWORD_>(mode);

        // From MSDN: If the string specifies a module name without a path and the
        // file name extension is omitted, the function appends the default library
        // extension .dll to the module name.
        //
        // From experiments: Default library extension appended to the modukle name even if
        // we have some path. So we do not check for path, only for extension. We can not be sure that 
        // such behavior remain across all platforms, so we add L".dll" by hand.
        const bool do_append_deco = !!(mode & load_mode::append_native_decorations);
        if (!do_append_deco && sl.has_extension()) {
            handle_ = boost::detail::winapi::LoadLibraryExW(sl.c_str(), 0, flags);
        } else {
            BOOST_TRY {
                // From MSDN: To prevent the function from appending .dll to the module name,
                // include a trailing point character (.) in the module name string.
                if (!do_append_deco) {
                    handle_ = boost::detail::winapi::LoadLibraryExW((sl.native() + L".").c_str(), 0, flags);
                } else {
                    flags &= ~static_cast<boost::detail::winapi::DWORD_>(load_mode::append_native_decorations);
                    handle_ = boost::detail::winapi::LoadLibraryExW((sl.native() + L".dll").c_str(), 0, flags);
                }
            } BOOST_CATCH (...) {
                ec = boost::system::error_code(
                    boost::system::errc::not_enough_memory,
                    boost::system::generic_category()
                );

                return;
            } BOOST_CATCH_END
        }

        if (!handle_) {
            ec = last_error_code();
        }
    }

    void load_self(boost::system::error_code &ec) BOOST_NOEXCEPT {
        unload();

        boost::detail::winapi::WCHAR_ path_hldr[default_path_size];
        boost::detail::winapi::LPCWSTR_ path = path_hldr;

        full_module_path_impl(NULL, path, ec);
        
        if (ec) {
            // Error other than ERROR_INSUFFICIENT_BUFFER_ occured, or failed to allocate buffer big enough
            return;
        }
        
        // here "handle" will be handle of current process!
        handle_ = boost::detail::winapi::LoadLibraryExW(path, 0, 0);
        if (!handle_) {
            ec = last_error_code();
        }

        if (path != path_hldr) {
            delete[] path;
        }
    }

    bool is_loaded() const BOOST_NOEXCEPT {
        return (handle_ != 0);
    }

    void unload() BOOST_NOEXCEPT {
        if (handle_) {
            boost::detail::winapi::FreeLibrary(handle_);
            handle_ = 0;
        }
    }

    void swap(shared_library_impl& rhs) BOOST_NOEXCEPT {
        boost::swap(handle_, rhs.handle_);
    }

    boost::filesystem::path full_module_path(boost::system::error_code &ec) const {
        boost::detail::winapi::WCHAR_ path_hldr[boost::dll::detail::default_path_size];
        boost::detail::winapi::LPCWSTR_ path = path_hldr;

        boost::dll::detail::full_module_path_impl(handle_, path, ec);

        if (ec) {
            // Error other than ERROR_INSUFFICIENT_BUFFER_ occured, or failed to allocate buffer big enough
            return boost::filesystem::path();
        }

        boost::filesystem::path full_module_path(path);

        if (path != path_hldr) {
            delete[] path;
        }

        return full_module_path;
    }

    static boost::filesystem::path suffix() {
        return L".dll";
    }

    void* symbol_addr(boost::string_ref sb, boost::system::error_code &ec) const BOOST_NOEXCEPT {
        // Judging by the documentation and
        // at GetProcAddress there is no version for UNICODE.
        // There can be it and is correct, as in executed
        // units names of functions are stored in narrow characters.
        void* const symbol = boost::dll::detail::aggressive_ptr_cast<void*>(
            boost::detail::winapi::GetProcAddress(handle_, sb.data())
        );
        if (symbol == NULL) {
            ec = last_error_code();
        }

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

