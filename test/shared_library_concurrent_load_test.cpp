// Copyright 2015 Antony Polukhin
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org


#include "../example/shared_lib_path.hpp"
#include <boost/dll.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/thread.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/bind.hpp>
#include <cctype>
#include <vector>

typedef std::vector<boost::filesystem::path> paths_t;

// Disgusting workarounds for b2 on Windows platform
inline paths_t generate_paths(int argc, char* argv[]) {
    paths_t ret;
    ret.reserve(argc - 1);

    for (int i = 1; i < argc; ++i) {
        boost::filesystem::path p = argv[i];
        if (dll_test::is_shared_library(p)) {
            ret.push_back(p);
        }
    }

    return ret;
}

inline void load_unload(const paths_t& paths, std::size_t count) {
    boost::filesystem::path ret;

    for (std::size_t j = 0; j < count; ++j) {
        for (std::size_t i = 0; i < paths.size(); ++i) {
            boost::dll::shared_library lib(paths[i]);
            BOOST_TEST(lib);
        }
    }
}


int main(int argc, char* argv[]) {
    BOOST_TEST(argc >= 3);
    paths_t paths = generate_paths(argc, argv);
    std::cout << "Libraries:\n";
    std::copy(paths.begin(), paths.end(), std::ostream_iterator<boost::filesystem::path>(std::cout, ", "));
    std::cout << std::endl;

    boost::thread_group threads;
    const std::size_t thread_count = 4;
    for (std::size_t i = 0; i < thread_count; ++i) {
        threads.create_thread(boost::bind(load_unload, paths, 1000));
    }
    threads.join_all();

    return boost::report_errors();
}
