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
#   include <windows.h>
#else
#   include <dlfcn.h> // Not suitable for Windows
#endif

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace dll {

#if BOOST_OS_WINDOWS
    // TODO: stopped here
    boost::filesystem::path symbol_location(const void* symbol) {    
        const size_t size = 1024;
        boost::detail::winapi::LPTWSTR_ buf = 0;
     
        boost::detail::winapi::MEMORY_BASIC_INFORMATION_ mbi;
        const boost::detail::winapi::SIZE_T_ res = boost::detail::winapi::VirtualQuery(&symbol, &mbi, sizeof(mbi));
        boost::detail::winapi::HMODULE_ mod = mbi.AllocationBase;
    }
#else
    boost::filesystem::path symbol_location(const void* symbol) {
        Dl_info info;
        const int res = dladdr(symbol, &info);
        return res ? boost::filesystem::path(info.dli_fname) : boost::filesystem::path();
    }
#endif

    template <class T>
    boost::filesystem::path symbol_location(const T& symbol) {
        return boost::dll::symbol_location(
            boost::dll::detail::aggressive_ptr_cast<const void*>(boost::addressof(symbol))
        );
    }

}} // boost::dll

#endif // BOOST_DLL_RUNTIME_SYMBOL_INFO_HPP

