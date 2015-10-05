/*
 * descriptors.hpp
 *
 *  Created on: 05.10.2015
 *      Author: Klemens
 */

#ifndef BOOST_DLL_DETAIL_DESCRIPTORS_HPP_
#define BOOST_DLL_DETAIL_DESCRIPTORS_HPP_

#include <vector>
#include <string>

namespace boost { namespace dll { namespace detail {

using param_list = std::vector<std::string>;

struct constructor
{
	std::string entry_point; //C0 for itanium
	std::string entry_point1; //only used for itanium
	std::string entry_point2; //only used for itanium
	std::string params;
	std::string type;
};

struct desctructor
{
	std::string entry_point; //D0 for itanium
	std::string entry_point1; //only used for itanium
	std::string entry_point2; //only used for itanium
	std::string type;
};

struct function
{
	//always __cdecl
	std::string entry_point;
	param_list params;
	std::string name;
	std::string return_;
};

struct method
{	//always __thiscall.
	std::string entry_point;
	param_list params;
	std::string type;
	std::string name;
	bool const_;
	bool volatile_;
	std::string return_;
};

struct variable
{
	std::string entry_point;
	std::string type;
	std::string name;
};


struct class_
{
	std::vector<constructor> ctors;
	std::vector<method> methods;
	std::vector<function> functions;
	std::size_t size;
	std::type_info * type_info;
	descructor dtor;
};


}}}




#endif /* INCLUDE_BOOST_DLL_DETAIL_DESCRIPTORS_HPP_ */
