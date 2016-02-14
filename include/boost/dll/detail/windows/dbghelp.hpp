//  dbghelp.hpp  --------------------------------------------------------------//

//  Copyright 2015 Klemens Morgenstern

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_DETAIL_WINAPI_DBGHELP_HPP_
#define BOOST_DETAIL_WINAPI_DBGHELP_HPP_


#include <boost/detail/winapi/basic_types.hpp>
namespace boost
{
namespace detail
{
namespace winapi
{
extern "C"
{

BOOST_SYMBOL_IMPORT DWORD_ WINAPI UnDecorateSymbolName
        (const CHAR_ * DecoratedName,
         CHAR_ *UnDecoratedName,
         DWORD_ UndecoratedLength,
         DWORD_ Flags);

BOOST_SYMBOL_IMPORT DWORD_ WINAPI UnDecorateSymbolNameW
        (const WCHAR_ * DecoratedName,
         WCHAR_* UnDecoratedName,
         DWORD_ UndecoratedLength,
         DWORD_ Flags);

}

inline DWORD_ undecorate_symbol_name(const char* decorated_name,
                              char*  undecorated_name,
                              DWORD_ undecorated_length,
                              DWORD_ flags)
{
    return UnDecorateSymbolName(decorated_name, undecorated_name, undecorated_length, flags);
}

inline DWORD_ undecorate_symbol_name(const wchar_t* decorated_name,
                              wchar_t*  undecorated_name,
                              DWORD_ undecorated_length,
                              DWORD_ flags)
{
    return UnDecorateSymbolNameW(decorated_name, undecorated_name, undecorated_length, flags);
}


extern "C"
{
struct api_version {
  unsigned short major_version;
  unsigned short minor_version;
  unsigned short revision;
  unsigned short reserved;
};

struct _API_VERSION;

_API_VERSION* WINAPI ImagehlpApiVersion();



}
inline api_version& image_api_version()
{
    return *reinterpret_cast<api_version*>(ImagehlpApiVersion());
}

}}}




#endif /* INCLUDE_BOOST_DETAIL_WINAPI_DBGHELP_HPP_ */
