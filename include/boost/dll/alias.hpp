// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_DLL_ALIAS_HPP
#define BOOST_DLL_ALIAS_HPP

#include <boost/config.hpp>
#include <boost/static_assert.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/dll/detail/aggressive_ptr_cast.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif


/// \file boost/dll/alias.hpp
/// \brief Includes alias methods and macro. You can include this header or
/// boost/dll/shared_library.hpp to reduce dependencies
/// in case you do not use the refcountable functions.

namespace boost { namespace dll {

#if BOOST_OS_WINDOWS

#define BOOST_DLL_SELECTANY __declspec(selectany)

#define BOOST_DLL_SECTION(SectionName, Permissions)                                 \
    __pragma(section(#SectionName, Permissions)) __declspec(allocate(#SectionName)) \
    /**/

#else

/*!
* \brief Macro that allows linker to select any occurence of this symbol instead of
* failing with 'multiple definitions' error at linktime.
*/
#define BOOST_DLL_SELECTANY __attribute__((weak))

// TODO: improve section permissions using following info:
// http://stackoverflow.com/questions/6252812/what-does-the-aw-flag-in-the-section-attribute-mean

/*!
* \brief Macro that pust symbol to a specific section.
* \param SectionName Name of the section. Must be a valid C identifier without quotes not longer than 8 bytes.
* \param Permissions Can be "read" or "write" (without quotes!).
*/
#define BOOST_DLL_SECTION(SectionName, Permissions) __attribute__ ((section (#SectionName)))

#endif

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
* \param FunctionOrVar Function or variable for wich an alias must be made.
* \param AliasName Name of the alias. Must be a valid C identifier.
*
* \b Example:
* \code
* namespace foo {
*   void bar(std::string&);
*
*   BOOST_DLL_ALIAS(foo::bar, foo_bar)
* }
*
* BOOST_DLL_ALIAS(foo::bar, foo_bar_another_alias_name)
* \endcode
*
* Puts all the aliases into the "boostdll" read only section of the binary.
*/
#define BOOST_DLL_ALIAS(FunctionOrVar, AliasName)                       \
    BOOST_DLL_ALIAS_SECTIONED(FunctionOrVar, AliasName, boostdll)       \
    /**/


// Note: we can not use `aggressive_ptr_cast` here, because in that case GCC applies
// different permissions to the section and it causes Segmentation fault.
/*!
* \brief Same as BOOST_DLL_ALIAS but puts alias name into the user specified section.
*
* \param FunctionOrVar Function or variable for wich an alias must be made.
* \param AliasName Name of the alias. Must be a valid C identifier.
* \param SectionName Name of the section. Must be a valid C identifier without quotes not longer than 8 bytes.
*
* \b Example:
* \code
* namespace foo {
*   void bar(std::string&);
*
*   BOOST_DLL_ALIAS_SECTIONED(foo::bar, foo_bar, sect_1) // section "sect_1" now exports "foo_bar"
* }
* \endcode
*
*/
#define BOOST_DLL_ALIAS_SECTIONED(FunctionOrVar, AliasName, SectionName)                        \
    extern "C" BOOST_SYMBOL_EXPORT const void *AliasName;                                       \
    BOOST_DLL_SECTION(SectionName, read) BOOST_DLL_SELECTANY                                    \
    const void * AliasName = reinterpret_cast<const void*>(reinterpret_cast<intptr_t>(          \
        &FunctionOrVar                                                                          \
    ));                                                                                         \
    BOOST_STATIC_ASSERT_MSG(                                                                    \
        sizeof(#SectionName) < 10,                                                              \
        "Some platforms require section names to be at most 8 bytest"                           \
    );                                                                                          \
    /**/

}} // boost::dll

#endif // BOOST_DLL_ALIAS_HPP

