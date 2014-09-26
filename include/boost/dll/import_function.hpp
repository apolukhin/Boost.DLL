// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_DLL_IMPORT_FUNCTION_HPP
#define BOOST_DLL_IMPORT_FUNCTION_HPP

#include <boost/config.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>
#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

/// \file boost/dll/import_function.hpp
/// \brief Contains all the boost::dll::import_function* reference counting
/// functions that hold a shared pointer to the instance of
/// boost::dll::shared_library.

namespace boost { namespace dll {

namespace detail {

    template <class T>
    class refc_function {
        boost::shared_ptr<shared_library>   lib_;
        T*                                  func_ptr_;

    public:
        refc_function(const boost::shared_ptr<shared_library>& lib, T* func_ptr)
            : lib_(lib)
            , func_ptr_(func_ptr)
        {}

        operator T*() const BOOST_NOEXCEPT {
            return func_ptr_;
        }
    };
} // namespace detail


/*!
* Returns boost::function<T> that holds an imported function from the loaded library and refcounts usage
* of the loaded shared library, so that it won't get unload until all copies of return value
* are not destroyed.
*
* This call will succeed if call to \forcedlink{shared_library}`::search_symbol(boost::string_ref )`
* function with the same symbol name returned `true`.
*
* For importing function by its \b alias name use \forcedlink{import_function_alias} method.
*
* \b Examples:
* \code
* boost::function<int(int)> f = import_function<int(int)>(
*           boost::make_shared<shared_library>("test_lib.so"),
*           "integer_func_name"
* );
* \endcode
*
* \code
* boost::function<int(int)> f = import_function<int(int)>("test_lib.so", "integer_func_name");
* \endcode
*
* \tparam T Type of the symbol that we are going to import. Must be explicitly specified.
*
* \param lib Path or shared pointer to library to load function from.
* \param func_name Null-terminated name of the function to import. Can handle std::string, char*, const char*.
* \param mode An mode that will be used on library load.
*
* \return boost::function<T> that holds an imported function from the loaded library and refcounts usage
* of the loaded shared library.
*
* \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
*       Overload that accepts path also throws std::bad_alloc in case of insufficient memory.
*/
template <class T>
boost::function<T> import_function(const boost::filesystem::path& lib,boost::string_ref func_name,
    load_mode::type mode = load_mode::default_mode)
{
    return import_function<T>(
        boost::make_shared<shared_library>(lib),
        func_name
    );
}

//! \overload boost::dll::import_function(const boost::shared_ptr<shared_library>& lib, boost::string_ref func_name, load_mode::type mode)
template <class T>
boost::function<T> import_function(const boost::shared_ptr<shared_library>& lib, boost::string_ref func_name) {
    return boost::dll::detail::refc_function<T>(lib, &lib->get<T>(func_name));
}



/*!
* Imports function by it alias name and returns boost::function<T> that holds the function
* from the loaded library and refcounts usage of the loaded shared library.  Library won't
* get unload until all copies of returned value are not destroyed.
*
* This call will succeed if call to \forcedlink{shared_library}`::search_symbol(boost::string_ref )`
* function with the same symbol name returned `true`.
*
* For \b non \b alias names use \forcedlink{import_function} method.
*
* \b Examples:
* \code
* boost::function<int(int)> f = import_function_alias<int(int)>(
*           boost::make_shared<shared_library>("test_lib.so"),
*           "integer_func_alias_name"
* );
* \endcode
*
* \code
* boost::function<int(int)> f = import_function_alias<int(int)>("test_lib.so", "integer_func_alias_name");
* \endcode
*
* \tparam T Type of the symbol that we are going to import. Must be explicitly specified.
*
* \param lib Shared pointer to library to load function from.
*
* \param func_name Null-terminated name of the function to import. Can handle std::string, char*, const char*.
*
* \param mode An mode that will be used on library load.
*
* \return boost::function<T> that holds an imported function from the loaded library and refcounts usage
* of the loaded shared library.
*
* \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
*       Overload that accepts path also throws std::bad_alloc in case of insufficient memory.
*
*/
template <class T>
boost::function<T> import_function_alias(const boost::filesystem::path& lib, boost::string_ref func_name,
    load_mode::type mode = load_mode::default_mode)
{
    return import_function_alias<T>(
        boost::make_shared<shared_library>(lib),
        func_name
    );
}

//! \overload boost::dll::import_function_alias(const boost::shared_ptr<shared_library>& lib, boost::string_ref func_name, load_mode::type mode)
template <class T>
boost::function<T> import_function_alias(const boost::shared_ptr<shared_library>& lib, boost::string_ref func_name) {
    return boost::dll::detail::refc_function<T>(lib, lib->get<T*>(func_name));
}


}} // boost::dll

#endif // BOOST_DLL_IMPORT_FUNCTION_HPP

