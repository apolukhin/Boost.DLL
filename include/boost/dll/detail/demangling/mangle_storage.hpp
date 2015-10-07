/*
 * cpp_loader.hpp
 *
 *  Created on: 07.10.2015
 *      Author: Klemens
 */

#ifndef BOOST_DLL_DETAIL_MANGLE_STORAGE_HPP_
#define BOOST_DLL_DETAIL_MANGLE_STORAGE_HPP_

#include <vector>
#include <string>
#include <boost/dll/detail/demangling/demangle_symbol.hpp>


namespace boost { namespace dll { namespace detail {

///stores the mangled names with the demangled name.
class mangled_storage
{
	struct _entry
	{
		std::string mangled;
		std::string demangled;
	};
	std::vector<_entry> _storage;
	///if a unknown class is imported it can be overloaded by this ype
	struct _alias
	{
		std::string in_dll;
		std::string alias;
	};
public:
	mangled_storage() = default;


};


}}}

#endif /* INCLUDE_BOOST_DLL_DETAIL_MANGLE_STORAGE_HPP_ */
