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
#include <array>
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
	//function to remove preceeding 'class ' or 'struct ' if the are given in this format.

	inline static void trim_typename(std::string & val);
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

	template<typename T> //overload, does not need to virtual.
	std::string get_name() const
	{
		auto nm = mangled_storage_base::get_name<T>();
		trim_typename(nm);
		return nm;
	}
};

void mangled_storage_impl::trim_typename(std::string & val)
{
	//remove preceeding class or struct, because you might want to use a struct as class, et vice versa
	if (val.size() >= 6)
	{
		using namespace std;
		static constexpr char class_ [7] = "class ";
		static constexpr char struct_[8] = "struct ";

		if (equal(begin(class_), end(class_)-1, val.begin())) //aklright, starts with 'class '
			val.erase(0, 6);
		else if (val.size() >= 7)
			if (equal(begin(struct_), end(struct_)-1, val.begin()))
				val.erase(0, 7);
	}
}


namespace parser
{
	namespace x3 = spirit::x3;

	auto const visibility = ("public:" | x3::lit("protected:") | "private:");
	auto const virtual_ = x3::space >> "virtual";
	auto const static_ 	= x3::space >> x3::lit("static") ;

	auto const_rule_impl(true_type )  {return x3::space >> "const";};
	auto const_rule_impl(false_type)  {return x3::eps;};
	template<typename T>
	auto const_rule() {using t = is_const<typename remove_reference<T>::type>; return const_rule_impl(t());}

	auto volatile_rule_impl(true_type )  {return x3::space >> "volatile";};
	auto volatile_rule_impl(false_type)  {return x3::eps;};
	template<typename T>
	auto volatile_rule() {using t = is_volatile<typename remove_reference<T>::type>; return volatile_rule_impl(t());}


	auto inv_const_rule_impl(true_type )  {return "const" >>  x3::space ;};
	auto inv_const_rule_impl(false_type)  {return x3::eps;};
	template<typename T>
	auto inv_const_rule() {using t = is_const<typename remove_reference<T>::type>; return inv_const_rule_impl(t());}

	auto inv_volatile_rule_impl(true_type )  {return "volatile" >> x3::space;};
	auto inv_volatile_rule_impl(false_type)  {return x3::eps;};
	template<typename T>
	auto inv_volatile_rule() {using t = is_volatile<typename remove_reference<T>::type>; return inv_volatile_rule_impl(t());}


	auto reference_rule_impl(false_type, false_type) {return x3::eps;}
	auto reference_rule_impl(true_type,  false_type) {return x3::space >>"&"  ;}
	auto reference_rule_impl(false_type, true_type ) {return x3::space >>"&&" ;}


	template<typename T>
	auto reference_rule() {using t_l = is_lvalue_reference<T>; using t_r = is_rvalue_reference<T>; return reference_rule_impl(t_l(), t_r());}

	auto const class_ = ("class" | x3::lit("struct"));

	//it takes a string, because it may be overloaded.
	template<typename T>
	auto type_rule(const std::string & type_name)
	{
		using namespace std;

		return -(class_ >> x3::space)>> type_name >>
				const_rule<T>() >>
				volatile_rule<T>() >>
				reference_rule<T>();
	}
	template<>
	auto type_rule<void>(const std::string &) { return x3::lit("void"); };

	auto const cdecl_  	= "__cdecl" 	>> x3::space;
	auto const stdcall  = "__stdcall" 	>> x3::space;
	auto const thiscall = "__thiscall" 	>> x3::space;



	template<typename Return, typename Arg>
	auto arg_list(const mangled_storage_impl & ms, Return (*)(Arg), std::string * p)
	{
		using namespace std;
		*p = ms.get_name<Arg>();
		return type_rule<Arg>(*p);
	}

	template<typename Return, typename First, typename Second, typename ...Args>
	auto arg_list(const mangled_storage_impl & ms, Return (*)(First, Second, Args...), std::string * p)
	{
		*p = ms.get_name<First>();

		using next_type = Return (*)(Second, Args...);
		return type_rule<First>(*p) >> x3::char_(',') >> arg_list(ms, next_type(), p + 1);
	}

	template<typename Return>
	auto arg_list(const mangled_storage_impl & ms, Return (*)(), std::string * p)
	{
		return x3::lit("void");
	}
}


template<typename T> std::string mangled_storage_impl::get_variable(const std::string &name)
{
	using namespace std;
	using namespace boost;

	namespace x3 = spirit::x3;
	using namespace parser;

	auto type_name = get_name<T>();

	auto matcher =
			-(visibility >> static_ >> x3::space) >> //it may be a static class-member
			parser::type_rule<T>(type_name) >> x3::space >>
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
	namespace x3 = spirit::x3;
	using namespace parser;
	using func_type = Func*;
	using return_type = typename function_traits<Func>::result_type;
	std::string return_type_name = get_name<return_type>();

	std::array<std::string, function_traits<Func>::arity> arg_buf; //a buffer for the argument types.
	//if the rule is constructed in functinos and the string goes out of scope, the rule will noch match


	auto matcher =
				-(visibility >> static_ >> x3::space) >> //it may be a static class-member, which does however not have the static attribute.
				parser::type_rule<return_type>(return_type_name) >>  x3::space >>
				cdecl_ >> //cdecl declaration for methods. stdcall cannot be
				name >> x3::lit('(') >> parser::arg_list(*this, func_type(), arg_buf.data()) >> x3::lit(')') ;


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

template<typename Class, typename Func>
std::string mangled_storage_impl::get_mem_fn(const std::string &name)
{
	namespace x3 = spirit::x3;
	using namespace parser;
	using func_type = Func*;
	using return_type = typename function_traits<Func>::result_type;
	auto return_type_name = get_name<return_type>();


	std::array<std::string, function_traits<Func>::arity> arg_buf; //a buffer for the argument types.
	//if the rule is constructed in functinos and the string goes out of scope, the rule will noch match

	auto cname = get_name<Class>();

	auto matcher =
				visibility >> -virtual_ >> x3::space >>
				parser::type_rule<return_type>(return_type_name) >>  x3::space >>
				thiscall >> //cdecl declaration for methods. stdcall cannot be
				cname >> "::" >> name >>
				x3::lit('(') >> parser::arg_list(*this, func_type(), arg_buf.data()) >> x3::lit(')') >>
				inv_const_rule<Class>() >> inv_volatile_rule<Class>();

	auto predicate = [&](const mangled_storage_base::entry & e)
			{
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


template<typename Signature>
auto mangled_storage_impl::get_constructor() -> ctor_sym
{
	namespace x3 = spirit::x3;
	using namespace parser;

	using func_type = Signature*;

	std::array<std::string, function_traits<Signature>::arity> arg_buf; //a buffer for the argument types.

	std::string ctor_name; // = class_name + "::" + name;
	std::string unscoped_cname; //the unscoped class-name
	{
		auto class_name = get_return_type(dummy<Signature>());
		auto pos = class_name.rfind("::");
		if (pos == std::string::npos)
		{
			ctor_name = class_name+ "::" + class_name ;
			unscoped_cname = class_name;
		}
		else
		{
			unscoped_cname = class_name.substr(pos+2) ;
			ctor_name = class_name+ "::" + unscoped_cname;
		}
	}

	auto matcher =
				visibility >> x3::space >>
				thiscall >> //cdecl declaration for methods. stdcall cannot be
				ctor_name >>
				x3::lit('(') >> parser::arg_list(*this, func_type(), arg_buf.data()) >> x3::lit(')');


	auto predicate = [&](const mangled_storage_base::entry & e)
			{
				auto itr = e.demangled.begin();
				auto end = e.demangled.end();
				auto res = x3::parse(itr, end, matcher);

				return res && (itr == end);
			};

	auto f = std::find_if(storage.begin(), storage.end(), predicate);

	if (f != storage.end())
		return f->mangled;
	else
		return "";
}

template<typename Class>
auto mangled_storage_impl::get_destructor() -> dtor_sym
{
	namespace x3 = spirit::x3;
	using namespace parser;
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

	auto matcher =
				visibility >> -virtual_ >> x3::space >>
				thiscall >> //cdecl declaration for methods. stdcall cannot be
				dtor_name;


	auto predicate = [&](const mangled_storage_base::entry & e)
				{
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




}}}



#endif /* INCLUDE_BOOST_DLL_DETAIL_DEMANGLING_MSVC_HPP_ */
