// Copyright 2011-2012 Renato Tegon Forti.
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/test/minimal.hpp>
#include <boost/dll.hpp>
#include <string>

#include "shared_lib_path.hpp"

// Unit Tests
int test_main(int argc, char* argv[]) {
    using namespace boost;

    BOOST_CHECK(argc >= 2);
    filesystem::path path_to_shared_library = shared_lib_path(argv[1], L"getting_started_library");
    
    //[getting_started_imports_c_function
    function<int(int)> c_func = dll::shared_function<int(int)>(
        path_to_shared_library, "c_func_name"
    );
    //]

    int c_func_res = c_func(1); // calling the function
    BOOST_CHECK(c_func_res == 2);


    //[getting_started_imports_c_variable
    shared_ptr<int> c_var = dll::shared_variable<int>(
        path_to_shared_library, "c_variable_name"
    );
    //]

    int c_var_old_contents = *c_var; // using the variable
    *c_var = 100;
    BOOST_CHECK(c_var_old_contents == 1);


    //[getting_started_imports_cpp_function
    typedef std::string(func_type)(const std::string&);

    function<func_type> cpp_func
        = dll::shared_function_alias<func_type>(
            path_to_shared_library, "cpp_function_alias_name"
        );
    //]

    // calling the function
    std::string cpp_func_res = cpp_func(std::string("In importer.")); 
    BOOST_CHECK(cpp_func_res == "In importer. Hello from lib!");

    //[getting_started_imports_cpp_variable    
    shared_ptr<std::string> cpp_var
        = dll::shared_variable_alias<std::string>(
            path_to_shared_library, "cpp_variable_alias_name"
        );
    //]

    std::string cpp_var_old_contents = *cpp_var; // using the variable
    *cpp_var = "New value";
    BOOST_CHECK(cpp_var_old_contents == "some value");
    BOOST_CHECK(*cpp_var == "New value");

    return 0;
}

