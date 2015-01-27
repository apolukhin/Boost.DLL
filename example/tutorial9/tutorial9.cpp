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

namespace dll = boost::dll;

int main()
{
    typedef unsigned long (timeGetTime_fn)();       // function signature
    boost::function<timeGetTime_fn> plugin 
        = dll::import<timeGetTime_fn>(              // we import using C name, not alias! Using `import<>`
            "winmm.dll",                            // Windows dll
            "timeGetTime"                           // function name
        );

    std::cout << "timeGetTime() returned " << plugin() << std::endl;
    return 0;
}

//]

#else // BOOST_WINDOWS

int main()
{
    return 0;
}

#endif // BOOST_WINDOWS
