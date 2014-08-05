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

#ifndef BOOST_APPLICATION_SHARED_LIBRARY_IMPL_HPP
#define BOOST_APPLICATION_SHARED_LIBRARY_IMPL_HPP

#include <boost/application/config.hpp>
#include <boost/application/shared_library_types.hpp>
#include <boost/application/shared_library_load_mode.hpp>

#include <boost/noncopyable.hpp>
#include <boost/swap.hpp>
#include <boost/filesystem/path.hpp>

namespace boost { namespace application {

class shared_library_impl : noncopyable {
public:
    shared_library_impl() BOOST_NOEXCEPT
        : handle_(NULL)
    {}

    ~shared_library_impl() BOOST_NOEXCEPT {
        unload();
    }

    static shared_library_load_mode default_mode() BOOST_NOEXCEPT {
        return load_library_default_mode;
    }

    void load(const library_path &sh, shared_library_load_mode mode, boost::system::error_code &ec) BOOST_NOEXCEPT {
        unload();

        DWORD flags = static_cast<DWORD>(mode);
        handle_ = LoadLibraryExW(sh.c_str(), 0, flags);
        if (!handle_) {
            ec = boost::application::last_error_code();
        }
    }

    bool is_loaded() const BOOST_NOEXCEPT {
        return (handle_ != 0);
    }

    void unload() BOOST_NOEXCEPT {
        if (handle_) {
            FreeLibrary(handle_);
            handle_ = 0;
        }
    }

    void swap(shared_library_impl& rhs) BOOST_NOEXCEPT {
        boost::swap(handle_, rhs.handle_);
    }

    static character_types::string_type suffix() {
#if defined(BOOST_APPLICATION_STD_WSTRING)
        return character_types::string_type(L".dll");
#else
        return character_types::string_type(".dll");
#endif
    }

    void* symbol_addr(const symbol_type &sb, boost::system::error_code &ec) const BOOST_NOEXCEPT {
        // Judging by the documentation and
        // at GetProcAddress there is no version for UNICODE.
        // There can be it and is correct, as in executed
        // units names of functions are stored in narrow characters.

        if (handle_) {
            return (void*) GetProcAddress(handle_, sb.data());
        } else {
            ec = boost::application::last_error_code();
        }

        return NULL;
    }

private:
    HMODULE handle_;
};

}} // boost::application

#endif // BOOST_APPLICATION_SHARED_LIBRARY_IMPL_HPP

