/*
 * read_outline.hpp
 *
 *  Created on: 05.10.2015
 *      Author: Klemens
 */

#ifndef INCLUDE_BOOST_DLL_DETAIL_READ_OUTLINE_HPP_
#define INCLUDE_BOOST_DLL_DETAIL_READ_OUTLINE_HPP_

#include <string>
#include <vector>
#include <boost/config.hpp>

#if defined(BOOST_WINDOWS)
#include <boost/dll/detail/windows/read_dll.hpp>
#elif
#include <boost/dll/detail/posix/read_elf.hpp>
#endif

namespace boost { namespace dll { namespace detail {

inline std::vector<std::string> read_outline(void* handle)
{
#if defined(BOOST_WINDOWS)
	return win_dll::read_dll(handle);
#else
	return elf::read_elf(handle);
#endif
}

}}}

#endif /* INCLUDE_BOOST_DLL_DETAIL_READ_OUTLINE_HPP_ */
