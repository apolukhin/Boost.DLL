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

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif


/// \file boost/plugin/alias.hpp
/// \brief Includes alias methods and macro. You can include this header or
/// boost/plugin/shared_library.hpp to reduce dependencies
/// in case you do not use the refcountable functions.

namespace boost { namespace plugin {

#if BOOST_OS_WINDOWS

#define BOOST_PLUGIN_SELECTANY __declspec(selectany)
#define BOOST_PLUGIN_SECTION(SectionName) /*TODO:*/

#else

#define BOOST_PLUGIN_SELECTANY __attribute__((weak))
#define BOOST_PLUGIN_SECTION(SectionName) __attribute__ ((section (SectionName)))

#endif

/*!
* \brief Makes an alias name for exported function or variable.
*
* This macro is usefull in cases of long mangled names.
*
* Can be used in any namespace, including global. FunctionOrVar must be fully qualified,
* so that address of it could be taken. Multiple different alises for a single variable/function
* are allowed.
*
* Make sure that AliasNames are unique per library/executable. Functions or variables
* in global namespace must not have names same as AliasNames.
*
* Same AliasName in different translation units must point to the same FunctionOrVar.
*
* \b Example:
* \code
* namespace foo {
*   void bar(std::string&);
*
*   BOOST_PLUGIN_ALIAS(foo::bar, foo_bar) // foo::bar function now could be dynamicaly loaded using "foo_bar" name
* }
*
* BOOST_PLUGIN_ALIAS(foo::bar, foo_bar_another_alias_name)
* \endcode
*/
#define BOOST_PLUGIN_ALIAS(FunctionOrVar, AliasName)                            \
    extern "C" BOOST_SYMBOL_EXPORT const void *AliasName;                       \
     BOOST_PLUGIN_SECTION("boost_aliases") BOOST_PLUGIN_SELECTANY               \
        const void *AliasName = reinterpret_cast<const void*>(&FunctionOrVar);  \
    /**/



/*!
* Returns a symbol (function or variable) from a shared library by alias name of the symbol.
*
* \b Example:
* \code
* shared_library lib("test_lib.so");
* int& i = alias<int>(lib, "integer_alias_name");
* \endcode
*
* \tparam T Type of the symbol that we are going to import. Must be explicitly specified..
*
* \param lib Library to load symbol from.
*
* \param sb Null-terminated alias symbol name. Can handle std::string, char*, const char*.
*
* \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
*/
template <typename T>
inline T& alias(const shared_library& lib, const boost::string_ref &symbol) {
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
    return *lib.get<T*>(symbol);
}

}} // boost::plugin

#endif // BOOST_PLUGIN_ALIAS_HPP

