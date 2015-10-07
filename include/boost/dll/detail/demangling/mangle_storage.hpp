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
#include <boost/type_index/stl_type_index.hpp>

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
	std::vector<_alias> _aliases;
public:
	mangled_storage() = default;

	/*! Allows do add a class as alias, if the class imported is not known
	 * in this binary.
	 * @tparam Alias The Alias type
	 *  @param The name to create the alias for.
	 *
	 *  @note There can be multiple aliases, this is on purpose.
	 */
	template<typename Alias> void add_alias(const std::string& name)
	{
		_aliases.emplace_back(
			name,
			boost::typeindex::stl_type_index::type_id<Alias>().pretty_name()
			);
	}
	void add_symbols(const std::vector<std::string> & symbols)
	{
		for (auto & sym : symbols)
		{
			auto dm = demangle_symbol(sym);
			if (!dm.empty())
				_storage.emplace_back(
					sym,
					dm);
		}
	}

};


}}}

#endif /* INCLUDE_BOOST_DLL_DETAIL_MANGLE_STORAGE_HPP_ */
