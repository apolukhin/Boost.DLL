// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_DLL_IMPORT_VARIABLE_HPP
#define BOOST_DLL_IMPORT_VARIABLE_HPP

#include <boost/config.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

/// \file boost/dll/import_variable.hpp
/// \brief Contains all the boost::dll::import_variable* reference counting
/// functions that hold a shared pointer to the instance of
/// boost::dll::shared_library.

namespace boost { namespace dll {

namespace detail {
    struct ptr_holding_empty_deleter {
        boost::shared_ptr<shared_library> lib_;

        explicit ptr_holding_empty_deleter(const boost::shared_ptr<shared_library>& lib)
            : lib_(lib)
        {}

        inline void operator()(const void*) const BOOST_NOEXCEPT { /*do nothing*/ }
    };
} // namespace detail

/*!
* Returns boost::shared_ptr<T> that holds an imported variable from the loaded library and refcounts usage
* of the loaded shared library, so that it won't get unload until all copies of return value
* are not destroyed.
*
* This call will succeed if call to \forcedlink{shared_library}`::search_symbol(boost::string_ref )`
* function with the same symbol name returned `true`.
*
* For importing variable by it's \b alias name use \forcedlink{import_variable_alias} method.
*
* \b Examples:
* \code
* boost::shared_ptr<int> i = import_variable<int>(
*           boost::make_shared<shared_library>("test_lib.so"),
*           "integer_name"
* );
* \endcode
*
* \code
* boost::shared_ptr<int> i = import_variable<int>("test_lib.so", "integer_name");
* \endcode
*
* \tparam T Type of the symbol that we are going to import. Must be explicitly specified.
*
* \param lib Shared pointer to library to load variable from.
*
* \param variable_name Null-terminated name of the variable to import. Can handle std::string, char*, const char*.
*
* \param mode An mode that will be used on library load.
*
* \return boost::shared_ptr<T> that holds an imported function from the loaded library and refcounts usage
* of the loaded shared library.
*
* \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
*       Overload that accepts path also throws std::bad_alloc in case of insufficient memory.
*
*/
template <class T>
boost::shared_ptr<T> import_variable(const boost::filesystem::path& lib, boost::string_ref variable_name,
    load_mode::type mode = load_mode::default_mode)
{
    return import_variable<T>(
        boost::make_shared<shared_library>(lib, mode),
        variable_name
    );
}

//! \overload boost::dll::import_variable(const boost::shared_ptr<shared_library>& lib, boost::string_ref variable_name, load_mode::type mode)
template <class T>
boost::shared_ptr<T> import_variable(const boost::shared_ptr<shared_library>& lib, boost::string_ref variable_name) {
    return boost::shared_ptr<T>(&lib->get<T>(variable_name), boost::dll::detail::ptr_holding_empty_deleter(lib));
}



/*!
* Imports variable by it alias name and returns boost::shared_ptr<T> that holds an imported
* variable from the loaded library and refcounts usage of the loaded shared library.
* Library won't get unload until all copies of returned value are not destroyed.
*
* This call will succeed if call to \forcedlink{shared_library}`::search_symbol(boost::string_ref )`
* function with the same symbol name returned `true`.
*
* For \b non \b alias names use \forcedlink{import_variable} method.
*
* \b Examples:
* \code
* boost::shared_ptr<int> i = import_variable_alias<int>(
*           boost::make_shared<shared_library>("test_lib.so"),
*           "integer_name"
* );
* \endcode
*
* \code
* boost::shared_ptr<int> i = import_variable_alias<int>("test_lib.so", "integer_name");
* \endcode
*
* \tparam T Type of the symbol that we are going to import. Must be explicitly specified.
*
* \param lib Shared pointer to the library to load variable from.
*
* \param variable_name Null-terminated name of the variable to import. Can handle std::string, char*, const char*.
*
* \param mode An mode that will be used on library load.
*
* \return boost::shared_ptr<T> that holds an imported variable from the loaded library and refcounts usage
* of the loaded shared library.
*
* \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
*       Overload that accepts path also throws std::bad_alloc in case of insufficient memory.
*
*/
template <class T>
boost::shared_ptr<T> import_variable_alias(const boost::filesystem::path& lib, boost::string_ref variable_name,
    load_mode::type mode = load_mode::default_mode)
{
    return import_variable_alias<T>(
        boost::make_shared<shared_library>(lib, mode),
        variable_name
    );
}

//! \overload boost::dll::import_variable_alias(const boost::shared_ptr<shared_library>& lib, boost::string_ref variable_name, load_mode::type mode)
template <class T>
boost::shared_ptr<T> import_variable_alias(const boost::shared_ptr<shared_library>& lib, boost::string_ref variable_name) {
    return boost::shared_ptr<T>(lib->get<T*>(variable_name), boost::dll::detail::ptr_holding_empty_deleter(lib));
}


}} // boost::dll

#endif // BOOST_DLL_IMPORT_VARIABLE_HPP

