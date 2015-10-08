/*
 * msvc.hpp
 *
 *  Created on: 08.10.2015
 *      Author: Klemens
 */

#ifndef BOOST_DLL_DETAIL_DEMANGLING_MSVC_HPP_
#define BOOST_DLL_DETAIL_DEMANGLING_MSVC_HPP_

#include <boost/dll/detail/demangling/mangled_storage_base_base.hpp>

namespace boost { namespace dll { namespace detail {

class signature_matcher  : public mangled_storage_base
{
public:
	struct ctor_t
	{
		std::string C;
	};

	struct dtor_t
	{
		std::string D;
	};


	template<typename T>
	std::string get_variable(const std::string &name);

	template<typename Func>
	std::string get_function(const std::string &name);

	template<typename Class, typename Func>
	std::string get_mem_fn(const std::string &name);

	template<typename Signature>
	ctor_t get_constructor();

	template<typename Class>
	dtor_t get_destructor();
};


}}}



#endif /* INCLUDE_BOOST_DLL_DETAIL_DEMANGLING_MSVC_HPP_ */
