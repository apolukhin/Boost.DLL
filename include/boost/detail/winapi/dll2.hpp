//  dll.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2014 Renato Tegon Forti, Antony Polukhin
//  Copyright 2015 Andrey Semashev, Antony Polukhin

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef BOOST_DETAIL_WINAPI_DLL_HPP
#define BOOST_DETAIL_WINAPI_DLL_HPP

#include <boost/detail/winapi/basic_types.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( BOOST_USE_WINDOWS_H )
extern "C" {

namespace boost { namespace detail { namespace winapi {
struct MEMORY_BASIC_INFORMATION64_ {
    ULONGLONG_  BaseAddress;
    ULONGLONG_  AllocationBase;
    DWORD_      AllocationProtect;
    DWORD_      __alignment1;
    ULONGLONG_  RegionSize;
    DWORD_      State;
    DWORD_      Protect;
    DWORD_      Type;
    DWORD_      __alignment2;
};

struct MEMORY_BASIC_INFORMATION32_ {
    DWORD_ BaseAddress;
    DWORD_ AllocationBase;
    DWORD_ AllocationProtect;
    DWORD_ RegionSize;
    DWORD_ State;
    DWORD_ Protect;
    DWORD_ Type;
};

#ifdef _WIN64
typedef INT_PTR_ (WINAPI *FARPROC_)();
typedef INT_PTR_ (WINAPI *NEARPROC_)();
typedef INT_PTR_ (WINAPI *PROC_)();
typedef MEMORY_BASIC_INFORMATION64_ MEMORY_BASIC_INFORMATION_;
#else
typedef int (WINAPI *FARPROC_)();
typedef int (WINAPI *NEARPROC_)();
typedef int (WINAPI *PROC_)();
typedef MEMORY_BASIC_INFORMATION32_ MEMORY_BASIC_INFORMATION_;
#endif // _WIN64

const DWORD_ DONT_RESOLVE_DLL_REFERENCES_           = 0x00000001;
const DWORD_ LOAD_IGNORE_CODE_AUTHZ_LEVEL_          = 0x00000010;
const DWORD_ LOAD_LIBRARY_AS_DATAFILE_              = 0x00000002;
const DWORD_ LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE_    = 0x00000040;
const DWORD_ LOAD_LIBRARY_AS_IMAGE_RESOURCE_        = 0x00000020;
const DWORD_ LOAD_WITH_ALTERED_SEARCH_PATH_         = 0x00000008;
}}} // namespace boost::detail::winapi

#if !defined( BOOST_NO_ANSI_APIS )
BOOST_SYMBOL_IMPORT boost::detail::winapi::HMODULE_ WINAPI
LoadLibraryA(boost::detail::winapi::LPCSTR_ lpFileName);

BOOST_SYMBOL_IMPORT boost::detail::winapi::HMODULE_ WINAPI
LoadLibraryExA(
    boost::detail::winapi::LPCSTR_ lpFileName,
    boost::detail::winapi::HANDLE_ hFile,
    boost::detail::winapi::DWORD_ dwFlags
);

BOOST_SYMBOL_IMPORT boost::detail::winapi::HMODULE_ WINAPI
GetModuleHandleA(boost::detail::winapi::LPCSTR_ lpFileName);

BOOST_SYMBOL_IMPORT boost::detail::winapi::DWORD_ WINAPI
GetModuleFileNameA(
    boost::detail::winapi::HMODULE_ hModule,
    boost::detail::winapi::LPSTR_ lpFilename,
    boost::detail::winapi::DWORD_ nSize
);
#endif

BOOST_SYMBOL_IMPORT boost::detail::winapi::HMODULE_ WINAPI
LoadLibraryW(boost::detail::winapi::LPCWSTR_ lpFileName);

BOOST_SYMBOL_IMPORT boost::detail::winapi::HMODULE_ WINAPI
LoadLibraryExW(
    boost::detail::winapi::LPCWSTR_ lpFileName,
    boost::detail::winapi::HANDLE_ hFile,
    boost::detail::winapi::DWORD_ dwFlags
);

BOOST_SYMBOL_IMPORT boost::detail::winapi::HMODULE_ WINAPI
GetModuleHandleW(boost::detail::winapi::LPCWSTR_ lpFileName);

BOOST_SYMBOL_IMPORT boost::detail::winapi::DWORD_ WINAPI
GetModuleFileNameW(
    boost::detail::winapi::HMODULE_ hModule,
    boost::detail::winapi::LPWSTR_ lpFilename,
    boost::detail::winapi::DWORD_ nSize
);


BOOST_SYMBOL_IMPORT boost::detail::winapi::BOOL_ WINAPI
FreeLibrary(boost::detail::winapi::HMODULE_ hModule);

BOOST_SYMBOL_IMPORT boost::detail::winapi::FARPROC_ WINAPI
GetProcAddress(boost::detail::winapi::HMODULE_ hModule, boost::detail::winapi::LPCSTR_ lpProcName);

BOOST_SYMBOL_IMPORT boost::detail::winapi::FARPROC_ WINAPI
VirtualQuery(
    boost::detail::winapi::LPCVOID_ lpAddress,
    boost::detail::winapi::MEMORY_BASIC_INFORMATION_* lpBuffer,
    boost::detail::winapi::ULONG_PTR_ dwLength
);

} // extern "C"
#endif // #if !defined( BOOST_USE_WINDOWS_H )

namespace boost { namespace detail { namespace winapi {

#if defined( BOOST_USE_WINDOWS_H )
typedef ::FARPROC FARPROC_;
typedef ::NEARPROC NEARPROC_;
typedef ::PROC PROC_;
typedef ::MEMORY_BASIC_INFORMATION MEMORY_BASIC_INFORMATION_;

const DWORD_ DONT_RESOLVE_DLL_REFERENCES_           = DONT_RESOLVE_DLL_REFERENCES;
const DWORD_ LOAD_IGNORE_CODE_AUTHZ_LEVEL_          = LOAD_IGNORE_CODE_AUTHZ_LEVEL;
const DWORD_ LOAD_LIBRARY_AS_DATAFILE_              = LOAD_LIBRARY_AS_DATAFILE;
const DWORD_ LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE_    = LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE;
const DWORD_ LOAD_LIBRARY_AS_IMAGE_RESOURCE_        = LOAD_LIBRARY_AS_IMAGE_RESOURCE;
const DWORD_ LOAD_WITH_ALTERED_SEARCH_PATH_         = LOAD_WITH_ALTERED_SEARCH_PATH;
#endif // defined( BOOST_USE_WINDOWS_H )

// We always define the following constants by explicit values, because
// they may not be defined in windows.h
//
// Minimum supported client :  Windows 8
// Minimum supported server : Windows Server 2012
//
// To Windows 7, Windows Server 2008 R2, Windows Vista,
// and Windows Server 2008: KB2533623 must be installed on the target platform.
const DWORD_ LOAD_LIBRARY_SEARCH_PLUGIN_DIR_    = 0x00000200;
const DWORD_ LOAD_LIBRARY_SEARCH_DEFAULT_DIRS_  = 0x00001000;
const DWORD_ LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR_  = 0x00000100;
const DWORD_ LOAD_LIBRARY_SEARCH_SYSTEM32_      = 0x00000800;
const DWORD_ LOAD_LIBRARY_SEARCH_USER_DIRS_     = 0x00000400;

#if !defined( BOOST_NO_ANSI_APIS )
using ::LoadLibraryA;
using ::LoadLibraryExA;
using ::GetModuleHandleA;
using ::GetModuleFileNameA;
#endif // !defined( BOOST_NO_ANSI_APIS )
using ::LoadLibraryW;
using ::LoadLibraryExW;
using ::GetModuleHandleW;
using ::GetModuleFileNameW;
using ::FreeLibrary;
using ::GetProcAddress;
using ::VirtualQuery;

#if !defined( BOOST_NO_ANSI_APIS )
BOOST_FORCEINLINE HMODULE_ load_library(LPCSTR_ lpFileName) {
    return ::LoadLibraryA(lpFileName);
}

BOOST_FORCEINLINE HMODULE_ load_library_ex(LPCSTR_ lpFileName, HANDLE_ hFile, DWORD_ dwFlags) {
    return ::LoadLibraryExA(lpFileName, hFile, dwFlags);
}

BOOST_FORCEINLINE HMODULE_ get_module_handle(LPCSTR_ lpFileName) {
    return ::GetModuleHandleA(lpFileName);
}

BOOST_FORCEINLINE DWORD_ get_module_file_name(HMODULE_ hModule, LPSTR_ lpFilename, DWORD_ nSize) {
    return ::GetModuleFileNameA(hModule, lpFilename, nSize);
}
#endif // #if !defined( BOOST_NO_ANSI_APIS )

BOOST_FORCEINLINE HMODULE_ load_library(LPCWSTR_ lpFileName) {
    return ::LoadLibraryW(lpFileName);
}

BOOST_FORCEINLINE HMODULE_ load_library_ex(LPCWSTR_ lpFileName, HANDLE_ hFile, DWORD_ dwFlags) {
    return ::LoadLibraryExW(lpFileName, hFile, dwFlags);
}

BOOST_FORCEINLINE HMODULE_ get_module_handle(LPCWSTR_ lpFileName) {
    return ::GetModuleHandleW(lpFileName);
}

BOOST_FORCEINLINE DWORD_ get_module_file_name(HMODULE_ hModule, LPWSTR_ lpFilename, DWORD_ nSize) {
    return ::GetModuleFileNameW(hModule, lpFilename, nSize);
}
}}} // namespace boost::detail::winapi

#endif // BOOST_DETAIL_WINAPI_DLL_HPP
