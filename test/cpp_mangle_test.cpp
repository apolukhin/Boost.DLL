// Copyright 2011-2012 Renato Tegon Forti
// Copyright 2015 Antony Polukhin
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <iostream>



#include <boost/dll.hpp>
#include <boost/dll/smart_library.hpp>
#include <boost/test/minimal.hpp>
#include <boost/filesystem.hpp>
#include <boost/variant.hpp>

struct override_class {};


int test_main(int argc, char* argv[])
{
    using namespace boost::dll;
    using mangled_storage = detail::mangled_storage_impl;

    BOOST_REQUIRE(argc >= 3);
   
    //argv[2] == dll location.

    boost::filesystem::path a1(argv[1]);
    boost::filesystem::path a2(argv[2]);


    boost::filesystem::path * selection;
    if (a1.extension() == ".dll")
    	selection = &a1;
    else if (a2.extension() == ".dll")
    	selection = &a2;
    else
    	return 1;

    library_info lib{*selection};

    mangled_storage ms(lib);

    std::string v;
    v = ms.get_variable<double>("some_space::variable");

    BOOST_CHECK(!v.empty()); //check if a symbols was found.
    BOOST_CHECK(v != "some_space::variable"); //demangle is different

    v  = ms.get_variable<double>("some_space::variable_typo");
    BOOST_CHECK(v.empty());


	v = ms.get_variable<const double>("unscoped_c_var");

	BOOST_CHECK(!v.empty()); //check if a symbols was found.

	v = ms.get_variable<int>("unscoped_var");

	BOOST_CHECK(!v.empty()); //check if a symbols was found.


	v = ms.get_function<const int &()>("some_space::scoped_fun");

	BOOST_CHECK(!v.empty());
	BOOST_CHECK(v != "some_space::scoped_fun");


	auto v1 = ms.get_function<void(const double)>("overloaded");
	auto v2 = ms.get_function<void(const volatile int)>("overloaded");
	BOOST_CHECK(!v1.empty());
	BOOST_CHECK(!v2.empty());
	BOOST_CHECK(v1 != v2);

	v = ms.get_variable<int>("some_space::some_class::value");
	BOOST_CHECK(!v.empty());
	BOOST_CHECK(v != "some_space::some_class::value");

	v = ms.get_function<void(const int &)>("some_space::some_class::set_value");

	BOOST_CHECK(!v.empty());
	BOOST_CHECK(v != "some_space::some_class::set_value");



	ms.add_alias<override_class>("some_space::some_class");

	auto ctor1 = ms.get_constructor<override_class()>();
	BOOST_CHECK(!ctor1.empty());

	auto ctor2 = ms.get_constructor<override_class(int)>();
	BOOST_CHECK(!ctor2.empty());


	v = ms.get_mem_fn<override_class, double(double, double)>("func");
	BOOST_CHECK(!v.empty());

	v = ms.get_mem_fn<override_class, int(int, int)>("func");
	BOOST_CHECK(!v.empty());


	auto dtor = ms.get_destructor<override_class>();

	BOOST_CHECK(!dtor.empty());

	auto var1 = ms.get_function<void(boost::variant<int, double> &)>("use_variant");
	auto var2 = ms.get_function<void(boost::variant<double, int> &)>("use_variant");

	BOOST_CHECK(!var1.empty());
	BOOST_CHECK(!var2.empty());


    return 0;
}
