/*
 * mangled_storage.hpp
 *
 *  Created on: 08.10.2015
 *      Author: Klemens
 */

#ifndef BOOST_DLL_MANGLED_STORAGE_HPP_
#define BOOST_DLL_MANGLED_STORAGE_HPP_

#include <boost/config.hpp>

#if defined(BOOST_MSVC) || defined(BOOST_MSVC_VER)
#include <boost/dll/detail/demangling/msvc.hpp>
#else
#include <boost/dll/detail/demangling/itanium.hpp>
#endif

namespace boost { namespace dll {

class mangled_storage : public detail::mangled_storage_impl
{
public:
	using detail::mangled_storage_impl::mangled_storage_impl;

};


}}



#endif /* INCLUDE_BOOST_DLL_MANGLED_STORAGE_HPP_ */
