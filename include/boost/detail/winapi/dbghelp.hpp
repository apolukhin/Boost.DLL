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

typedef const WCHAR_ *PCWSTR_;
typedef WCHAR_          *PWSTR_;
typedef const CHAR_  *PCSTR_;
typedef CHAR_          *PSTR_;


__declspec(dllimport) DWORD_ WINAPI UnDecorateSymbolName
        (PCSTR_ DecoratedName,
         PSTR_ UnDecoratedName,
         DWORD_ UndecoratedLength,
         DWORD_ Flags);

__declspec(dllimport) DWORD_ WINAPI UnDecorateSymbolNameW
        (PCWSTR_ DecoratedName,
         PWSTR_ UnDecoratedName,
         DWORD_ UndecoratedLength,
         DWORD_ Flags);

}
}}}




#endif /* INCLUDE_BOOST_DETAIL_WINAPI_DBGHELP_HPP_ */
