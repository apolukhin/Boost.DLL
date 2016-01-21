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

namespace boost
{
namespace dll
{

namespace detail
{
#if defined(BOOST_MSVC) || defined(BOOST_MSVC_VER)
template<typename T> T mem_fn_cast(void* p)
{
	union
	{
		T out;
		void* in;
	} u;
	u.in = p;
	return u.out; //aggressive_ptr_cast<T>(p);
}

#else
template<typename T> T mem_fn_cast(void* p)
{
	union
	{
		T out;
		struct
		{
			void * in;
			void * dummy;
		} in;
	} conv;
	conv.in.in = p;
	conv.in.dummy = nullptr;
	return conv.out;
}

#endif
}}}


#endif /* INCLUDE_BOOST_DLL_DETAIL_MEM_FN_CAST_HPP_ */
