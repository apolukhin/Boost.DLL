/*
 * itanium.hpp
 *
 *  Created on: 08.10.2015
 *      Author: Klemens
 */

#ifndef BOOST_DLL_DETAIL_DEMANGLING_ITANIUM_HPP_
#define BOOST_DLL_DETAIL_DEMANGLING_ITANIUM_HPP_

#include <boost/dll/detail/demangling/mangled_storage_base_base.hpp>
#include <iterator>
#include <algorithm>
#include <boost/dll/detail/demangling/tokenizer.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/is_volatile.hpp>

namespace boost { namespace dll { namespace detail {



class mangled_storage_impl : public mangled_storage_base
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
	using mangled_storage_base::mangled_storage_base;
	struct ctor_sym
	{
		std::string C0;
		std::string C1;
		std::string C2;
		bool empty() const
		{
			return C0.empty() && C1.empty() && C2.empty();
		}
	};

	struct dtor_sym
	{
		std::string D0;
		std::string D1;
		std::string D2;
		bool empty() const
		{
			return D0.empty() && D1.empty() && D2.empty();
		}
	};

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

inline bool match_types(const std::string &lhs, const std::string &rhs)
{
	using namespace boost;
	using namespace std;
	char_separator<char> sep(" ");
	tokenizer<char_separator<char>> tlhs(lhs, sep);
	tokenizer<char_separator<char>> trhs(rhs, sep);

	vector<string> vlhs(tlhs.begin(), tlhs.end());
	vector<string> vrhs(trhs.begin(), trhs.end());
	if (vlhs.size() != vrhs.size())
		return false;

	sort(vlhs.begin(), vlhs.end());
	sort(vrhs.begin(), vrhs.end());

	return std::equal(vlhs.begin(), vlhs.end(), vrhs.begin());
}


template<typename Range, typename Iterator>
bool match_params(const Range &range, Iterator & itr, const Iterator & end)
{
	for (auto &p : range)
	{
		if (itr == end)
		{
			return false;
		}
		if (! match_types(p, *itr))
			return false;
		itr++;
	}
	return true;
}


auto match_function(
		const std::vector<std::string> &params,
		const std::string& name)
{
	return [params, name](const mangled_storage_impl::entry& e)
	{
		//ok, we need to tokenize it, but that should be rathers fast
		tokenizer<tokenize_function> tkz(e.demangled);
		auto itr = tkz.begin();
		if (itr == tkz.end()) //errornous name
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

		if (itr == tkz.end())
			return false;
		if (*itr != ")")
			return false;
		itr++;
		if (itr != tkz.end())
			return false;

		return true;
	};
}

auto match_mem_fn(const std::string &class_name,
		const std::vector<std::string> &params,
		const std::string& name,
		bool is_const = false,
		bool is_volatile = false)
{
	auto mem_fn_name = class_name + "::" + name;

	return [params, name, is_const, is_volatile, mem_fn_name]
			(const mangled_storage_impl::entry& e)
	{
		//ok, we need to tokenize it, but that should be rathers fast
		tokenizer<tokenize_function> tkz(e.demangled);
		auto itr = tkz.begin();

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

		if (itr != tkz.end()) //dummy token in itanium
			if (*itr == "")
				itr++;

		if (itr != tkz.end())
			if ((*itr == "const") && is_const)
				itr++;

		if (itr != tkz.end())
			if ((*itr == "volatile") && is_volatile)
				itr++;

		if (itr != tkz.end())
			return false;

		return true;
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

		if (*itr != ctor_name)
			return false;

		itr++;
		if (itr == tkz.end()) //no function.
			return false;

		if (*itr != "(")
			return false;
		itr++;
		using namespace std;;

		for (auto i = itr; i != tkz.end(); i++)
			cout << *i << ", ";
		cout << endl;

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
	auto found = std::find_if(storage.begin(), storage.end(), [&](const entry& e)
			{
				if (e.demangled == name)
					return true;
				else
					return false;
			});

	if (found != storage.end())
		return found->mangled;
	else
		return "";
}

template<typename Func> std::string mangled_storage_impl::get_function(const std::string &name)
{
	auto predicate = match_function(get_func_params(dummy<Func>()), name);

	auto found = std::find_if(storage.begin(), storage.end(), predicate);

	if (found != storage.end())
		return found->mangled;
	else
		return "";

}

template<typename Class, typename Func>
std::string mangled_storage_impl::get_mem_fn(const std::string &name)
{
	auto predicate = match_mem_fn(
			get_name<Class>(),
			get_func_params(dummy<Func>()),
			name,
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

	std::vector<entry> findings;
	std::copy_if(storage.begin(), storage.end(),
			std::back_inserter(findings), predicate);

	ctor_sym ct;

	for (auto & e : findings)
	{
		if (e.mangled.find(unscoped_cname +"C0E") != std::string::npos)
		{
			ct.C0 = e.mangled;
		}
		else if (e.mangled.find(unscoped_cname +"C1E") != std::string::npos)
		{
			ct.C1 = e.mangled;
		}
		else if (e.mangled.find(unscoped_cname +"C2E") != std::string::npos)
		{
			ct.C2 = e.mangled;
		}
	}
	return ct;
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
			dtor_name = class_name+ "::~" + class_name  + "()";
			unscoped_cname = class_name;
		}
		else
		{
			unscoped_cname = class_name.substr(pos+2) ;
			dtor_name = class_name+ "::~" + unscoped_cname + "()";
		}
	}

	auto d0 = unscoped_cname + "D0Ev";
	auto d1 = unscoped_cname + "D1Ev";
	auto d2 = unscoped_cname + "D2Ev";

	dtor_sym dt;
	//this is so simple, i don#t need a predicate
	for (auto & s : storage)
	{
		//alright, name fits
		if (s.demangled == dtor_name)
		{
			if (s.mangled.find(d0) != std::string::npos)
				dt.D0 = s.mangled;
			else if (s.mangled.find(d1) != std::string::npos)
				dt.D1 = s.mangled;
			else if (s.mangled.find(d2) != std::string::npos)
				dt.D2 = s.mangled;

		}
	}
	return dt;

}

}}}


#endif /* INCLUDE_BOOST_DLL_DETAIL_DEMANGLING_ITANIUM_HPP_ */
