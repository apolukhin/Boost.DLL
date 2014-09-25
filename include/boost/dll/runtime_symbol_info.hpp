// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org

#ifndef BOOST_DLL_RUNTIME_SYMBOL_INFO_HPP
#define BOOST_DLL_RUNTIME_SYMBOL_INFO_HPP

#include <boost/config.hpp>
#include <boost/dll/detail/aggressive_ptr_cast.hpp>
#if BOOST_OS_WINDOWS
#   include <boost/detail/winapi/dll2.hpp> // TODO: FIXME
#   include <boost/dll/detail/windows/full_module_path_impl.hpp>
#else
#   include <dlfcn.h> // Not suitable for Windows
#endif

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace dll {

namespace detail {
#if BOOST_OS_WINDOWS
    inline boost::filesystem::path symbol_location_impl(const void* symbol) {
        boost::filesystem::path ret;

        boost::detail::winapi::MEMORY_BASIC_INFORMATION_ mbi;
        if (!boost::detail::winapi::VirtualQuery(symbol, &mbi, sizeof(mbi))) {
            return ret;
        }

        boost::detail::winapi::WCHAR_ path_hldr[boost::dll::detail::default_path_size];
        boost::detail::winapi::LPCWSTR_ path = path_hldr;
        boost::system::error_code ec;
        boost::dll::detail::full_module_path_impl(reinterpret_cast<boost::detail::winapi::HMODULE_>(mbi.AllocationBase), path, ec);

        if (ec) {
            // Error other than ERROR_INSUFFICIENT_BUFFER_ occured, or failed to allocate buffer big enough
            return ret;
        }

        ret = path;
        if (path != path_hldr) {
            delete[] path;
        }

        return ret;
    }
#else
    inline boost::filesystem::path symbol_location_impl(const void* symbol) {
        Dl_info info;
        const int res = dladdr(symbol, &info);
        return res ? boost::filesystem::path(info.dli_fname) : boost::filesystem::path();
    }
#endif
} // namespace detail

    /*!
    * On success returns full path and name of the binary object that holds symbol.
    * \tparam T Type of the symbol, must not be explicitly specified.
    * \param symbol Symbol which location is to be determinated.
    * \return Path to the binary object that holds symbol or empty path in case error.
    * \throws std::bad_alloc in case of insufficient memory.
    */
    template <class T>
    inline boost::filesystem::path symbol_location(const T& symbol) {
        return boost::dll::detail::symbol_location_impl(
            boost::dll::detail::aggressive_ptr_cast<const void*>(boost::addressof(symbol))
        );
    }

}} // namespace boost::dll

#endif // BOOST_DLL_RUNTIME_SYMBOL_INFO_HPP

