// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_DLL_IMPORT_HPP
#define BOOST_DLL_IMPORT_HPP

#include <boost/config.hpp>
#include <boost/dll/import_function.hpp>
#include <boost/dll/import_variable.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_object.hpp>
#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

/// \file boost/dll/import_function.hpp
/// \brief Contains all the boost::dll::import_function* reference counting
/// functions that hold a shared pointer to the instance of
/// boost::dll::shared_library.

namespace boost { namespace dll {

/*!
* Returns boost::function<T> or boost::shared_ptr<T> that holds an imported function or variable
* from the loaded library and refcounts usage
* of the loaded shared library, so that it won't get unload until all copies of return value
* are not destroyed.
*
* This call will succeed if call to \forcedlink{shared_library}`::search_symbol(boost::string_ref )`
* function with the same symbol name returned `true`.
*
* For importing symbols by \b alias names use \forcedlink{import_alias} method.
*
* \b Examples:
* \code
* boost::function<int(int)> f = import<int(int)>(
*           boost::make_shared<shared_library>("test_lib.so"),
*           "integer_func_name"
*       );
* \endcode
*
* \code
* boost::function<int(int)> f = import<int(int)>("test_lib.so", "integer_func_name");
* \endcode
*
* \code
* boost::shared_ptr<int> i = import<int>("test_lib.so", "integer_name");
* \endcode
*
* \tparam T Type of the symbol that we are going to import. Must be explicitly specified.
*
* \param lib Path or shared pointer to library to load function from.
* \param func_name Null-terminated C or C++ mangled name of the function to import. Can handle std::string, char*, const char*.
* \param mode An mode that will be used on library load.
*
* \return boost::function<T> if T is a function type, or boost::shared_ptr<T> if T is an object type.
*
* \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
*       Overload that accepts path also throws std::bad_alloc in case of insufficient memory.
*/
template <class T>
typename boost::disable_if<boost::is_object<T>, boost::function<T> >::type 
import(const boost::filesystem::path& lib, boost::string_ref func_name,
    load_mode::type mode = load_mode::default_mode)
{
    return boost::dll::explicit_api::import_function<T>(lib, func_name, mode);
}

//! \overload boost::dll::import(const boost::shared_ptr<shared_library>& lib, boost::string_ref func_name, load_mode::type mode)
template <class T>
typename boost::disable_if<boost::is_object<T>, boost::function<T> >::type
import(const boost::shared_ptr<shared_library>& lib, boost::string_ref func_name) {
    return boost::dll::explicit_api::import_function<T>(lib, func_name);
}

//! \overload boost::dll::import(const boost::shared_ptr<shared_library>& lib, boost::string_ref func_name, load_mode::type mode)
template <class T>
typename boost::enable_if<boost::is_object<T>, boost::shared_ptr<T> >::type 
import(const boost::filesystem::path& lib, boost::string_ref variable_name,
    load_mode::type mode = load_mode::default_mode)
{
    return boost::dll::explicit_api::import_variable<T>(lib, variable_name, mode);
}

//! \overload boost::dll::import(const boost::shared_ptr<shared_library>& lib, boost::string_ref func_name, load_mode::type mode)
template <class T>
typename boost::enable_if<boost::is_object<T>, boost::shared_ptr<T> >::type
import(const boost::shared_ptr<shared_library>& lib, boost::string_ref variable_name) {
    return boost::dll::explicit_api::import_variable<T>(lib, variable_name);
}



/*!
* Returns boost::function<T> or boost::shared_ptr<T> that holds an imported function or variable
* from the loaded library and refcounts usage
* of the loaded shared library, so that it won't get unload until all copies of return value
* are not destroyed.
*
* This call will succeed if call to \forcedlink{shared_library}`::search_symbol(boost::string_ref )`
* function with the same symbol name returned `true`.
*
* For importing symbols by \b non \b alias names use \forcedlink{import} method.
*
* \b Examples:
* \code
* boost::function<int(int)> f = import_alias<int(int)>(
*           boost::make_shared<shared_library>("test_lib.so"),
*           "integer_func_alias_name"
*       );
* \endcode
*
* \code
* boost::function<int(int)> f = import_alias<int(int)>("test_lib.so", "integer_func_alias_name");
* \endcode
*
* \code
* boost::shared_ptr<int> i = import_alias<int>("test_lib.so", "integer_alias_name");
* \endcode
*
* \tparam T Type of the symbol that we are going to import. Must be explicitly specified.
*
* \param lib Path or shared pointer to library to load function from.
* \param func_name Null-terminated C or C++ mangled name of the function to import. Can handle std::string, char*, const char*.
* \param variable_name Null-terminated C or C++ mangled name of the variable to import. 
* Can handle std::string, char*, const char*.
*
* \param mode An mode that will be used on library load.
*
* \return boost::function<T> if T is a function type, or boost::shared_ptr<T> if T is an object type.
*
* \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
*       Overload that accepts path also throws std::bad_alloc in case of insufficient memory.
*/
template <class T>
typename boost::disable_if<boost::is_object<T>, boost::function<T> >::type 
import_alias(const boost::filesystem::path& lib, boost::string_ref func_name,
    load_mode::type mode = load_mode::default_mode)
{
    return boost::dll::explicit_api::import_function_alias<T>(lib, func_name, mode);
}

//! \overload boost::dll::import_alias(const boost::shared_ptr<shared_library>& lib, boost::string_ref func_name, load_mode::type mode)
template <class T>
typename boost::disable_if<boost::is_object<T>, boost::function<T> >::type
import_alias(const boost::shared_ptr<shared_library>& lib, boost::string_ref func_name) {
    return boost::dll::explicit_api::import_function_alias<T>(lib, func_name);
}

//! \overload boost::dll::import_alias(const boost::shared_ptr<shared_library>& lib, boost::string_ref func_name, load_mode::type mode)
template <class T>
typename boost::enable_if<boost::is_object<T>, boost::shared_ptr<T> >::type 
import_alias(const boost::filesystem::path& lib, boost::string_ref variable_name,
    load_mode::type mode = load_mode::default_mode)
{
    return boost::dll::explicit_api::import_variable_alias<T>(lib, variable_name, mode);
}

//! \overload boost::dll::import_alias(const boost::shared_ptr<shared_library>& lib, boost::string_ref func_name, load_mode::type mode)
template <class T>
typename boost::enable_if<boost::is_object<T>, boost::shared_ptr<T> >::type
import_alias(const boost::shared_ptr<shared_library>& lib, boost::string_ref variable_name) {
    return boost::dll::explicit_api::import_variable_alias<T>(lib, variable_name);
}


}} // boost::dll

#endif // BOOST_DLL_IMPORT_HPP

