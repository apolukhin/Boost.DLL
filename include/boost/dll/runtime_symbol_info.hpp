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
#   include <boost/dll/detail/windows/path_from_handle.hpp>
#else
#   include <dlfcn.h> // Not suitable for Windows
#   include <boost/dll/detail/posix/path_from_handle.hpp>
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

        boost::system::error_code ignore;
        return boost::dll::detail::path_from_handle(reinterpret_cast<boost::detail::winapi::HMODULE_>(mbi.AllocationBase), ignore);
    }

    inline boost::filesystem::path program_location_impl() {
        boost::system::error_code ignore;
        return boost::dll::detail::path_from_handle(NULL, ignore);
    }    
#else
    inline boost::filesystem::path symbol_location_impl(const void* symbol) {
        Dl_info info;
        const int res = dladdr(symbol, &info);
        return res ? boost::filesystem::path(info.dli_fname) : boost::filesystem::path();
    }

    inline boost::filesystem::path program_location_impl() {
        // As is known the function dlopen() loads the dynamic library file 
        // named by the null-terminated string filename and returns an opaque 
        // "handle" for the dynamic library. If filename is NULL, then the 
        // returned handle is for the main program.
        void* handle = dlopen(NULL, RTLD_LAZY | RTLD_LOCAL);
        if (!handle) {
            return boost::filesystem::path();
        }
        
        boost::system::error_code ignore;
        return boost::dll::detail::path_from_handle(handle, ignore);
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

    /// @cond
    // We have anonymous namespace here to make sure that `this_line_location()` method is instantiated in
    // current translation module and is not shadowed by instantiations from other modules.
    namespace {
    /// @endcond

    /*!
    * On success returns full path and name of the binary object that holds the current line of code
    * (the line in wich the `this_line_location()` method was called).
    *
    * \throws std::bad_alloc in case of insufficient memory.
    */
    static inline boost::filesystem::path this_line_location() {
        return boost::dll::symbol_location(this_line_location);
    }

    /// @cond
    } // anonymous namespace
    /// @endcond

    /*!
    * On success returns full path and name of the currently running program (the one which contains the `main()` function).
    *
    * \throws std::bad_alloc in case of insufficient memory.
    */
    static inline boost::filesystem::path program_location() {
        return boost::dll::detail::program_location_impl();
    }

}} // namespace boost::dll

#endif // BOOST_DLL_RUNTIME_SYMBOL_INFO_HPP

