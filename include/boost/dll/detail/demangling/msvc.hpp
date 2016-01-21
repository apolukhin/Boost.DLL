/*
 * msvc.hpp
 *
 *  Created on: 08.10.2015
 *      Author: Klemens
 */

#ifndef BOOST_DLL_DETAIL_DEMANGLING_MSVC_HPP_
#define BOOST_DLL_DETAIL_DEMANGLING_MSVC_HPP_

#include <boost/dll/detail/demangling/mangled_storage_base.hpp>
#include <iterator>
#include <algorithm>
#include <boost/dll/detail/demangling/tokenizer.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/is_volatile.hpp>
#include <boost/type_traits/is_lvalue_reference.hpp>
#include <boost/type_traits/is_rvalue_reference.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include <boost/spirit/home/x3.hpp>

namespace boost { namespace dll { namespace detail {

class mangled_storage_impl  : public mangled_storage_base
{
	template<typename T>
	struct dummy {};

	template<typename Return, typename ...Args>
	std::vector<std::string> get_func_params(dummy<Return(Args...)>)
	{
		return {get_name<Args>()...};
	}
	template<typename Return, typename ...Args>
	std::string get_return_type(dummy<Return(Args...)>)
	{
		return get_name<Return>();
	}
public:
	using ctor_sym = std::string;
	using dtor_sym = std::string;

	using mangled_storage_base::mangled_storage_base;

	template<typename T>
	std::string get_variable(const std::string &name);

	template<typename Func>
	std::string get_function(const std::string &name);

	template<typename Class, typename Func>
	std::string get_mem_fn(const std::string &name);

	template<typename Signature>
	ctor_sym get_constructor();

	template<typename Class>
	dtor_sym get_destructor();
};


namespace parser
{
	namespace x3 = spirit::x3;

	auto visibility = ("public:" | x3::lit("protected:") | "private:") >> x3::space ;
	auto static_ 	= x3::lit("static") >> x3::space;

	auto const_rule_impl(true_type )  {return "const"  >> x3::space;};
	auto const_rule_impl(false_type)  {return x3::eps;};

	template<typename T>
	auto const_rule() {using t = is_const<typename remove_reference<T>::type>; return const_rule_impl(t());}


	auto volatile_rule_impl(true_type )  {return "volatile" >> x3::space;};
	auto volatile_rule_impl(false_type)  {return x3::eps;};

	template<typename T>
	auto volatile_rule() {using t = is_volatile<typename remove_reference<T>::type>; return volatile_rule_impl(t());}


	auto reference_rule_impl(false_type, false_type) {return x3::eps;}
	auto reference_rule_impl(true_type,  false_type) {return "&"  >> x3::space;}
	auto reference_rule_impl(false_type, true_type ) {return "&&" >> x3::space;}


	template<typename T>
	auto reference_rule() {using t_l = is_lvalue_reference<T>; using t_r = is_rvalue_reference<T>; return reference_rule_impl(t_l(), t_r());}

	auto class_ = ("class" | x3::lit("struct")) >> x3::space;

	//it takes a string, because it may be overloaded.
	template<typename T>
	auto type_rule(const std::string & type_name)
	{
		return -class_ >> type_name >> x3::space >>
				const_rule<T>() >>
				volatile_rule<T>() >>
				reference_rule<T>();
	}
}




template<typename Range, typename Iterator>
bool match_params(const Range &range, Iterator & itr, const Iterator & end)
{
	if (range.size() == 0) //msvc provides a void for an empty parameter list
	{
		if (itr == end)
			return false;

		if (*itr != "void")
			return false;
		itr++;
		return true;
	}
	else
	{
		for (auto &p : range)
		{
			if (itr == end)
			{
				return false;
			}
			/*if (! match_types(p, *itr))
				return false;
			*/itr++;
		}
	}
	return true;
}

template<typename T>
auto match_function(
		const std::vector<std::string> &params,
		const std::string& name,
		const std::string& return_type)
{
	return [params, name, return_type](const mangled_storage_impl::entry& e)
	{
		//ok, we need to tokenize it, but that should be rathers fast
		tokenizer<tokenize_function> tkz(e.demangled);
		auto itr = tkz.begin();

		if (itr == tkz.end()) //empty name
			return false;

		//first of, look if it's a static member-function

	//	match_static_member(itr, tkz.end());
		if (itr == tkz.end()) //errornous name
			return false;

		//we match the type at the end, because it is more likely that the type is right, than the name

		auto rt_begin = itr;
		while ((*itr != "__cdecl")
			&& (*itr != "__thiscall")
			&& (*itr != "__stdcall"))
 		{
 			itr++;
 			if (itr == tkz.end())
 				return false;
 		}
 		if (*itr != "__cdecl")
 			return false;
 		auto rt_end = itr;
 		itr++;

 		if (itr == tkz.end())
 			return false;
 		if (*itr != name) //alright name matched
 			return false;


 		itr++;
 		if (itr == tkz.end()) //exported in a C-manner, errornous for this approach.
 			return false;

 		if (*itr != "(") //start the param list
 			return false;
 		itr++;

 		if (!match_params(params, itr, tkz.end()))
 			return false;

 		//now check the end of the function name, i.e. ")" and the end after this.
 		if (itr == tkz.end())
 			return false;

 		if (*itr != ")")
 			return false;
 		itr++;
 		if (itr != tkz.end())
 			return false;

 		//alright everything fitted, now match the return type
 		//if (match_type<T>(return_type, rt_begin, rt_end))
			//return true;
		return false;
	};
}

template<typename T>
auto match_mem_fn(const std::string &class_name,
		const std::vector<std::string> &params,
		const std::string& name,
		const std::string& return_type,
		bool is_const = false,
		bool is_volatile = false)
{
	auto mem_fn_name = class_name + "::" + name;

	return [params, name, is_const, is_volatile, mem_fn_name, return_type]
			(const mangled_storage_impl::entry& e)
	{
		//ok, we need to tokenize it, but that should be rathers fast
		tokenizer<tokenize_function> tkz(e.demangled);
		auto itr = tkz.begin();

		if (  (*itr == "public:")
			||(*itr == "private:")
			||(*itr == "protected:"))
			{
				itr++;
				if (itr == tkz.end())
					return false;
			}

		if (*itr == "virtual")
		{
			itr++;
			if (itr == tkz.end())
				return false;
		}


		auto rt_begin = itr;
		while ((*itr != "__cdecl")
			&& (*itr != "__thiscall")
			&& (*itr != "__stdcall"))
 		{
 			itr++;
 			if (itr == tkz.end())
 				return false;
 		}
 		if (*itr != "__thiscall")
 			return false;
 		auto rt_end = itr;
 		itr++;
 		if (itr == tkz.end())
 			return false;

		if (*itr != mem_fn_name)
			return false;
		itr++;
		if (itr == tkz.end()) //no function.
			return false;


		if (*itr != "(")
			return false;
		itr++;

		if (!match_params(params, itr, tkz.end()))
			return false;

		if (itr == tkz.end())
			return false;
		if (*itr != ")")
			return false;
		itr++;


		if (itr != tkz.end())
			if ((*itr == "const") && is_const)
				itr++;

		if (itr != tkz.end())
			if ((*itr == "volatile") && is_volatile)
				itr++;

		if (itr != tkz.end())
			return false;
 		//alright everything fitted, now match the return type

 		//if (match_type<T>(return_type, rt_begin, rt_end))
		//	return true;
		return false;
	};
}


auto match_constructor(const std::string &ctor_name,
		const std::vector<std::string> &params)
{
	return [params, ctor_name](const mangled_storage_impl::entry& e)
	{
		//ok, we need to tokenize it, but that should be rathers fast
		tokenizer<tokenize_function> tkz(e.demangled);
		auto itr = tkz.begin();

		if (   (*itr != "public:")
			&& (*itr != "private:")
			&& (*itr != "protected:"))
			return false;
		itr++;
		if (itr == tkz.end())
			return false;

		if (*itr != "__thiscall")
			return false;
		itr++;
		if (itr == tkz.end())
			return false;

		if (*itr != ctor_name)
			return false;

		itr++;
		if (itr == tkz.end()) //no function.
			return false;

		if (*itr != "(")
			return false;
		itr++;

		if (!match_params(params, itr, tkz.end()))
			return false;

		if (itr == tkz.end())
			return false;
		if (*itr != ")")
			return false;
		itr++;

		if (itr != tkz.end()) //dummy token in itanium
			if (*itr == "")
				itr++;

		if (itr != tkz.end())
			return false;

		return true;
	};
}



template<typename T> std::string mangled_storage_impl::get_variable(const std::string &name)
{
	using namespace std;
	using namespace boost;

	namespace x3 = spirit::x3;
	using namespace parser;

	auto type_name = get_name<T>();

	auto matcher =
			-(visibility >> static_) >> //it may be a static class-member
			parser::type_rule<T>(type_name) >>
			name;

	auto predicate = [&](const mangled_storage_base::entry & e)
		{
			if (e.demangled == name)//maybe not mangled,
				return true;

			auto itr = e.demangled.begin();
			auto end = e.demangled.end();
			auto res = x3::parse(itr, end, matcher);
			return res && (itr == end);
		};

	auto found = std::find_if(storage.begin(), storage.end(), predicate);

	if (found != storage.end())
		return found->mangled;
	else
		return "";
}

template<typename Func> std::string mangled_storage_impl::get_function(const std::string &name)
{

	auto predicate =
		match_function<typename function_traits<Func>::result_type>(
			get_func_params(dummy<Func>()),
			name,
			get_return_type(dummy<Func>()));

	auto found = std::find_if(storage.begin(), storage.end(), predicate);

	if (found != storage.end())
		return found->mangled;
	else
		return "";

}

template<typename Class, typename Func>
std::string mangled_storage_impl::get_mem_fn(const std::string &name)
{
	auto predicate = match_mem_fn<typename function_traits<Func>::result_type>(
			get_name<Class>(),
			get_func_params(dummy<Func>()),
			name,
			get_return_type(dummy<Func>()),
			is_const<Class>::value,
			is_volatile<Class>::value);

	auto found = std::find_if(storage.begin(), storage.end(), predicate);

	if (found != storage.end())
		return found->mangled;
	else
		return "";

}


template<typename Signature>
auto mangled_storage_impl::get_constructor() -> ctor_sym
{

	std::string ctor_name; // = class_name + "::" + name;
	std::string unscoped_cname; //the unscoped class-name
	{
		auto class_name = get_return_type(dummy<Signature>());
		auto pos = class_name.rfind("::");
		if (pos == std::string::npos)
		{
			ctor_name = class_name+ "::" +class_name ;
			unscoped_cname = class_name;
		}
		else
		{
			unscoped_cname = class_name.substr(pos+2) ;
			ctor_name = class_name+ "::" + unscoped_cname;
		}
	}
	auto predicate = match_constructor(
				ctor_name,
				get_func_params(dummy<Signature>()));

	auto f = std::find_if(storage.begin(), storage.end(), predicate);

	if (f != storage.end())
		return f->mangled;
	else
		return "";
}

template<typename Class>
auto mangled_storage_impl::get_destructor() -> dtor_sym
{

	std::string dtor_name; // = class_name + "::" + name;
	std::string unscoped_cname; //the unscoped class-name
	{
		auto class_name = get_name<Class>();
		auto pos = class_name.rfind("::");
		if (pos == std::string::npos)
		{
			dtor_name = class_name+ "::~" + class_name  + "(void)";
			unscoped_cname = class_name;
		}
		else
		{
			unscoped_cname = class_name.substr(pos+2) ;
			dtor_name = class_name+ "::~" + unscoped_cname + "(void)";
		}
	}

	auto found = std::find_if(storage.begin(), storage.end(),
			[&](const entry& e)
			{
				using namespace std;
				char_separator<char> sep(" ");
				tokenizer<char_separator<char>> tkz(e.demangled, sep);
				auto itr = tkz.begin();
				if (  (*itr == "public:")
					||(*itr == "private:")
					||(*itr == "protected:"))
					{
						itr++;
						if (itr == tkz.end())
							return false;
					}

				if (*itr == "virtual")
				{
					itr++;
					if (itr == tkz.end())
						return false;
				}

				if (*itr == "__thiscall")
				{
					itr++;
					if (itr == tkz.end())
						return false;
				}
				else
					return false;

				return (*itr == dtor_name);
			});



	if (found != storage.end())
		return found->demangled;
	else
		return "";

}




}}}



#endif /* INCLUDE_BOOST_DLL_DETAIL_DEMANGLING_MSVC_HPP_ */
