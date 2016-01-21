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

#define L() std::cout << "Line: " << __LINE__ << std::endl;

struct override_class
{
	int arr[32];
};


int test_main(int argc, char* argv[])
{
    using namespace boost::dll;

    BOOST_REQUIRE(argc >= 3);

    //argv[2] == dll location.

    boost::filesystem::path a1(argv[1]);
    boost::filesystem::path a2(argv[2]);


    boost::filesystem::path * selection;
    if ((a1.extension() == ".dll") || (a1.extension() == ".so"))
    	selection = &a1;
    else if ((a2.extension() == ".dll") || (a2.extension() == ".so"))
    	selection = &a2;
    else
    	return 1;

    smart_library sm(*selection);

    auto &unscoped_var = sm.get_variable<int>("unscoped_var");


    BOOST_REQUIRE(&unscoped_var != nullptr);
    BOOST_CHECK(unscoped_var == 42);

    auto & unscoped_c_var = sm.get_variable<const double>("unscoped_c_var");

    BOOST_REQUIRE(&unscoped_c_var != nullptr);
    BOOST_CHECK(unscoped_c_var == 1.234);


    auto & sp_variable = sm.get_variable<double>("some_space::variable");

    BOOST_REQUIRE(&sp_variable != nullptr);
    BOOST_CHECK(sp_variable == 0.2);


    auto scoped_fun = sm.get_function<const int&()>("some_space::scoped_fun");

    BOOST_REQUIRE(scoped_fun != nullptr);
    {
    	auto &res = scoped_fun();
    	BOOST_CHECK(res == 0xDEADBEEF);
    }

    auto ovl1 = sm.get_function<void(int)>   ("overloaded");
    auto ovl2 = sm.get_function<void(double)>("overloaded");

    BOOST_REQUIRE(ovl1 != nullptr);
    BOOST_REQUIRE(ovl2 != nullptr);
    BOOST_CHECK(reinterpret_cast<void*>(ovl1) != reinterpret_cast<void*>(ovl2));

    ovl1(12);
    BOOST_CHECK(unscoped_var == 12);
    ovl2(5.0);
    BOOST_CHECK(sp_variable == 5.0);



	auto var1 = sm.get_function<void(boost::variant<int, double> &)>("use_variant");
	auto var2 = sm.get_function<void(boost::variant<double, int> &)>("use_variant");

	BOOST_REQUIRE(var1 != nullptr);
	BOOST_REQUIRE(var2 != nullptr);
	BOOST_CHECK(reinterpret_cast<void*>(var1) != reinterpret_cast<void*>(var2));

    {
    	 boost::variant<int, double> v1 = 232.22;
    	 boost::variant<double, int> v2 = -1;

    	 var1(v1);
    	 var2(v2);

    	 struct : boost::static_visitor<void>
    	 {
    		 void operator()(double) {BOOST_CHECK(false);}
    		 void operator()(int i) {BOOST_CHECK(i == 42);}
    	 } vis1;

    	 struct : boost::static_visitor<void>
    	 {
    		 void operator()(double d) {BOOST_CHECK(d == 3.124);}
    		 void operator()(int ) {BOOST_CHECK(false);}
    	 } vis2;

    	 boost::apply_visitor(vis1, v1);
    	 boost::apply_visitor(vis2, v2);

    }

    /* now test the class stuff */

    //first we import and test the global variables

    auto &father_val = sm.get_variable<int>("some_space::father_value");
    auto &static_val = sm.get_variable<int>("some_space::some_class::value");


    BOOST_REQUIRE(&father_val != nullptr);
    BOOST_REQUIRE(&static_val != nullptr);

    BOOST_CHECK(father_val == 12);
    BOOST_CHECK(static_val == -1);
    //now get the static function.
    auto set_value = sm.get_function<void(const int &)>("some_space::some_class::set_value");

    BOOST_REQUIRE(set_value != nullptr);

    set_value(42);
    BOOST_CHECK(static_val == 42); //alright, static method works.


    //alright, now import the class members
    //first add the type alias.
    sm.add_type_alias<override_class>("some_space::some_class");

    auto set = sm.get_mem_fn<override_class, void(int)>("set");

    {
    	using namespace std;
    	try
    	{
    		sm.get_mem_fn<override_class, int()>("get");
    		BOOST_REQUIRE(false);
    	}
    	catch(boost::system::system_error &) {}
    }
    auto get = sm.get_mem_fn<const override_class, int()>("get");

    BOOST_REQUIRE(get != nullptr);
    BOOST_REQUIRE(set != nullptr);

    auto func_dd  = sm.get_mem_fn<override_class, 				 double(double, double)>("func");
    auto func_ii  = sm.get_mem_fn<override_class, 				 int(int, int)>			("func");
    auto func_iiv = sm.get_mem_fn<volatile override_class, 		 int(int, int)>			("func");
    auto func_ddc = sm.get_mem_fn<const volatile override_class, double(double, double)>("func");


    BOOST_REQUIRE(func_dd != nullptr);
    BOOST_REQUIRE(func_ii != nullptr);


    auto ctor_v = sm.get_constructor<override_class()>();
    auto ctor_i = sm.get_constructor<override_class(int)>();

    auto dtor   = sm.get_destructor<override_class>();

    if (ctor_v.has_allocating())
    {
    	//allocate
    	auto p = ctor_v.allocating();

    	//assert it works
    	auto val = (p->*get)();
    	BOOST_CHECK(val == 123);

    	//deallocate
    	dtor.deleting(p);

    	//now i cannot assert that it deletes, since it would crash.
    }
    //ok, now load the ctor/dtor
    override_class oc;

    for (auto & i : oc.arr)
    	i = 0;

    BOOST_CHECK((oc.*get)() == 0);

    ctor_i.standard(&oc, 12); //initialized.

    BOOST_CHECK(static_val == 12);
    BOOST_CHECK((oc.*get)() == 456);
    (oc.*set)(42);
    BOOST_CHECK((oc.*get)() == 42);

    BOOST_CHECK((oc.*func_dd)(3,2)   == 6);
    BOOST_CHECK((oc.*func_ii)(1,2)   == 3);
    BOOST_CHECK((oc.*func_ddc)(10,2) == 5);
    BOOST_CHECK((oc.*func_iiv)(9,2)  == 7);


    dtor.standard(&oc);

    return 0;
}
