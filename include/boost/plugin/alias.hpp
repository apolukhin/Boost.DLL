// alias.hpp --------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2014 Renato Tegon Forti, Antony Polukhin.

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 06-08-2014 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_PLUGIN_ALIAS_HPP
#define BOOST_PLUGIN_ALIAS_HPP

#include <boost/config.hpp>
#include <boost/plugin/shared_library.hpp>

namespace boost { namespace plugin {


/*!
* \brief makes an alias for a function
*/
#define BOOST_PLUGIN_ALIAS(Function, AliasName)             \
    extern "C" BOOST_SYMBOL_EXPORT void *AliasName;         \
    void *AliasName = reinterpret_cast<void*>(&Function);   \
    /**/

template <typename Result>
inline Result& alias(const shared_library& lib, const symbol_type &symbol) {
    // Alias - is just a variable that pointers to original data
    //
    // A few attempts were made to avoid additional indirection:
    // 1) 
    //          // Does not work on Windows, work on Linux
    //          extern "C" BOOST_SYMBOL_EXPORT void AliasName() {
    //              reinterpret_cast<void (*)()>(Function)();
    //          }
    //
    // 2) 
    //          // Does not work on Linux (changes permissions of .text section and produces incorrect DSO)
    //          extern "C" BOOST_SYMBOL_EXPORT void* __attribute__ ((section(".text#"))) 
    //                  func_ptr = *reinterpret_cast<std::ptrdiff_t**>(&foo::bar);
    //
    // 3)       // requires mangled name of `Function` 
    //          //  AliasName() __attribute__ ((weak, alias ("Function")))  
    //
    //          // hard to use
    //          `#pragma comment(linker, "/alternatename:_pWeakValue=_pDefaultWeakValue")`
    return *lib.get<Result*>(symbol);
}

}} // boost::plugin

#endif // BOOST_PLUGIN_ALIAS_HPP

