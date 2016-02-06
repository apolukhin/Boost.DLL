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
#include <boost/dll/import_mangled.hpp>

#include <boost/core/lightweight_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/variant.hpp>

#include <iostream>

struct override_class
{
    int arr[32];
};


int main(int argc, char* argv[])
{
    using namespace boost::dll;
    using namespace boost::dll::experimental;
    boost::filesystem::path pt = b2_workarounds::first_lib_from_argv(argc, argv);

    BOOST_TEST(!pt.empty());
    std::cout << "Library: " << pt << std::endl;

   // boost::shared_ptr<smart_library> sm = boost::make_shared<smart_library>(pt);
    smart_library sm(pt);

    auto sp_variable = import_mangled<double>(sm, "some_space::variable");

 /*   auto& unscoped_var = import_mangled<int>(sm, "unscoped_var");


    auto ovl = import_mangled<void(int), void(double)>(sm, "overloaded");

    ovl(12);
    BOOST_TEST(unscoped_var == 12);
    ovl(5.0);
    BOOST_TEST(sp_variable == 5.0);
*/
    return boost::report_errors();
}

#else
int main() {return 0;}
#endif
