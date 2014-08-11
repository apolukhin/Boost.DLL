// refccountable.hpp --------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2014 Renato Tegon Forti, Antony Polukhin.

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 06-08-2014 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_PLUGIN_REFCOUNTABLE_HPP
#define BOOST_PLUGIN_REFCOUNTABLE_HPP

#include <boost/config.hpp>
#include <boost/plugin/shared_library.hpp>
#include <boost/plugin/alias.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>

namespace boost { namespace plugin {

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


    struct ptr_holding_empty_deleter {
        boost::shared_ptr<shared_library> lib_;

        explicit ptr_holding_empty_deleter(const boost::shared_ptr<shared_library>& lib)
            : lib_(lib)
        {}

        inline void operator()(const void*) const BOOST_NOEXCEPT { /*do nothing*/ }
    };
}


///////////////////////////////// shared_function methods ////////////////////////////////////////////
/*!
* Returns boost::function<T> that holds an imported function from the loaded library and refcounts usage
* of the loaded shared library, so that it won't get unload until all copies of return value
* are not destroyed.
*
* This call will succeed if call to `shared_library::search_symbol(const symbol_type &)`
* function with the same symbol name returned `true`.
*
* For importing function by it alias name use boost::plugin::shared_function_alias method.
*
* This method has an overload that accepts boost::filesystem::path as a first argument.
*
* \b Example:
* \code
* boost::function<int(int)> f = shared_function<int(int)>(
*           boost::make_shared<shared_library>("test_lib.so"),
*           "integer_func_name"
* );
* \endcode
*
* \tparam T Type of the symbol that we are going to import. Must be explicitly specified.
*
* \param lib Shared pointer to library to load function from.
*
* \param func_name Name of the function to import. Can handle std::string, char*, const char*.
*
* \return boost::function<T> that holds an imported function from the loaded library and refcounts usage
* of the loaded shared library.
*
* \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
*
*/
template <class T>
boost::function<T> shared_function(const boost::shared_ptr<shared_library>& lib, boost::string_ref func_name) {
    return boost::plugin::detail::refc_function<T>(lib, &lib->get<T>(func_name));
}

/*!
* Returns boost::function<T> that holds an imported function from the loaded library and refcounts usage
* of the loaded shared library, so that it won't get unload until all copies of return value
* are not destroyed.
*
* This call will succeed if call to `shared_library::search_symbol(const symbol_type &)`
* function with the same symbol name returned `true`.
*
* For importing function by it's alias name use boost::plugin::shared_function_alias method.
*
* \b Example:
* \code
* boost::function<int(int)> f = shared_function<int(int)>("test_lib.so", "integer_func_name");
* \endcode
*
* \tparam T Type of the symbol that we are going to import. Must be explicitly specified.
*
* \param lib_path Path to the library to load function from.
*
* \param func_name Name of the function to import. Can handle std::string, char*, const char*.
*
* \return boost::function<T> that holds an imported function from the loaded library and refcounts usage
* of the loaded shared library.
*
* \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
*        std::bad_alloc in case of insufficient memory.
*
*/
template <class T>
boost::function<T> shared_function(const boost::filesystem::path& lib_path, boost::string_ref func_name) {
    return shared_function<T>(
        boost::make_shared<shared_library>(lib_path),
        func_name
    );
}



//////////////////////////////////////// shared_function_alias methods //////////////////////////////////////////
/*!
* Imports function by it alias name and returns boost::function<T> that holds the function
* from the loaded library and refcounts usage of the loaded shared library.  Library won't
* get unload until all copies of returned value are not destroyed.
*
* This call will succeed if call to `shared_library::search_symbol(const symbol_type &)`
* function with the same symbol name returned `true`.
*
* For non alias names use boost::plugin::shared_function method.
*
* This method has an overload that accepts boost::filesystem::path as a first argument.
*
* \b Example:
* \code
* boost::function<int(int)> f = shared_function_alias<int(int)>(
*           boost::make_shared<shared_library>("test_lib.so"),
*           "integer_func_alias_name"
* );
* \endcode
*
* \tparam T Type of the symbol that we are going to import. Must be explicitly specified.
*
* \param lib Shared pointer to library to load function from.
*
* \param func_name Name of the function to import. Can handle std::string, char*, const char*.
*
* \return boost::function<T> that holds an imported function from the loaded library and refcounts usage
* of the loaded shared library.
*
* \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
*
*/
template <class T>
boost::function<T> shared_function_alias(const boost::shared_ptr<shared_library>& lib, boost::string_ref func_name) {
    return boost::plugin::detail::refc_function<T>(lib, lib->get<T*>(func_name));
}

/*!
* Imports function by it alias name and returns boost::function<T> that holds the function
* from the loaded library and refcounts usage of the loaded shared library.  Library won't
* get unload until all copies of returned value are not destroyed.
*
* This call will succeed if call to `shared_library::search_symbol(const symbol_type &)`
* function with the same symbol name returned `true`.
*
* For non alias names use boost::plugin::shared_function method.
*
* \b Example:
* \code
* boost::function<int(int)> f = shared_function_alias<int(int)>("test_lib.so", "integer_func_alias_name");
* \endcode
*
* \tparam T Type of the symbol that we are going to import. Must be explicitly specified.
*
* \param lib_path Path to the library to load function from.
*
* \param func_name Name of the function to import. Can handle std::string, char*, const char*.
*
* \return boost::function<T> that holds an imported function from the loaded library and refcounts usage
* of the loaded shared library.
*
* \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
*        std::bad_alloc in case of insufficient memory.
*
*/
template <class T>
boost::function<T> shared_function_alias(const boost::filesystem::path& lib_path, boost::string_ref func_name) {
    return shared_function_alias<T>(
        boost::make_shared<shared_library>(lib_path),
        func_name
    );
}



//////////////////////////////////////////// shared_variable methods /////////////////////////////////////////////////
/*!
* Returns boost::shared_ptr<T> that holds an imported variable from the loaded library and refcounts usage
* of the loaded shared library, so that it won't get unload until all copies of return value
* are not destroyed.
*
* This call will succeed if call to `shared_library::search_symbol(const symbol_type &)`
* function with the same symbol name returned `true`.
*
* For importing variable by it's alias name use boost::plugin::shared_variable_alias method.
*
* This method has an overload that accepts boost::filesystem::path as a first argument.
*
* \b Example:
* \code
* boost::shared_ptr<int> i = shared_variable<int>(
*           boost::make_shared<shared_library>("test_lib.so"),
*           "integer_name"
* );
* \endcode
*
* \tparam T Type of the symbol that we are going to import. Must be explicitly specified.
*
* \param lib Shared pointer to library to load variable from.
*
* \param variable_name Name of the variable to import. Can handle std::string, char*, const char*.
*
* \return boost::shared_ptr<T> that holds an imported function from the loaded library and refcounts usage
* of the loaded shared library.
*
* \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
*
*/
template <class T>
boost::shared_ptr<T> shared_variable(const boost::shared_ptr<shared_library>& lib, boost::string_ref variable_name) {
    return boost::shared_ptr<T>(&lib->get<T>(variable_name), boost::plugin::detail::ptr_holding_empty_deleter(lib));
}

/*!
* Returns boost::shared_ptr<T> that holds an imported variable from the loaded library and refcounts usage
* of the loaded shared library, so that it won't get unload until all copies of return value
* are not destroyed.
*
* This call will succeed if call to `shared_library::search_symbol(const symbol_type &)`
* function with the same symbol name returned `true`.
*
* For importing variable by it's alias name use boost::plugin::shared_variable_alias method.
*
* \b Example:
* \code
* boost::shared_ptr<int> i = shared_variable<int>("test_lib.so", "integer_name");
* \endcode
*
* \tparam T Type of the symbol that we are going to import. Must be explicitly specified.
*
* \param lib_path Path to the library to load variable from.
*
* \param variable_name Name of the variable to import. Can handle std::string, char*, const char*.
*
* \return boost::shared_ptr<T> that holds an imported function from the loaded library and refcounts usage
* of the loaded shared library.
*
* \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
*        std::bad_alloc in case of insufficient memory.
*
*/
template <class T>
boost::shared_ptr<T> shared_variable(const boost::filesystem::path& lib_path, boost::string_ref variable_name) {
    return shared_variable<T>(
        boost::make_shared<shared_library>(lib_path),
        variable_name
    );
}



//////////////////////////////////////////////// shared_variable_alias methods //////////////////////////////////////////////
/*!
* Imports function by it alias name and returns boost::shared_ptr<T> that holds an imported
* variable from the loaded library and refcounts usage of the loaded shared library.
* Library won't get unload until all copies of returned value are not destroyed.
*
* This call will succeed if call to `shared_library::search_symbol(const symbol_type &)`
* function with the same symbol name returned `true`.
*
* For non alias names use boost::plugin::shared_variable method.
*
* This method has an overload that accepts boost::filesystem::path as a first argument.
*
* \b Example:
* \code
* boost::shared_ptr<int> i = shared_variable_alias<int>(
*           boost::make_shared<shared_library>("test_lib.so"),
*           "integer_name"
* );
* \endcode
*
* \tparam T Type of the symbol that we are going to import. Must be explicitly specified.
*
* \param lib Shared pointer to the library to load variable from.
*
* \param variable_name Name of the variable to import. Can handle std::string, char*, const char*.
*
* \return boost::shared_ptr<T> that holds an imported variable from the loaded library and refcounts usage
* of the loaded shared library.
*
* \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
*
*/
template <class T>
boost::shared_ptr<T> shared_variable_alias(const boost::shared_ptr<shared_library>& lib, boost::string_ref variable_name) {
    return boost::shared_ptr<T>(lib->get<T*>(variable_name), boost::plugin::detail::ptr_holding_empty_deleter(lib));
}

/*!
* Imports function by it alias name and returns boost::shared_ptr<T> that holds an imported
* variable from the loaded library and refcounts usage of the loaded shared library.
* Library won't get unload until all copies of returned value are not destroyed.
*
* This call will succeed if call to `shared_library::search_symbol(const symbol_type &)`
* function with the same symbol name returned `true`.
*
* For non alias names use boost::plugin::shared_variable method.
*
* \b Example:
* \code
* boost::shared_ptr<int> i = shared_variable_alias<int>("test_lib.so", "integer_name");
* \endcode
*
* \tparam T Type of the symbol that we are going to import. Must be explicitly specified.
*
* \param lib_path Path to the library to load variable from.
*
* \param variable_name Name of the variable to import. Can handle std::string, char*, const char*.
*
* \return boost::shared_ptr<T> that holds an imported variable from the loaded library and refcounts usage
* of the loaded shared library.
*
* \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
*        std::bad_alloc in case of insufficient memory.
*
*/
template <class T>
boost::shared_ptr<T> shared_variable_alias(const boost::filesystem::path& lib_path, boost::string_ref variable_name) {
    return shared_variable_alias<T>(
        boost::make_shared<shared_library>(lib_path),
        variable_name
    );
}

}} // boost::plugin

#endif // BOOST_PLUGIN_REFCOUNTABLE_HPP

