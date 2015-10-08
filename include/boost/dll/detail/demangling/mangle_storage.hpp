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
#include <map>
#include <boost/dll/detail/demangling/demangle_symbol.hpp>
#include <boost/type_index/stl_type_index.hpp>

namespace boost { namespace dll { namespace detail {

///stores the mangled names with the demangled name.
struct mangled_storage
{
	struct entry
	{
		std::string mangled;
		std::string demangled;
	};
private:
	std::vector<entry> storage;
	///if a unknown class is imported it can be overloaded by this type
	std::map<boost::typeindex::stl_type_index, std::string> aliases;
public:
	template<typename T>
	std::string get_name() const
	{
		auto tx = boost::typeindex::stl_type_index::type_id<T>();
		return (aliases.count(tx) > 0) ? aliases[tx] : tx.pretty_name();
	}

	mangled_storage() = default;
	mangled_storage(mangled_storage&&) = default;
	mangled_storage(const mangled_storage&) = default;

	/*! Allows do add a class as alias, if the class imported is not known
	 * in this binary.
	 * @tparam Alias The Alias type
	 *  @param The name to create the alias for.
	 *
	 *  @note There can be multiple aliases, this is on purpose.
	 */
	template<typename Alias> void add_alias(const std::string& name)
	{
		aliases.emplace(
			boost::typeindex::stl_type_index::type_id<Alias>(),
			name
			);
	}
	void add_symbols(const std::vector<std::string> & symbols)
	{
		for (auto & sym : symbols)
		{
			auto dm = demangle_symbol(sym);
			if (!dm.empty())
				storage.emplace_back(
					sym,
					dm);
		}
	}


};


}}}

#endif /* INCLUDE_BOOST_DLL_DETAIL_MANGLE_STORAGE_HPP_ */
