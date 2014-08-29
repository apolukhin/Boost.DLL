// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/dll/shared_library.hpp>
#include <boost/test/minimal.hpp>

#include "../example/shared_lib_path.hpp"
// Unit Tests

inline bool lib_path_equal(const boost::filesystem::path& lhs, const boost::filesystem::path& rhs) {
    std::size_t size = lhs.native().size();
    if (size > rhs.native().size()) {
        size = rhs.native().size();
    }

    return !rhs.native().compare(0, size, lhs.native(), 0, size);
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
      BOOST_CHECK(lib_path_equal(sl.path(), shared_library_path));

      shared_library sl2;
      BOOST_CHECK(!sl2.is_loaded());

      swap(sl, sl2);
      BOOST_CHECK(!sl.is_loaded());
      BOOST_CHECK(sl2.is_loaded());
   }

   {
      boost::system::error_code ec;
      shared_library sl(shared_library_path, ec);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(lib_path_equal(sl.path(), shared_library_path));
   }

   {
      shared_library sl;
      sl.load(shared_library_path);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(lib_path_equal(sl.path(), shared_library_path));
   }

   {
      shared_library sl;
      boost::system::error_code ec;
      sl.load(shared_library_path, ec);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(lib_path_equal(sl.path(), shared_library_path));
   }

   {
      shared_library sl(shared_library_path, load_mode::load_with_altered_search_path );
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(lib_path_equal(sl.path(), shared_library_path));
   }

   {
      boost::system::error_code ec;
      shared_library sl(shared_library_path, load_mode::load_with_altered_search_path, ec);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(lib_path_equal(sl.path(), shared_library_path));
   }

   {
      shared_library sl;
      sl.load(shared_library_path, load_mode::load_with_altered_search_path);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(lib_path_equal(sl.path(), shared_library_path));
   }

   {
      shared_library sl;
      boost::system::error_code ec;
      sl.load(shared_library_path, load_mode::load_with_altered_search_path, ec);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(lib_path_equal(sl.path(), shared_library_path));
   }

   {
      shared_library sl(shared_library_path, load_mode::rtld_lazy | load_mode::rtld_global);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(lib_path_equal(sl.path(), shared_library_path));
   }

   {
      shared_library sl(shared_library_path, load_mode::rtld_local);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(lib_path_equal(sl.path(), shared_library_path));
   }

   {
      shared_library sl(shared_library_path, load_mode::rtld_now);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(lib_path_equal(sl.path(), shared_library_path));
   }

   {
      shared_library sl(shared_library_path, load_mode::rtld_now | load_mode::rtld_global | load_mode::load_library_as_datafile);
      BOOST_CHECK(sl.is_loaded());
      // The GetModuleFileName function does not retrieve the path for modules that were loaded using the LOAD_LIBRARY_AS_DATAFILE flag.
      // BOOST_CHECK(lib_path_equal(sl.path(), shared_library_path));
   }

   {
      boost::system::error_code ec;
      shared_library sl(shared_library_path, load_mode::rtld_lazy | load_mode::rtld_global, ec);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(!ec);
      BOOST_CHECK(lib_path_equal(sl.path(), shared_library_path));
   }

   {
      shared_library sl;
      sl.load(shared_library_path, load_mode::rtld_lazy | load_mode::rtld_global);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(lib_path_equal(sl.path(), shared_library_path));
   }
   
   {  // self_load
      shared_library sl;
      sl.load_self();
      BOOST_CHECK(sl.is_loaded());


      boost::system::error_code ec;
      shared_library sl2;
      sl2.load_self(ec);
      BOOST_CHECK(sl2.is_loaded());
      BOOST_CHECK(!ec);

      BOOST_CHECK(sl == sl2);
      BOOST_CHECK(!(sl < sl2 || sl2 <sl));
      BOOST_CHECK(!(sl != sl2));

      sl.load(shared_library_path);
      BOOST_CHECK(sl != sl2);
      BOOST_CHECK(sl < sl2 || sl2 <sl);
      BOOST_CHECK(!(sl == sl2));

      sl.unload();
      BOOST_CHECK(sl != sl2);
      BOOST_CHECK(sl < sl2 || sl2 <sl);
      BOOST_CHECK(!(sl == sl2));
    
      sl2.unload();
      BOOST_CHECK(sl == sl2);
      BOOST_CHECK(!(sl < sl2 || sl2 <sl));
      BOOST_CHECK(!(sl != sl2));
   }

   {
      shared_library sl;
      boost::system::error_code ec;
      sl.load(shared_library_path, load_mode::rtld_lazy | load_mode::rtld_global, ec);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(!ec);
      BOOST_CHECK(lib_path_equal(sl.path(), shared_library_path));

      sl.load_self();
      BOOST_CHECK(sl.is_loaded());

      sl.load_self(ec);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(!ec);
   }
   
   {  // self_load
      shared_library sl;
      boost::system::error_code ec;
      sl.load_self(ec);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(!ec);
   }

   {  // unload
      shared_library sl(shared_library_path);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(lib_path_equal(sl.path(), shared_library_path));
      sl.unload();
      BOOST_CHECK(!sl.is_loaded());
   }


   {  // error_code load calls test
      boost::system::error_code ec;
      shared_library sl(shared_library_path / "dir_that_does_not_exist", ec);
      BOOST_CHECK(ec);
      BOOST_CHECK(!sl.is_loaded());

      boost::filesystem::path bad_path(shared_library_path);
      bad_path += ".1.1.1.1.1.1";
      sl.load(bad_path, ec);
      BOOST_CHECK(ec);
      BOOST_CHECK(!sl.is_loaded());

      sl.load(shared_library_path, ec);
      BOOST_CHECK(!ec);
      BOOST_CHECK(sl.is_loaded());

      shared_library sl2(bad_path, ec);
      BOOST_CHECK(ec);
      BOOST_CHECK(!sl2.is_loaded());

      shared_library sl3(shared_library_path, ec);
      BOOST_CHECK(!ec);
      BOOST_CHECK(sl3.is_loaded());

      sl.load("", ec);
      BOOST_CHECK(ec);
      BOOST_CHECK(!sl.is_loaded());
   }

   return 0;
}
