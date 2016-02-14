//  dbghelp.hpp  --------------------------------------------------------------//

//  Copyright 2015 Klemens Morgenstern

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_DLL_DETAIL_WINDOWS_DBGHELP_HPP_
#define BOOST_DLL_DETAIL_WINDOWS_DBGHELP_HPP_

#include <boost/detail/winapi/basic_types.hpp>

struct API_VERSION;

namespace boost
{
namespace dll
{
namespace detail
{
namespace winapi
{
extern "C"
{

BOOST_SYMBOL_IMPORT boost::detail::winapi::DWORD_ WINAPI UnDecorateSymbolName
        (const boost::detail::winapi::CHAR_ * DecoratedName,
         boost::detail::winapi::CHAR_ *UnDecoratedName,
         boost::detail::winapi::DWORD_ UndecoratedLength,
         boost::detail::winapi::DWORD_ Flags);

BOOST_SYMBOL_IMPORT boost::detail::winapi::DWORD_ WINAPI UnDecorateSymbolNameW
        (const boost::detail::winapi::WCHAR_ * DecoratedName,
         boost::detail::winapi::WCHAR_* UnDecoratedName,
         boost::detail::winapi::DWORD_ UndecoratedLength,
         boost::detail::winapi::DWORD_ Flags);

}

inline boost::detail::winapi::DWORD_ undecorate_symbol_name(const char* decorated_name,
                              char*  undecorated_name,
                              boost::detail::winapi::DWORD_ undecorated_length,
                              boost::detail::winapi::DWORD_ flags)
{
    return UnDecorateSymbolName(decorated_name, undecorated_name, undecorated_length, flags);
}

inline boost::detail::winapi::DWORD_ undecorate_symbol_name(const wchar_t* decorated_name,
                              wchar_t*  undecorated_name,
                              boost::detail::winapi::DWORD_ undecorated_length,
                              boost::detail::winapi::DWORD_ flags)
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

BOOST_SYMBOL_IMPORT ::API_VERSION* WINAPI ImagehlpApiVersion();



}
inline api_version& image_api_version()
{
    return *reinterpret_cast<api_version*>(ImagehlpApiVersion());
}

}}}}




#endif /* BOOST_DLL_DETAIL_WINDOWS_DBGHELP_HPP_ */
