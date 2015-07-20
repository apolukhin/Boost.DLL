// Copyright 2011-2012 Renato Tegon Forti
// Copyright 2015 Antony Polukhin
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/dll.hpp>
#include <boost/test/minimal.hpp>

#include "../example/shared_lib_path.hpp"
// Unit Tests

inline bool lib_path_equal(const boost::filesystem::path& lhs, const boost::filesystem::path& rhs) {
    // ./b2 may create symlinks. We assume that there are no two files with same names,
    // si if `lhs.filename() == rhs.filename()` then paths are same.
    const bool res = (lhs.filename() == rhs.filename() && lhs.is_absolute() && rhs.is_absolute());
    if (!res) {
        std::cerr << "lhs != rhs: " << lhs << " != " << rhs << '\n';
    }
    return res;
}

int test_main(int argc, char* argv[])
{
    using namespace boost::dll;

    BOOST_CHECK(argc >= 2);
    boost::filesystem::path shared_library_path = shared_lib_path(argv[1], L"test_library");
    std::cout << "Library: " << shared_library_path;

    {
        shared_library sl(shared_library_path);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(lib_path_equal(sl.location(), shared_library_path));

        shared_library sl2;
        BOOST_CHECK(!sl2.is_loaded());
        BOOST_CHECK(!sl2);

        swap(sl, sl2);
        BOOST_CHECK(!sl.is_loaded());
        BOOST_CHECK(!sl);
        BOOST_CHECK(sl2.is_loaded());
        BOOST_CHECK(sl2);
   }

   {
        boost::system::error_code ec;
        shared_library sl(shared_library_path, ec);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        BOOST_CHECK(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        shared_library sl;
        sl.load(shared_library_path);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        BOOST_CHECK(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        shared_library sl;
        boost::system::error_code ec;
        sl.load(shared_library_path, ec);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        BOOST_CHECK(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        shared_library sl(shared_library_path, load_mode::load_with_altered_search_path );
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        BOOST_CHECK(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        boost::system::error_code ec;
        shared_library sl(shared_library_path, load_mode::load_with_altered_search_path, ec);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        BOOST_CHECK(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        shared_library sl;
        sl.load(shared_library_path, load_mode::load_with_altered_search_path);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        BOOST_CHECK(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        shared_library sl;
        boost::system::error_code ec;
        sl.load(shared_library_path, load_mode::load_with_altered_search_path, ec);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        BOOST_CHECK(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        shared_library sl(shared_library_path, load_mode::rtld_lazy | load_mode::rtld_global);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        BOOST_CHECK(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        shared_library sl(shared_library_path, load_mode::rtld_local);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        BOOST_CHECK(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        shared_library sl(shared_library_path, load_mode::rtld_now);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        BOOST_CHECK(lib_path_equal(sl.location(), shared_library_path));
   }


   {
        boost::filesystem::path platform_independent_path = shared_library_path;
        platform_independent_path.replace_extension();
        if (platform_independent_path.filename().wstring().find(L"lib") == 0) {
          platform_independent_path
                = platform_independent_path.parent_path() / platform_independent_path.filename().wstring().substr(3);
        }

        shared_library sl(platform_independent_path, load_mode::append_decorations);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        BOOST_CHECK(lib_path_equal(sl.location(), shared_library_path));


        boost::filesystem::copy_file(shared_library_path, boost::filesystem::current_path() / shared_library_path.filename());
        sl.load("./" / platform_independent_path.filename(), load_mode::append_decorations);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);

        boost::system::error_code ec;
        sl.load("./" / platform_independent_path.filename(), ec);
        BOOST_CHECK(ec);
        BOOST_CHECK(!sl.is_loaded());
        BOOST_CHECK(!sl);

        sl.load(platform_independent_path.filename(), ec);
        BOOST_CHECK(ec);
        BOOST_CHECK(!sl.is_loaded());
        BOOST_CHECK(!sl);

        sl.unload();
        BOOST_CHECK(!sl.is_loaded());
        BOOST_CHECK(!sl);

        boost::filesystem::remove(shared_library_path.filename());
   }

   {
        shared_library sl(shared_library_path, load_mode::rtld_now | load_mode::rtld_global | load_mode::load_library_as_datafile);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        // The GetModuleFileName function does not retrieve the path for modules that were loaded using the LOAD_LIBRARY_AS_DATAFILE flag.
        // BOOST_CHECK(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        boost::system::error_code ec;
        shared_library sl(shared_library_path, load_mode::rtld_lazy | load_mode::rtld_global, ec);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        BOOST_CHECK(!ec);
        BOOST_CHECK(lib_path_equal(sl.location(), shared_library_path));
   }

   {
        shared_library sl;
        sl.load(shared_library_path, load_mode::rtld_lazy | load_mode::rtld_global);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        BOOST_CHECK(lib_path_equal(sl.location(), shared_library_path));
   }
   
   {  // self_load
        shared_library sl(program_location());
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);


        boost::system::error_code ec;
        shared_library sl2(program_location());
        BOOST_CHECK(sl2.is_loaded());
        BOOST_CHECK(sl2);
        BOOST_CHECK(!ec);

        BOOST_CHECK(sl == sl2);
        BOOST_CHECK(!(sl < sl2 || sl2 <sl));
        BOOST_CHECK(!(sl != sl2));

        sl.load(shared_library_path);
        BOOST_CHECK(sl != sl2);
        BOOST_CHECK(sl < sl2 || sl2 <sl);
        BOOST_CHECK(!(sl == sl2));

        sl.unload();
        BOOST_CHECK(!sl);
        BOOST_CHECK(sl != sl2);
        BOOST_CHECK(sl < sl2 || sl2 <sl);
        BOOST_CHECK(!(sl == sl2));
    
        sl2.unload();
        BOOST_CHECK(!sl2);
        BOOST_CHECK(sl == sl2);
        BOOST_CHECK(!(sl < sl2 || sl2 <sl));
        BOOST_CHECK(!(sl != sl2));
   }

   {
        shared_library sl;
        boost::system::error_code ec;
        sl.load(shared_library_path, load_mode::rtld_lazy | load_mode::rtld_global, ec);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        BOOST_CHECK(!ec);
        BOOST_CHECK(lib_path_equal(sl.location(), shared_library_path));

        sl.load(program_location());
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);

        sl.load(program_location());
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        BOOST_CHECK(!ec);
   }
   
   {  // self_load
        shared_library sl;
        boost::system::error_code ec;
        sl.load(program_location());
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        BOOST_CHECK(!ec);
   }

   {  // unload
        shared_library sl(shared_library_path);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);
        BOOST_CHECK(lib_path_equal(sl.location(), shared_library_path));
        sl.unload();
        BOOST_CHECK(!sl.is_loaded());
        BOOST_CHECK(!sl);
   }


   {  // error_code load calls test
        boost::system::error_code ec;
        shared_library sl(shared_library_path / "dir_that_does_not_exist", ec);
        BOOST_CHECK(ec);
        BOOST_CHECK(!sl.is_loaded());
        BOOST_CHECK(!sl);

        boost::filesystem::path bad_path(shared_library_path);
        bad_path += ".1.1.1.1.1.1";
        sl.load(bad_path, ec);
        BOOST_CHECK(ec);
        BOOST_CHECK(!sl.is_loaded());
        BOOST_CHECK(!sl);

        sl.load(shared_library_path, ec);
        BOOST_CHECK(!ec);
        BOOST_CHECK(sl.is_loaded());
        BOOST_CHECK(sl);

        shared_library sl2(bad_path, ec);
        BOOST_CHECK(ec);
        BOOST_CHECK(!sl2.is_loaded());
        BOOST_CHECK(!sl2);

        shared_library sl3(shared_library_path, ec);
        BOOST_CHECK(!ec);
        BOOST_CHECK(sl3.is_loaded());
        BOOST_CHECK(sl3);

        sl.load("", ec);
        BOOST_CHECK(ec);
        BOOST_CHECK(!sl.is_loaded());
        BOOST_CHECK(!sl);
   }

    shared_library starts_with_lib(
        boost::filesystem::path(argv[1]) / "library1" BOOST_B2_LIBRARY_DECORATIONS,
        load_mode::append_decorations
    );

    starts_with_lib.load(
        boost::filesystem::path(argv[1]) / "library1" BOOST_B2_LIBRARY_DECORATIONS,
        load_mode::append_decorations
    );

   return 0;
}
