// Copyright Antony Polukhin, 2015-2024.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/predef/os.h>
#if BOOST_OS_WINDOWS

//[callplugcpp_tutorial9
#include <boost/dll/import.hpp>         // for dll::import
#include <boost/dll/shared_library.hpp> // for dll::shared_library
#include <functional>
#include <iostream>
#include <windows.h>

namespace dll = boost::dll;

int main() {
    typedef HANDLE(__stdcall GetStdHandle_t)(DWORD );       // function signature with calling convention

    // OPTION #0, requires C++11 compatible compiler that understands GetStdHandle_t signature.
    auto get_std_handle = dll::import_symbol<GetStdHandle_t>(
        "Kernel32.dll",
        "GetStdHandle",
        boost::dll::load_mode::search_system_folders
    );
    std::cout << "0.0 GetStdHandle() returned " << get_std_handle(STD_OUTPUT_HANDLE) << std::endl;

    // You may put the `get_std_handle` into std::function<>. But std::function<Signature> may not compile with
    // Signature template parameter that contains calling conventions, so you'll have to remove the calling convention.
    std::function<HANDLE(DWORD)> get_std_handle2 = get_std_handle;
    std::cout << "0.1 GetStdHandle() returned " << get_std_handle2(STD_OUTPUT_HANDLE) << std::endl;

    // OPTION #1, hand write the import.
    dll::shared_library lib("Kernel32.dll", dll::load_mode::search_system_folders);
    GetStdHandle_t& func = lib.get<GetStdHandle_t>("GetStdHandle");

    // Here `func` does not keep a reference to `lib`, you'll have to deal with that on your own.
    std::cout << "1.0 GetStdHandle() returned " << func(STD_OUTPUT_HANDLE) << std::endl;

    return 0;
}

//]

#else // BOOST_WINDOWS

#include <boost/dll/shared_library.hpp> // for dll::shared_library

int main() {
    return 0;
}

#endif // BOOST_WINDOWS
