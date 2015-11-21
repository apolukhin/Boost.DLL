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

template <class Func>
struct helper {
private:
    dll::shared_library lib_;
    Func*               f_;

public:
    helper(const char* library, const char* function)
        : lib_(library)
        , f_(&lib_.get<GetStdHandle_t>(function))
    {}

    inline operator Func* () const {
        return f_;
    }
};

int main() {
    typedef HANDLE(__stdcall GetStdHandle_t)(DWORD );       // function signature with calling convention
    typedef HANDLE(GetStdHandle_no_call_conv_t)(DWORD );    // function signature without calling convention
    boost::function<GetStdHandle_no_call_conv_t> plugin = helper<GetStdHandle_t>("Kernel32.dll", "GetStdHandle");

    std::cout << "1. GetStdHandle() returned " << plugin(STD_OUTPUT_HANDLE) << std::endl;
    std::cout << "2. GetStdHandle() returned " << helper<GetStdHandle_t>("Kernel32.dll", "GetStdHandle")(STD_OUTPUT_HANDLE) << std::endl;
    return 0;
}

//]

#else // BOOST_WINDOWS

int main()
{
    return 0;
}

#endif // BOOST_WINDOWS
