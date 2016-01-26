/*
 * mem_fn_cast.hpp
 *
 *  Created on: 17.10.2015
 *      Author: Klemens
 */

#ifndef INCLUDE_BOOST_DLL_DETAIL_MEM_FN_CAST_HPP_
#define INCLUDE_BOOST_DLL_DETAIL_MEM_FN_CAST_HPP_

#include <boost/config.hpp>
#include <boost/dll/detail/aggressive_ptr_cast.hpp>
#include <cstring>

namespace boost
{
namespace dll
{

namespace detail
{
#if defined(BOOST_MSVC) || defined(BOOST_MSVC_VER)
template<typename T> T mem_fn_cast(void* p)
{
    T out;
    std::memcpy(&out, &p, sizeof(void*));
    return out; //aggressive_ptr_cast<T>(p);
}

#else
template<typename T> T mem_fn_cast(void* p)
{
    T out;
    std::memset(&out, 0, sizeof(T));
    std::memcpy(&out, &p, sizeof(void*));

    return out;
}

#endif
}}}


#endif /* INCLUDE_BOOST_DLL_DETAIL_MEM_FN_CAST_HPP_ */
