// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Copyright 2015 Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/minimal.hpp>
#include <boost/dll.hpp>
#include <string>

#include "shared_lib_path.hpp"

// Unit Tests
int test_main(int argc, char* argv[]) {
    using namespace boost;

    BOOST_CHECK(argc >= 2);
    filesystem::path path_to_shared_library = argv[1];
    BOOST_CHECK(path_to_shared_library.string().find("getting_started_library") != std::string::npos);
    
    //[getting_started_imports_c_function
    // Importing pure C function
    function<int(int)> c_func = dll::import<int(int)>(
            path_to_shared_library, "c_func_name"
        );
    //]

    int c_func_res = c_func(1); // calling the function
    BOOST_CHECK(c_func_res == 2);


    //[getting_started_imports_c_variable
    // Importing pure C variable
    shared_ptr<int> c_var = dll::import<int>(
            path_to_shared_library, "c_variable_name"
        );
    //]

    int c_var_old_contents = *c_var; // using the variable
    *c_var = 100;
    BOOST_CHECK(c_var_old_contents == 1);


    //[getting_started_imports_cpp_function
    // Importing C++ function
    typedef std::string str;
    function<str(const str&)> cpp_func = dll::import_alias<str(const str&)>(
            path_to_shared_library, "cpp_function_name"
        );
    //]

    // calling the function
    std::string cpp_func_res = cpp_func(std::string("In importer.")); 
    BOOST_CHECK(cpp_func_res == "In importer. Hello from lib!");

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    //[getting_started_imports_cpp11_function
    // You may use `auto cpp11_func` here for shortness
    function<int(std::string&&)> cpp11_func = dll::import_alias<int(std::string&&)>(
            path_to_shared_library, "cpp11_function"
        );
    //]

    // calling the function
    int cpp11_func_res = cpp11_func(std::string("In importer.")); 
    BOOST_CHECK(cpp11_func_res == sizeof("In importer.") - 1);
#endif


    //[getting_started_imports_cpp_variable
    // Importing C++ variable
    shared_ptr<std::string> cpp_var = dll::import_alias<std::string>(
            path_to_shared_library, "cpp_variable_name"
        );
    //]

    std::string cpp_var_old_contents = *cpp_var; // using the variable
    *cpp_var = "New value";
    BOOST_CHECK(cpp_var_old_contents == "some value");
    BOOST_CHECK(*cpp_var == "New value");

    return 0;
}

