// system_error.hpp ----------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See http://www.boost.org/libs/application for documentation.

// -----------------------------------------------------------------------------

// Revision History
// 25-08-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_PLUGIN_DETAIL_AGGRESSIVE_PTR_CAST_HPP
#define BOOST_PLUGIN_DETAIL_AGGRESSIVE_PTR_CAST_HPP

#include <boost/config.hpp>
#ifdef BOOST_HAS_PRAGMA_ONCE
#   pragma once
#endif

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/is_const.hpp>
#include <stdint.h> // cstdint is not good because it does not define std::uintptr_t

namespace boost { namespace dll { namespace detail {

// GCC warns when reinterpret_cast between function pointer and object pointer occur.
// This method supress the warnings and ensures that such casts are safe.
template <class To, class From>
BOOST_FORCEINLINE To aggressive_ptr_cast(From* v) BOOST_NOEXCEPT {
    BOOST_STATIC_ASSERT_MSG(
        boost::is_pointer<To>::value,
        "`agressive_ptr_cast` function must be used only for pointer casting."
    );

    BOOST_STATIC_ASSERT_MSG(
        sizeof(v) == sizeof(To),
        "Pointer to function and pointer to object differ in size on your platform."
    );

    return reinterpret_cast<To>(
        reinterpret_cast<uintptr_t>(v)
    );
}

}}} // boost::dll::detail

#endif // BOOST_PLUGIN_DETAIL_AGGRESSIVE_PTR_CAST_HPP

