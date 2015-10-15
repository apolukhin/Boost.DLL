// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Copyright 2015 Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_DLL_RUNTIME_SYMBOL_INFO_HPP
#define BOOST_DLL_RUNTIME_SYMBOL_INFO_HPP

#include <boost/config.hpp>
#include <boost/predef/os.h>
#include <boost/dll/detail/aggressive_ptr_cast.hpp>
#if BOOST_OS_WINDOWS
#   include <boost/detail/winapi/dll2.hpp> // TODO: FIXME
#   include <boost/dll/detail/windows/path_from_handle.hpp>
#else
#   include <dlfcn.h>
#   include <boost/dll/detail/posix/program_location_impl.hpp>
#endif

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

/// \file boost/dll/runtime_symbol_info.hpp
/// \brief Provides methods for getting acceptable by boost::dll::shared_library location of symbol, source line or program.
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

    inline boost::filesystem::path program_location_impl(boost::system::error_code& ec) {
        return boost::dll::detail::path_from_handle(NULL, ec);
    }    
#else
    inline boost::filesystem::path symbol_location_impl(const void* symbol) {
        Dl_info info;

        // Some of the libc headers miss `const` in `dladdr(const void*, Dl_info*)`
        const int res = dladdr(const_cast<void*>(symbol), &info);
        return res ? boost::filesystem::path(info.dli_fname) : boost::filesystem::path();
    }
#endif
} // namespace detail

    /*!
    * On success returns full path and name of the binary object that holds symbol.
    * \tparam T Type of the symbol, must not be explicitly specified.
    * \param symbol Symbol which location is to be determined.
    * \return Path to the binary object that holds symbol or empty path in case error.
    * \throws std::bad_alloc in case of insufficient memory.
    *
    * \b Examples:
    * \code
    * int var;
    * void foo() {}
    *
    * int main() {
    *    dll::symbol_location(var);                     // returns program location
    *    dll::symbol_location(foo);                     // returns program location
    *    dll::symbol_location(std::cerr);               // returns location of libstdc++: "/usr/lib/x86_64-linux-gnu/libstdc++.so.6"
    *    dll::symbol_location(std::placeholders::_1);   // returns location of libstdc++: "/usr/lib/x86_64-linux-gnu/libstdc++.so.6"
    *    dll::symbol_location(std::puts);               // returns location of libc: "/lib/x86_64-linux-gnu/libc.so.6"
    * }
    * \endcode
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
    * (the line in which the `this_line_location()` method was called).
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
    * Return value can be used as a parameter for shared_library. See Tutorial "Linking plugin into the executable"
    * for usage example. Flag '-rdynamic' must be used when linking the plugin into the executable
    * on Linux OS.
    *
    * \throws std::bad_alloc in case of insufficient memory.
    */
    static inline boost::filesystem::path program_location() {
        boost::system::error_code ignore;
        return boost::dll::detail::program_location_impl(ignore);
    }

}} // namespace boost::dll

#endif // BOOST_DLL_RUNTIME_SYMBOL_INFO_HPP

