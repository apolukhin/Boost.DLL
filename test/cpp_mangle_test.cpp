// Copyright 2016 Klemens Morgenstern
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/predef.h>

#if (__cplusplus >= 201402L) || (BOOST_COMP_MSVC >= BOOST_VERSION_NUMBER(14,0,0))

#include "../example/b2_workarounds.hpp"

#include <boost/dll/smart_library.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/variant.hpp>

#include <iostream>


struct override_class {};

#include <string>

class alias;

namespace space {
template <typename... T>
class test_template_class {
};

template <typename T1, typename T2 = char,
         typename T3 = test_template_class<int>>
class test_template_class_have_default_args {
};
} // namespace space

int main(int argc, char* argv[])
{
    using namespace boost::dll;
    using mangled_storage = detail::mangled_storage_impl;

    boost::dll::fs::path pt = b2_workarounds::first_lib_from_argv(argc, argv);

    std::cout << "Library: " << pt << std::endl;
    library_info lib{pt};

    mangled_storage ms(lib);

    std::cout << "Symbols: " << std::endl;

    for (auto &s : ms.get_storage())
    {
        std::cout << s.demangled << std::endl;
    }

    std::string v;
    v = ms.get_variable<double>("some_space::variable");

    BOOST_TEST(!v.empty()); //check if a symbols was found.
    BOOST_TEST(v != "some_space::variable"); //demangle is different

    v  = ms.get_variable<double>("some_space::variable_typo");
    BOOST_TEST(v.empty());


    v = ms.get_variable<const double>("unscoped_c_var");

    BOOST_TEST(!v.empty()); //check if a symbols was found.

    v = ms.get_variable<int>("unscoped_var");

    BOOST_TEST(!v.empty()); //check if a symbols was found.


    v = ms.get_function<const int &()>("some_space::scoped_fun");

    BOOST_TEST(!v.empty());
    BOOST_TEST(v != "some_space::scoped_fun");


    auto v1 = ms.get_function<void(const double)>("overloaded");
    auto v2 = ms.get_function<void(const volatile int)>("overloaded");
    BOOST_TEST(!v1.empty());
    BOOST_TEST(!v2.empty());
    BOOST_TEST(v1 != v2);

    v = ms.get_variable<int>("some_space::some_class::value");
    BOOST_TEST(!v.empty());
    BOOST_TEST(v != "some_space::some_class::value");

    v = ms.get_function<void(const int &)>("some_space::some_class::set_value");

    BOOST_TEST(!v.empty());
    BOOST_TEST(v != "some_space::some_class::set_value");



    ms.add_alias<override_class>("some_space::some_class");

    auto ctor1 = ms.get_constructor<override_class()>();
    BOOST_TEST(!ctor1.empty());

    auto ctor2 = ms.get_constructor<override_class(int)>();
    BOOST_TEST(!ctor2.empty());


    v = ms.get_mem_fn<override_class, double(double, double)>("func");
    BOOST_TEST(!v.empty());

    v = ms.get_mem_fn<override_class, int(int, int)>("func");
    BOOST_TEST(!v.empty());


    auto dtor = ms.get_destructor<override_class>();

    BOOST_TEST(!dtor.empty());

// TODO: ms.get_name on Clang has space after comma `boost::variant<double, int>`
#if !(defined(BOOST_TRAVISCI_BUILD) && defined(_MSC_VER) && defined(BOOST_CLANG))
    auto var1 = ms.get_function<void(boost::variant<int, double> &)>("use_variant");
    auto var2 = ms.get_function<void(boost::variant<double, int> &)>("use_variant");

    BOOST_TEST(!var1.empty());
    BOOST_TEST(!var2.empty());
#endif

#ifndef BOOST_NO_RTTI

#if defined(_MSC_VER) // MSVC, Clang-cl, and ICC on Windows
    auto vtable = ms.get_vtable<override_class>();
    BOOST_TEST(!vtable.empty());
#else
    auto ti  = ms.get_type_info<override_class>();
    BOOST_TEST(!ti.empty());
#endif

#endif // #ifndef BOOST_NO_RTTI

    ms.add_alias<alias>("space::cpp_plugin_type_pasrser");

    auto ctor1 = ms.get_constructor<alias()>();
    BOOST_TEST(!ctor1.empty());

    auto ctor2 = ms.get_constructor<alias(int*)>();
    BOOST_TEST(!ctor2.empty());

    auto ctor3 = ms.get_constructor<alias(const int*)>();
    BOOST_TEST(!ctor3.empty());

    auto ctor4 = ms.get_constructor<alias(const volatile int*)>();
    BOOST_TEST(!ctor4.empty());

    auto ctor5 = ms.get_constructor<alias(const std::string&)>();
    BOOST_TEST(!ctor5.empty());

    auto ctor6 = ms.get_constructor<alias(const volatile std::string*)>();
    BOOST_TEST(!ctor6.empty());

    v = ms.get_mem_fn<alias, void(int*)>("type_test");
    BOOST_TEST(!v.empty());

    v = ms.get_mem_fn<alias, void(const int*)>("type_test");
    BOOST_TEST(!v.empty());

    v = ms.get_mem_fn<alias, void(const volatile int*)>("type_test");
    BOOST_TEST(!v.empty());

    v = ms.get_mem_fn<alias, void(std::string*)>("type_test");
    BOOST_TEST(!v.empty());

    v = ms.get_mem_fn<alias, void(const std::string*)>("type_test");
    BOOST_TEST(!v.empty());

    v = ms.get_mem_fn<alias, void(const volatile std::string*)>("type_test");
    BOOST_TEST(!v.empty());

    v = ms.get_mem_fn<alias, void(const std::string&)>("type_test");
    BOOST_TEST(!v.empty());

    v = ms.get_mem_fn<alias, void(const space::test_template_class<>&)>(
        "type_test");
    BOOST_TEST(!v.empty());

    v = ms.get_mem_fn<alias, void(const space::test_template_class<void(int)>&)>(
        "type_test");
    BOOST_TEST(!v.empty());

    v = ms.get_mem_fn<alias, void(const space::test_template_class<int>&)>(
        "type_test");
    BOOST_TEST(!v.empty());

    v = ms.get_mem_fn<alias,
                      void(const space::test_template_class<std::string>&)>(
        "type_test");
    BOOST_TEST(!v.empty());

    v = ms.get_mem_fn<
        alias, void(const space::test_template_class<char, int, std::string>&)>(
        "type_test");
    BOOST_TEST(!v.empty());

    v = ms.get_mem_fn<
        alias, void(const space::test_template_class_have_default_args<int>&)>(
        "type_test");
    BOOST_TEST(!v.empty());

    v = ms.get_mem_fn<
        alias,
        void(const space::test_template_class_have_default_args<int, double>&)>(
        "type_test");
    BOOST_TEST(!v.empty());

    v = ms.get_mem_fn<alias,
                      void(const space::test_template_class_have_default_args<
                           int, double, std::string>&)>("type_test");
    BOOST_TEST(!v.empty());

    auto dtor = ms.get_destructor<alias>();

    BOOST_TEST(!dtor.empty());

    return boost::report_errors();
}

#else
int main() {return 0;}
#endif
