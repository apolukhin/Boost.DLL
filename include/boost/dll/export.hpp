/*
 * export.hpp
 *
 *  Created on: 05.10.2015
 *      Author: Klemens
 */

#ifndef INCLUDE_BOOST_DLL_EXPORT_HPP_
#define INCLUDE_BOOST_DLL_EXPORT_HPP_

#include <boost/predef/compiler.h>


#if BOOST_COMP_GNUC || BOOST_COMP_CLANG || BOOST_COMP_HPACC || BOOST_COMP_IBM
#define BOOST_DLL_MEMBER_EXPORT BOOST_SYMBOL_EXPORT
#elif BOOST_COMP_MSVC
#define BOOST_DLL_MEMBER_EXPORT
#else
#error "Compiler not supported"
#endif


#endif /* INCLUDE_BOOST_DLL_EXPORT_HPP_ */
