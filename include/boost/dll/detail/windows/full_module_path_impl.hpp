// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_DLL_DETAIL_FULL_MODULE_PATH_IMPL_HPP
#define BOOST_DLL_DETAIL_FULL_MODULE_PATH_IMPL_HPP

#include <boost/config.hpp>
#include <boost/dll/detail/system_error.hpp>
#include <boost/detail/winapi/dll2.hpp> // TODO: FIXME

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace dll { namespace detail {

    BOOST_STATIC_CONSTANT(boost::detail::winapi::DWORD_, default_path_size = 260);

    static inline void full_module_path_impl(
            boost::detail::winapi::HMODULE_ h,
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

}}} // namespace boost::dll::detail

#endif // BOOST_DLL_DETAIL_FULL_MODULE_PATH_IMPL_HPP

