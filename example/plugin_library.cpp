// Copyright 2011-2013 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <iostream>

//[plugcpp

#include <boost/config.hpp>
#define LIBRARY_API BOOST_SYMBOL_EXPORT
   
#include "plugin_api.hpp"

extern "C" LIBRARY_API my_plugin_api* create_my_plugin(void);
extern "C" LIBRARY_API void delete_my_plugin(my_plugin_api* myplugin);

class my_plugin_sum : public my_plugin_api
{
public:

   float version() 
   { 
      return 1.0;  
   };

   float calculate(float x, float y) 
   { 
      return x+y; 
   };
   
   ~my_plugin_sum()
   {
      std::cout << ";o)" << std::endl;
   }
};

my_plugin_api* create_my_plugin(void)
{
   my_plugin_api *myplugin = new my_plugin_sum();
   return myplugin;
}

void delete_my_plugin(my_plugin_api* myplugin)
{
   delete myplugin;
}
//]


// platform dependent initialization sample
//

#if defined( BOOST_WINDOWS_API )

// return TRUE on success and FALSE if an error occurs. returning
// FALSE will cause the library to be unloaded.
BOOL WINAPI DllMain
(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpReserved
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // add initialization code...
        break;
    case DLL_PROCESS_DETACH:
        // add clean-up code...
        break;
    }

    return (TRUE);
}

#elif defined( BOOST_POSIX_API )

// GCC
void __attribute__ ((constructor)) my_load(void);
void __attribute__ ((destructor)) my_unload(void);

// called when the library is loaded and before dlopen() returns
void my_load(void)
{
   // Add initialization code
}

// called when the library is unloaded and before dlclose() returns
void my_unload(void)
{
   // Add clean-up code
}

#endif



