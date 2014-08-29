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

#ifndef BOOST_PLUGIN_SHARED_LIBRARY_IMPL_HPP
#define BOOST_PLUGIN_SHARED_LIBRARY_IMPL_HPP

#include <boost/config.hpp>
#include <boost/plugin/shared_library_load_mode.hpp>
#include <boost/plugin/detail/aggressive_ptr_cast.hpp>

#include <boost/move/move.hpp>
#include <boost/swap.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/utility/string_ref.hpp>

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

    void load(const boost::filesystem::path &sh, load_mode::type mode, boost::system::error_code &ec) BOOST_NOEXCEPT {
        unload();

        boost::detail::winapi::DWORD_ flags = static_cast<boost::detail::winapi::DWORD_>(mode);
        handle_ = boost::detail::winapi::LoadLibraryExW(sh.c_str(), 0, flags);
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

private:
    // 260 == MAX_PATH
    BOOST_STATIC_CONSTANT(boost::detail::winapi::DWORD_, default_path_size = 260);

    static void full_module_path_impl(
            native_handle_t h,
            boost::detail::winapi::LPCWSTR_& path,
            boost::system::error_code &ec) BOOST_NOEXCEPT
    {
        const boost::detail::winapi::DWORD_ ERROR_INSUFFICIENT_BUFFER_ = 0x7A;

        // A handle to the loaded module whose path is being requested. 
        // If this parameter is NULL, GetModuleFileName retrieves the path of the 
        // executable file of the current process.
        boost::detail::winapi::GetModuleFileNameW(h, path, default_path_size);
        ec = last_error_code();

        // In case of ERROR_INSUFFICIENT_BUFFER_ trying to get buffer big enough to store the whole path
        for (unsigned i = 2; i < 1025 && ec.value() == ERROR_INSUFFICIENT_BUFFER_; i *= 2) {
            path = new(std::nothrow) boost::detail::winapi::WCHAR_[default_path_size * i];
            if (!path) {
                ec = boost::system::error_code(
                    boost::system::errc::not_enough_memory,
                    boost::system::generic_category()
                );
                return;
            }

            boost::detail::winapi::GetModuleFileNameW(h, path, default_path_size * i);
            ec = last_error_code();

            if (ec) {
                delete[] path;
            }
        }
    }

public:
    boost::filesystem::path full_module_path(boost::system::error_code &ec) const {
        boost::detail::winapi::WCHAR_ path_hldr[default_path_size];
        boost::detail::winapi::LPCWSTR_ path = path_hldr;

        full_module_path_impl(handle_, path, ec);

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

    void* symbol_addr(const boost::string_ref &sb, boost::system::error_code &ec) const BOOST_NOEXCEPT {
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

#endif // BOOST_PLUGIN_SHARED_LIBRARY_IMPL_HPP

