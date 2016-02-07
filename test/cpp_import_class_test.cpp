// Copyright 2016 Klemens Morgenstern
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/predef.h>

#if (__cplusplus >= 201402L) || (BOOST_COMP_MSVC >= BOOST_VERSION_NUMBER(14,0,0))

#include "../example/b2_workarounds.hpp"

#include <iostream>

using namespace std;

#include <boost/dll/smart_library.hpp>
#include <boost/dll/import_mangled.hpp>
#include <boost/dll/import_class.hpp>

#include <boost/core/lightweight_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/variant.hpp>



int main(int argc, char* argv[])
{   
	 using namespace boost::dll;
	using namespace boost::dll::experimental;
    boost::filesystem::path pt = b2_workarounds::first_lib_from_argv(argc, argv);

    BOOST_TEST(!pt.empty());
    std::cout << "Library: " << pt << std::endl;

    boost::shared_ptr<smart_library> sm = boost::make_shared<smart_library>(pt);

    auto static_val = import_mangled<int>(sm, "some_space::some_class::value");


    for (auto &s : sm->symbol_storage().get_storage())
        std::cout << s.demangled << std::endl;

   // boost::shared_ptr<smart_library> sm = boost::make_shared<smart_library>(pt);

    auto sp_variable = import_mangled<double>(sm, "some_space::variable");

    auto unscoped_var = import_mangled<int>(sm, "unscoped_var");

    std::size_t type_size = *import_mangled<std::size_t>(sm, "some_space::size_of_some_class");

    {
        class override_class{};
        auto cl = import_class<override_class, int>(sm, "some_space::some_class", type_size, 42);


        BOOST_TEST(!cl.is_copy_assignable());
        BOOST_TEST(!cl.is_copy_constructible());

#if (!defined(BOOST_MSVC) && !defined(BOOST_MSVC_FULL_VER))
        BOOST_TEST( cl.is_move_assignable());
        BOOST_TEST( cl.is_move_constructible());
#endif

        BOOST_TEST(*static_val == 42);

        auto i = cl.call<const override_class, int()>("get")();
        BOOST_TEST(i == 456);
        cl.call<void(int)>("set")(42);
        i = 0;
        i = cl.call<const override_class, int()>("get")();
        BOOST_TEST(i == 42);

        auto func = import_mangled<
                override_class, double(double, double), int(int, int),
                volatile override_class, int(int, int),
                const volatile override_class, double(double, double)>(sm, "func");



        BOOST_TEST((cl->*func)(3.,2.) == 6.);
        BOOST_TEST((cl->*func)(1 ,2 ) == 3 );

        auto fun2 = cl.import<double(double, double), int(int, int)>("func");

        BOOST_TEST((cl->*fun2)(3.,2.) == 6.);
        BOOST_TEST((cl->*fun2)(5 ,2 ) == 7 );

    }

    BOOST_TEST(*static_val == 0);

    return boost::report_errors();
}

#else
int main() {return 0;}
#endif
