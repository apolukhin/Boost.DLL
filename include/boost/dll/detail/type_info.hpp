// Copyright 2016 Klemens Morgenstern, Antony Polukhin
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_DLL_DETAIL_TYPE_INFO_HPP_
#define BOOST_DLL_DETAIL_TYPE_INFO_HPP_

#include <typeinfo>
#include <cstring>

namespace boost { namespace dll { namespace detail {

#if defined(BOOST_MSVC) || defined(BOOST_MSVC_VER)
template<typename Class, typename Lib, typename Storage>
const std::type_info& load_type_info(Lib & lib, Storage & storage)
{
    struct RTTICompleteObjectLocator
    {
        boost::detail::winapi::DWORD_ signature; //always zero ?
        boost::detail::winapi::DWORD_ offset;    //offset of this vtable in the complete class
        boost::detail::winapi::DWORD_ cdOffset;  //constructor displacement offset
        boost::detail::winapi::DWORD_ pTypeDescriptor; //TypeDescriptor of the complete class
        boost::detail::winapi::DWORD_ pClassDescriptor; //describes inheritance hierarchy (ignored)
    };

    RTTICompleteObjectLocator** vtable_p = &lib.template get<RTTICompleteObjectLocator*>(storage.template get_vtable<Class>());

    vtable_p--;
    auto vtable = *vtable_p;
    const std::type_info * ti_p;
    std::memcpy(&ti_p, &vtable->pTypeDescriptor, sizeof(boost::detail::winapi::DWORD_));
    return *ti_p;
}

#else

template<typename Class, typename Lib, typename Storage>
const std::type_info& load_type_info(Lib & lib, Storage & storage)
{
    return lib.template get<const std::type_info>(storage.template get_type_info<Class>());

}

#endif


}}}
#endif /* BOOST_DLL_DETAIL_TYPE_INFO_HPP_ */
