// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/plugin/shared_library.hpp>
#include <boost/test/minimal.hpp>
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
// Unit Tests

int test_main(int argc, char* argv[])
{
   using namespace boost::plugin;

   BOOST_CHECK(argc >= 2);
   boost::filesystem::path shared_library_path = get_shared_lib(argv[1], L"test_library");
   std::cout << "Library: " << shared_library_path;

   {
      shared_library sl(shared_library_path);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(sl.path() == shared_library_path);

      shared_library sl2;
      BOOST_CHECK(!sl2.is_loaded());
      BOOST_CHECK(sl2.path().empty());

      swap(sl, sl2);
      BOOST_CHECK(!sl.is_loaded());
      BOOST_CHECK(sl2.is_loaded());
   }

   {
      boost::system::error_code ec;
      shared_library sl(shared_library_path, ec);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(sl.path() == shared_library_path);
   }

   {
      shared_library sl;
      sl.load(shared_library_path);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(sl.path() == shared_library_path);
   }

   {
      shared_library sl;
      boost::system::error_code ec;
      sl.load(shared_library_path, ec);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(sl.path() == shared_library_path);
   }

#if defined( BOOST_WINDOWS_API )
#if !defined(__MINGW32__)   
   {
      shared_library sl(shared_library_path, load_with_altered_search_path );
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(sl.path() == shared_library_path);
   }

   {
      boost::system::error_code ec;
      shared_library sl(shared_library_path, load_with_altered_search_path, ec);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(sl.path() == shared_library_path);
   }

   {
      shared_library sl;
      sl.load(shared_library_path, load_with_altered_search_path);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(sl.path() == shared_library_path);
   }

   {
      shared_library sl;
      boost::system::error_code ec;
      sl.load(shared_library_path, load_with_altered_search_path, ec);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(sl.path() == shared_library_path);
   }
#endif   
#elif defined( BOOST_POSIX_API )

   {
      shared_library sl(shared_library_path, rtld_lazy | rtld_global);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(sl.path() == shared_library_path);
   }

   {
      boost::system::error_code ec;
      shared_library sl(shared_library_path, rtld_lazy | rtld_global, ec);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(sl.path() == shared_library_path);
   }

   {
      shared_library sl;
      sl.load(shared_library_path, rtld_lazy | rtld_global);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(sl.path() == shared_library_path);
   }

   {
      shared_library sl;
      boost::system::error_code ec;
      sl.load(shared_library_path, rtld_lazy | rtld_global, ec);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(sl.path() == shared_library_path);
   }
#endif

   {  // unload
      shared_library sl(shared_library_path);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(sl.path() == shared_library_path);
      sl.unload();
      BOOST_CHECK(!sl.is_loaded());
      BOOST_CHECK(sl.path().empty());
   }


   {  // error_code load calls test
      boost::system::error_code ec;
      shared_library sl(shared_library_path / "dir_that_does_not_exist", ec);
      BOOST_CHECK(ec);
      BOOST_CHECK(!sl.is_loaded());
      BOOST_CHECK(sl.path().empty());

      boost::filesystem::path bad_path(shared_library_path);
      bad_path += ".1.1.1.1.1.1";
      sl.load(bad_path, ec);
      BOOST_CHECK(ec);
      BOOST_CHECK(!sl.is_loaded());
      BOOST_CHECK(sl.path().empty());

      sl.load(shared_library_path, ec);
      BOOST_CHECK(!ec);
      BOOST_CHECK(sl.is_loaded());
      BOOST_CHECK(sl.path() == shared_library_path);

      shared_library sl2(bad_path, ec);
      BOOST_CHECK(ec);
      BOOST_CHECK(!sl2.is_loaded());
      BOOST_CHECK(sl2.path().empty());

      shared_library sl3(shared_library_path, ec);
      BOOST_CHECK(!ec);
      BOOST_CHECK(sl3.is_loaded());
      BOOST_CHECK(sl3.path() == shared_library_path);
   }

   return 0;
}
