// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org


#include <boost/dll.hpp>
#include <boost/dll/runtime_symbol_info.hpp>

#include <boost/test/minimal.hpp>
#include "../example/shared_lib_path.hpp"


// lib functions

typedef float (lib_version_func)();
typedef void  (say_hello_func)  ();
typedef int   (increment)       (int);

// exe function
extern "C" int BOOST_SYMBOL_EXPORT exef() {
    return 15;
}

// Unit Tests
int test_main(int argc, char* argv[]) {
    using namespace boost::dll;

    BOOST_CHECK(argc >= 2);
    boost::filesystem::path shared_library_path = shared_lib_path(argv[1], L"test_library");
    std::cout << "Library: " << shared_library_path;
    shared_library lib(shared_library_path);

    std::cout << std::endl;
    std::cout << "shared_library_: " << shared_library_path << std::endl;
    std::cout << "symbol_location: " << symbol_location(&lib.get<int>("integer_g")) << std::endl;
    std::cout << "lib.path():      " << lib.path() << std::endl;
    BOOST_CHECK(
        symbol_location(&lib.get<int>("integer_g")) == lib.path()
    );

/*    sl.get<int>("integer_g") = 10;
    BOOST_CHECK(sl.get<int>("integer_g") == 10);

    BOOST_CHECK(sl.get<say_hello_func>("say_hello"));
    sl.get<say_hello_func>("say_hello")();

    float ver = sl.get<lib_version_func>("lib_version")();
    BOOST_CHECK(ver == 1.0);

    int n = sl.get<increment>("increment")(1);
    BOOST_CHECK(n == 2);

    BOOST_CHECK(sl.get<const int>("const_integer_g") == 777);

    boost::function<int(int)> inc = sl.get<int(int)>("increment");
    BOOST_CHECK(inc(1) == 2);
    BOOST_CHECK(inc(2) == 3);
    BOOST_CHECK(inc(3) == 4);

    // Cheking that symbols are still available, after another load+unload of the library
    { shared_library sl2(shared_library_path); }

    BOOST_CHECK(inc(1) == 2);
    BOOST_CHECK(sl.get<int>("integer_g") == 10);


    // Checking aliases
    boost::function<std::size_t(const std::vector<int>&)> sz 
        = sl.get_alias<std::size_t(const std::vector<int>&)>("foo_bar");

    std::vector<int> v(10);
    BOOST_CHECK(sz(v) == 10);
    BOOST_CHECK(sl.get_alias<std::size_t>("foo_variable") == 42);


    sz = sl.get<std::size_t(*)(const std::vector<int>&)>("foo_bar");
    BOOST_CHECK(sz(v) == 10);
    BOOST_CHECK(*sl.get<std::size_t*>("foo_variable") == 42);
    
    { // self
        shared_library sl;
        sl.load_self();
        int val = sl.get<int(void)>("exef")();
        BOOST_CHECK(val == 15);   
    }*/

    return 0;
}

