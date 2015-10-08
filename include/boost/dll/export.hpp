/*
 * export.hpp
 *
 *  Created on: 05.10.2015
 *      Author: Klemens
 */

#ifndef INCLUDE_BOOST_DLL_EXPORT_HPP_
#define INCLUDE_BOOST_DLL_EXPORT_HPP_

#include <boost/config.hpp>


#if defined(BOOST_GCC)
#define BOOST_MEMBER_EXPORT BOOST_SYMBOL_EXPORT
#else
#define BOOST_MEMBER_EXPORT
#endif


#endif /* INCLUDE_BOOST_DLL_EXPORT_HPP_ */
