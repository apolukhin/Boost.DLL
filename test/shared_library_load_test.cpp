// Copyright 2011-2012 Renato Tegon Forti
// Copyright 2015 Antony Polukhin
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/dll.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/filesystem.hpp>
#include <cctype>
// Unit Tests


inline boost::filesystem::path drop_version(const boost::filesystem::path& lhs) {
    boost::filesystem::path ext = lhs.filename().extension();
    if (ext.native().size() > 1 && std::isdigit(ext.string()[1])) {
        ext = lhs;
        ext.replace_extension().replace_extension().replace_extension();
        return ext;
    }

    return lhs;
}

inline bool lib_path_equal(const boost::filesystem::path& lhs, const boost::filesystem::path& rhs) {
    const bool res = (drop_version(lhs).filename() == drop_version(rhs).filename());
    if (!res) {
        std::cerr << "lhs != rhs: " << lhs << " != " << rhs << '\n';
    }
    return res;
}

struct fs_copy_guard {
    const boost::filesystem::path actual_path_;
    const bool same_;

    inline explicit fs_copy_guard(const boost::filesystem::path& shared_library_path)
        : actual_path_( drop_version(shared_library_path) )
        , same_(actual_path_ == shared_library_path)
    {
        if (!same_) {
            boost::system::error_code ignore;
            boost::filesystem::remove(actual_path_, ignore);
            boost::filesystem::copy(shared_library_path, actual_path_, ignore);
        }
    }

    inline ~fs_copy_guard() {
        if (!same_) {
            boost::system::error_code ignore;
            boost::filesystem::remove(actual_path_, ignore);
        }
    }
};

// Disgusting workarounds for b2 on Windows platform
inline boost::filesystem::path do_find_correct_libs_path(int argc, char* argv[], const char* lib_name) {
    boost::filesystem::path ret;

    for (int i = 1; i < argc; ++i) {
        ret = argv[i];
        if (ret.string().find(lib_name) != std::string::npos && ret.string().find(".lib") == std::string::npos) {
            return ret;
        }
    }

    return lib_name;
}

int main(int argc, char* argv[])
{
    using namespace boost::dll;

    BOOST_TEST(argc >= 3);
    boost::filesystem::path shared_library_path = do_find_correct_libs_path(argc, argv, "test_library");
    std::cout << "Library: " << shared_library_path;

    {
        shared_library sl(shared_library_path);
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(lib_path_equal(sl.location(), shared_library_path));

        shared_library sl2;
        BOOST_TEST(!sl2.is_loaded());
        BOOST_TEST(!sl2);

        swap(sl, sl2);
        BOOST_TEST(!sl.is_loaded());
        BOOST_TEST(!sl);
        BOOST_TEST(sl2.is_loaded());
        BOOST_TEST(sl2);
   }

   {
        boost::system::error_code ec;
        shared_library sl(shared_library_path, ec);
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
        BOOST_TEST(!ec);
        BOOST_TEST(lib_path_equal(sl.location(), shared_library_path));
        BOOST_TEST(lib_path_equal(sl.location(ec), shared_library_path));
        BOOST_TEST(!ec);
   }

   {
        shared_library sl;
        sl.load(shared_library_path);
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
        BOOST_TEST(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        shared_library sl;
        boost::system::error_code ec;
        sl.load(shared_library_path, ec);
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
        BOOST_TEST(!ec);
        BOOST_TEST(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        shared_library sl(shared_library_path, load_mode::load_with_altered_search_path );
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
        BOOST_TEST(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        boost::system::error_code ec;
        shared_library sl(shared_library_path, load_mode::load_with_altered_search_path, ec);
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
        BOOST_TEST(!ec);
        BOOST_TEST(lib_path_equal(sl.location(), shared_library_path));
        BOOST_TEST(lib_path_equal(sl.location(ec), shared_library_path));
        BOOST_TEST(!ec);
   }

   {
        shared_library sl;
        sl.load(shared_library_path, load_mode::load_with_altered_search_path);
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
        BOOST_TEST(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        shared_library sl;
        boost::system::error_code ec;
        sl.load(shared_library_path, load_mode::load_with_altered_search_path, ec);
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
        BOOST_TEST(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        shared_library sl(shared_library_path, load_mode::rtld_lazy | load_mode::rtld_global);
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
        BOOST_TEST(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        shared_library sl(shared_library_path, load_mode::rtld_local);
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
        BOOST_TEST(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        shared_library sl(shared_library_path, load_mode::rtld_now);
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
        BOOST_TEST(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        fs_copy_guard guard(shared_library_path);

        boost::filesystem::path platform_independent_path = guard.actual_path_;
        platform_independent_path.replace_extension();
        if (platform_independent_path.filename().wstring().find(L"lib") == 0) {
            platform_independent_path
                = platform_independent_path.parent_path() / platform_independent_path.filename().wstring().substr(3);
        }
        std::cerr << "platform_independent_path: " << platform_independent_path << '\n';

        shared_library sl(platform_independent_path, load_mode::append_decorations);
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
        BOOST_TEST(lib_path_equal(sl.location(), shared_library_path));

        sl.unload();
        BOOST_TEST(!sl.is_loaded());
        BOOST_TEST(!sl);
   }

   {
        shared_library sl(shared_library_path, load_mode::rtld_now | load_mode::rtld_global | load_mode::load_with_altered_search_path);
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
   }

   {
        boost::system::error_code ec;
        shared_library sl(shared_library_path, load_mode::rtld_lazy | load_mode::rtld_global, ec);
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
        BOOST_TEST(!ec);
        BOOST_TEST(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        shared_library sl;
        sl.load(shared_library_path, load_mode::rtld_lazy | load_mode::rtld_global);
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
        BOOST_TEST(lib_path_equal(sl.location(), shared_library_path));
   }
   
   {  // self_load
        shared_library sl(program_location());
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);


        boost::system::error_code ec;
        shared_library sl2(program_location());
        BOOST_TEST(sl2.is_loaded());
        BOOST_TEST(sl2);
        BOOST_TEST(!ec);

        BOOST_TEST(sl == sl2);
        BOOST_TEST(!(sl < sl2 || sl2 <sl));
        BOOST_TEST(!(sl != sl2));

        sl.load(shared_library_path);
        BOOST_TEST(sl != sl2);
        BOOST_TEST(sl < sl2 || sl2 <sl);
        BOOST_TEST(!(sl == sl2));

        sl.unload();
        BOOST_TEST(!sl);
        BOOST_TEST(sl != sl2);
        BOOST_TEST(sl < sl2 || sl2 <sl);
        BOOST_TEST(!(sl == sl2));
    
        sl2.unload();
        BOOST_TEST(!sl2);
        BOOST_TEST(sl == sl2);
        BOOST_TEST(!(sl < sl2 || sl2 <sl));
        BOOST_TEST(!(sl != sl2));
   }

   {
        shared_library sl;
        boost::system::error_code ec;
        sl.load(shared_library_path, load_mode::rtld_lazy | load_mode::rtld_global, ec);
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
        BOOST_TEST(!ec);
        BOOST_TEST(lib_path_equal(sl.location(), shared_library_path));

        sl.load(program_location());
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);

        sl.load(program_location());
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
        BOOST_TEST(!ec);
   }
   
   {  // self_load
        shared_library sl;
        boost::system::error_code ec;
        sl.load(program_location());
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
        BOOST_TEST(!ec);
   }

   {  // unload
        shared_library sl(shared_library_path);
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);
        BOOST_TEST(lib_path_equal(sl.location(), shared_library_path));
        sl.unload();
        BOOST_TEST(!sl.is_loaded());
        BOOST_TEST(!sl);
   }


   {  // error_code load calls test
        boost::system::error_code ec;
        shared_library sl(shared_library_path / "dir_that_does_not_exist", ec);
        BOOST_TEST(ec);
        BOOST_TEST(!sl.is_loaded());
        BOOST_TEST(!sl);

        boost::filesystem::path bad_path(shared_library_path);
        bad_path += ".1.1.1.1.1.1";
        sl.load(bad_path, ec);
        BOOST_TEST(ec);
        BOOST_TEST(!sl.is_loaded());
        BOOST_TEST(!sl);

        sl.load(shared_library_path, ec);
        BOOST_TEST(!ec);
        BOOST_TEST(sl.is_loaded());
        BOOST_TEST(sl);

        shared_library sl2(bad_path, ec);
        BOOST_TEST(ec);
        BOOST_TEST(!sl2.is_loaded());
        BOOST_TEST(!sl2);

        shared_library sl3(shared_library_path, ec);
        BOOST_TEST(!ec);
        BOOST_TEST(sl3.is_loaded());
        BOOST_TEST(sl3);

        sl.load("", ec);
        BOOST_TEST(ec);
        BOOST_TEST(!sl.is_loaded());
        BOOST_TEST(!sl);
   }


    shared_library_path = do_find_correct_libs_path(argc, argv, "library1");
    fs_copy_guard guard(shared_library_path);
    shared_library starts_with_lib(
        boost::filesystem::path(guard.actual_path_).replace_extension(),
        load_mode::append_decorations
    );

    starts_with_lib.load(
        boost::filesystem::path(guard.actual_path_).replace_extension(),
        load_mode::append_decorations
    );

   return boost::report_errors();
}
