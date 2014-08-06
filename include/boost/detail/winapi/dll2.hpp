//  dll.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2014 Renato Tegon Forti, Antony Polukhin.

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef BOOST_DETAIL_WINAPI_DLL_HPP
#define BOOST_DETAIL_WINAPI_DLL_HPP

#include <boost/detail/winapi/basic_types.hpp>
#include <boost/detail/winapi/security.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost
{
namespace detail
{
namespace winapi
{
#if defined( BOOST_USE_WINDOWS_H )
    typedef ::FARPROC FARPROC_;
    typedef ::NEARPROC NEARPROC_;
    typedef ::PROC PROC_;

    using ::LoadLibraryW;
    using ::LoadLibraryExW;
    using ::GetModuleHandleW;

# ifndef BOOST_NO_ANSI_APIS
    using ::LoadLibraryA;
    using ::LoadLibraryExA;
    using ::GetModuleHandleA;
# endif
    using ::FreeLibrary;
    using ::GetProcAddress;

    const DWORD_ DONT_RESOLVE_DLL_REFERENCES_       = DONT_RESOLVE_DLL_REFERENCES;
    const DWORD_ LOAD_IGNORE_CODE_AUTHZ_LEVEL_      = LOAD_IGNORE_CODE_AUTHZ_LEVEL;
    const DWORD_ LOAD_LIBRARY_AS_DATAFILE_          = LOAD_LIBRARY_AS_DATAFILE;
    const DWORD_ LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE_ = LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE;
    const DWORD_ LOAD_LIBRARY_AS_IMAGE_RESOURCE_    = LOAD_LIBRARY_AS_IMAGE_RESOURCE;

    const DWORD_ LOAD_LIBRARY_SEARCH_PLUGIN_DIR_    = LOAD_LIBRARY_SEARCH_PLUGIN_DIR;
    const DWORD_ LOAD_LIBRARY_SEARCH_DEFAULT_DIRS_  = LOAD_LIBRARY_SEARCH_DEFAULT_DIRS;
    const DWORD_ LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR_  = LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR;
    const DWORD_ LOAD_LIBRARY_SEARCH_SYSTEM32_      = LOAD_LIBRARY_SEARCH_SYSTEM32;
    const DWORD_ LOAD_LIBRARY_SEARCH_USER_DIRS_     = LOAD_LIBRARY_SEARCH_USER_DIRS;
    const DWORD_ LOAD_WITH_ALTERED_SEARCH_PATH_     = LOAD_LIBRARY_SEARCH_USER_DIRS;

#else
extern "C" {
# ifdef _WIN64
    typedef INT_PTR_ (WINAPI *FARPROC_)();
    typedef INT_PTR_ (WINAPI *NEARPROC_)();
    typedef INT_PTR_ (WINAPI *PROC_)();
# else
    typedef int (WINAPI *FARPROC_)();
    typedef int (WINAPI *NEARPROC_)();
    typedef int (WINAPI *PROC_)();
# endif // _WIN64

    __declspec(dllimport) HMODULE_ WINAPI
        LoadLibraryW(
            LPCWSTR_ lpFileName
    );
    __declspec(dllimport) HMODULE_ WINAPI
        LoadLibraryExW(
            LPCWSTR_ lpFileName,
            HANDLE_ hFile,
            DWORD_ dwFlags
    );
    __declspec(dllimport) HMODULE_ WINAPI
        GetModuleHandleW(
            LPCWSTR_ lpFileName
    );

# ifndef BOOST_NO_ANSI_APIS
    __declspec(dllimport) HMODULE_ WINAPI
        LoadLibraryA(
            LPCSTR_ lpFileName
    );
    __declspec(dllimport) HMODULE_ WINAPI
        LoadLibraryExA(
            LPCSTR_ lpFileName,
            HANDLE_ hFile,
            DWORD_ dwFlags
    );
    __declspec(dllimport) HMODULE_ WINAPI
        GetModuleHandleA(
            LPCSTR_ lpFileName
    );
# endif

    __declspec(dllimport) BOOL_ WINAPI
        FreeLibrary(
            HMODULE_ hModule
    );
    __declspec(dllimport) FARPROC_ WINAPI
        GetProcAddress(
            HMODULE_ hModule,
            LPCSTR_ lpProcName
    );

    const DWORD_ DONT_RESOLVE_DLL_REFERENCES_       = 0x00000001;
    const DWORD_ LOAD_IGNORE_CODE_AUTHZ_LEVEL_      = 0x00000010;
    const DWORD_ LOAD_LIBRARY_AS_DATAFILE_          = 0x00000002;
    const DWORD_ LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE_ = 0x00000040;
    const DWORD_ LOAD_LIBRARY_AS_IMAGE_RESOURCE_    = 0x00000020;

    const DWORD_ LOAD_LIBRARY_SEARCH_PLUGIN_DIR_    = 0x00000200;
    const DWORD_ LOAD_LIBRARY_SEARCH_DEFAULT_DIRS_  = 0x00001000;
    const DWORD_ LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR_  = 0x00000100;
    const DWORD_ LOAD_LIBRARY_SEARCH_SYSTEM32_      = 0x00000800;
    const DWORD_ LOAD_LIBRARY_SEARCH_USER_DIRS_     = 0x00000400;
    const DWORD_ LOAD_WITH_ALTERED_SEARCH_PATH_     = 0x00000008;

}
#endif
}
}
}

#endif // BOOST_DETAIL_WINAPI_DLL_HPP
