// Copyright 2015 Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/predef/os.h>
#if BOOST_OS_WINDOWS

//[callplugcpp_tutorial9
#include <boost/dll/import.hpp> // for import
#include <iostream>
#include <windows.h>

namespace dll = boost::dll;

int main()
{
    typedef HANDLE(__stdcall GetStdHandle_t)(DWORD );       // function signature with calling convention
    typedef HANDLE(GetStdHandle_no_call_conv_t)(DWORD );    // function signature without calling convention

    boost::function<GetStdHandle_no_call_conv_t> plugin
        = dll::import<GetStdHandle_t>(              // we import using C name, not alias! Using `import<>`
            "Kernel32.dll",                         // Windows dll
            "GetStdHandle"                          // function name
        );

    std::cout << "GetStdHandle(STD_OUTPUT_HANDLE) returned " << plugin(STD_OUTPUT_HANDLE) << std::endl;
    return 0;
}

//]

#else // BOOST_WINDOWS

int main()
{
    return 0;
}

#endif // BOOST_WINDOWS
