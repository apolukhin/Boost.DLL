// Copyright 2011-2013 Renato Tegon Forti.
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <iostream>
#include <boost/make_shared.hpp>

//[plugcpp_my_plugin_sum
   
#include "../tutorial_common/plugin_api.hpp"
#include <boost/dll.hpp>

namespace my_namespace {

class my_plugin_sum : public my_plugin_api {
public:
    my_plugin_sum() {
        std::cout << "Constructing my_plugin_sum" << std::endl;
    }

    float version() const {
        return 1.0;
    }

    std::string name() const {
        return "sum";
    }

    float calculate(float x, float y) {
        return x + y;
    }
   
    ~my_plugin_sum() {
        std::cout << "Destructing my_plugin_sum ;o)" << std::endl;
    }
};

my_plugin_sum plugin;

} // namespace my_namespace

BOOST_DLL_ALIAS(my_namespace::plugin, plugin)
//]

// platform dependent initialization sample
//
/*
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
*/


