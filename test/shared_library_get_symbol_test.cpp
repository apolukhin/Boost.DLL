// Copyright 2011-2012 Renato Tegon Forti.
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/plugin/shared_library.hpp>
#include <boost/test/minimal.hpp>
#include <boost/function.hpp>

#include <boost/filesystem.hpp>
namespace fs = ::boost::filesystem;
fs::path get_shared_lib(const fs::path& root, const std::wstring& filename_part) {  
    fs::directory_iterator it(root);
    fs::directory_iterator endit;

    while (it != endit) {
        if (fs::is_regular_file(*it) && it->path().filename().wstring().find(filename_part) != std::wstring::npos) {
            return *it;
        }
        ++it;
    }

    throw std::runtime_error("Failed to find library");
}

// lib functions

typedef float (lib_version_func)();
typedef void  (say_hello_func)  ();
typedef int   (increment)       (int);



// Unit Tests
int test_main(int argc, char* argv[]) {
    using namespace boost::application;

    BOOST_CHECK(argc >= 2);
    boost::filesystem::path shared_library_path = get_shared_lib(argv[1], L"test_library");
    std::cout << "Library: " << shared_library_path;

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

    boost::function<std::size_t(const std::vector<int>&)> sz 
        = sl.get<std::size_t(const std::vector<int>&)>("alias");

    std::cout << "Before alias call\n";

    std::vector<int> v(10);
    BOOST_CHECK(sz(v) == 10);

    std::cout << "Before c++ variable\n";

    BOOST_CHECK(sl.get<std::size_t>("variable") == 42);

    return 0;
}

