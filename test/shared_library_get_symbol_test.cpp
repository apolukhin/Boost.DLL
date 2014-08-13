// Copyright 2011-2012 Renato Tegon Forti.
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/plugin/shared_library.hpp>
#include <boost/plugin/alias.hpp>
#include <boost/plugin/refcountable.hpp>
#include <boost/test/minimal.hpp>
#include <boost/function.hpp>
#include <boost/fusion/container.hpp>

#include "../example/shared_lib_path.hpp"

// lib functions

typedef float (lib_version_func)();
typedef void  (say_hello_func)  ();
typedef int   (increment)       (int);

typedef boost::fusion::vector<std::vector<int>, std::vector<int>, std::vector<int>, const std::vector<int>*, std::vector<int>* > do_share_res_t;
typedef boost::shared_ptr<do_share_res_t> (do_share_t)(
            std::vector<int> v1,
            std::vector<int>& v2,
            const std::vector<int>& v3,
            const std::vector<int>* v4,
            std::vector<int>* v5
        );

void refcountable_test(boost::filesystem::path shared_library_path) {
    using namespace boost::plugin;
    using namespace boost::fusion;

    std::vector<int> v(1000);

    {
        boost::function<say_hello_func> sz2
            = shared_function<say_hello_func>(shared_library_path, "say_hello");

        sz2();
        sz2();
        sz2();
    }

    {
        boost::function<std::size_t(const std::vector<int>&)> sz
            = shared_function_alias<std::size_t(const std::vector<int>&)>(shared_library_path, "foo_bar");
        BOOST_CHECK(sz(v) == 1000);
    }


    {
        boost::function<do_share_t> f;

        {
            boost::function<do_share_t> f2 = shared_function_alias<do_share_t>(shared_library_path, "do_share");
            f = f2;
        }

        std::vector<int> v1(1, 1), v2(2, 2), v3(3, 3), v4(4, 4), v5(1000, 5);
        boost::shared_ptr<do_share_res_t> res = f(v1, v2, v3, &v4, &v5);

        BOOST_CHECK(at_c<0>(*res).size() == 1); BOOST_CHECK(at_c<0>(*res).front() == 1);
        BOOST_CHECK(at_c<1>(*res).size() == 2); BOOST_CHECK(at_c<1>(*res).front() == 2);
        BOOST_CHECK(at_c<2>(*res).size() == 3); BOOST_CHECK(at_c<2>(*res).front() == 3);
        BOOST_CHECK(at_c<3>(*res)->size() == 4); BOOST_CHECK(at_c<3>(*res)->front() == 4);
        BOOST_CHECK(at_c<4>(*res)->size() == 1000); BOOST_CHECK(at_c<4>(*res)->front() == 5);

        BOOST_CHECK(at_c<3>(*res) == &v4);
        BOOST_CHECK(at_c<4>(*res) == &v5);
        BOOST_CHECK(at_c<1>(*res).back() == 777);
        BOOST_CHECK(v5.back() == 9990);
    }

    {
        boost::shared_ptr<int> i = shared_variable<int>(shared_library_path, "integer_g");
        BOOST_CHECK(*i == 100);

        boost::shared_ptr<int> i2;
        i.swap(i2);
        BOOST_CHECK(*i2 == 100);
    }

    {
        boost::shared_ptr<const int> i = shared_variable<const int>(shared_library_path, "const_integer_g");
        BOOST_CHECK(*i == 777);

        boost::shared_ptr<const int> i2 = i;
        i.reset();
        BOOST_CHECK(*i2 == 777);
    }

    {
        boost::shared_ptr<std::string> s = shared_variable_alias<std::string>(shared_library_path, "info");
        BOOST_CHECK(*s == "I am a std::string from the test_library (Think of me as of 'Hello world'. Long 'Hello world').");

        boost::shared_ptr<std::string> s2;
        s.swap(s2);
        BOOST_CHECK(*s2 == "I am a std::string from the test_library (Think of me as of 'Hello world'. Long 'Hello world').");
    }
}

// exe function
extern "C" int BOOST_SYMBOL_EXPORT exef() {
    return 15;
}

// Unit Tests
int test_main(int argc, char* argv[]) {
    using namespace boost::plugin;

    BOOST_CHECK(argc >= 2);
    boost::filesystem::path shared_library_path = shared_lib_path(argv[1], L"test_library");
    std::cout << "Library: " << shared_library_path;


    refcountable_test(shared_library_path);



    shared_library sl(shared_library_path);

    BOOST_CHECK(sl.get<int>("integer_g") == 100);

    sl.get<int>("integer_g") = 10;
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
        = alias<std::size_t(const std::vector<int>&)>(sl, "foo_bar");

    std::vector<int> v(10);
    BOOST_CHECK(sz(v) == 10);
    BOOST_CHECK(alias<std::size_t>(sl, "foo_variable") == 42);


    sz = sl.get<std::size_t(*)(const std::vector<int>&)>("foo_bar");
    BOOST_CHECK(sz(v) == 10);
    BOOST_CHECK(*sl.get<std::size_t*>("foo_variable") == 42);
    
    { // self
        shared_library sl;
        sl.load_self();
        int val = sl.get<int(void)>("exef")();
        BOOST_CHECK(val == 15);   
    }

    return 0;
}

