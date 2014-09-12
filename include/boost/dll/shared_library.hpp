// shared_library.hpp --------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 05-04-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_DLL_SHARED_LIBRARY_HPP
#define BOOST_DLL_SHARED_LIBRARY_HPP

/// \file boost/dll/shared_library.hpp
/// \brief Contains the boost::dll::shared_library class, main class for all the
/// DLL/DSO operations.

#include <boost/config.hpp>
#include <boost/predef/os.h>
#include <boost/dll/detail/system_error.hpp>
#include <boost/dll/detail/aggressive_ptr_cast.hpp>

#if BOOST_OS_WINDOWS
#   include <boost/dll/detail/windows/shared_library_impl.hpp>
#else
#   include <boost/dll/detail/posix/shared_library_impl.hpp>
#endif

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace dll {

/*!
* \brief This class can be used to load a
*        Dynamic link libraries (DLL's) or Shared Library, also know
*        as dynamic shared objects (DSO's) and get their exported
*        symbols (functions and variables).
*
* On Linux/POSIX link with library "dl". "-fvisibility=hidden" flag is also recomended for use on Linux/POSIX.
*/
class shared_library: protected shared_library_impl {
    typedef shared_library_impl base_t;
    
    // The 'shared library' is not enabled to be copied but can be movable.
    // This makes class 'shared library' movable.
    BOOST_MOVABLE_BUT_NOT_COPYABLE(shared_library)

public:
    typedef shared_library_impl::native_handle_t native_handle_t;

    /*!
    * Creates empty shared_library.
    *
    * \throw Nothing.
    */
    shared_library() BOOST_NOEXCEPT {}

    /*!
    * Creates a shared_library object and loads a library by specified path.
    *
    * \param lib_path Library file name. Can handle std::string, char*, std::wstring,
    *           wchar_t* or boost::filesystem::path.
    *
    * \throw boost::system::system_error.
    */
    explicit shared_library(const boost::filesystem::path& lib_path) {
        load(lib_path);
    }

    /*!
    * Creates a shared_library object and loads a library by specified path.
    *
    * \param lib_path Library file name. Can handle std::string, char, std::wstring,
    *           wchar_t or filesystem path.
    *
    * \param ec Variable that will be set to the result of the operation.
    *
    * \throw Nothing.
    */
    shared_library(const boost::filesystem::path& lib_path, boost::system::error_code &ec) BOOST_NOEXCEPT {
        load(lib_path, ec);
    }

    /*!
    * Creates a shared_library object and loads a library by specified path
    * with a specified mode.
    *
    * \param lib_path Library file name. Can handle std::string, char, std::wstring,
    *           wchar_t or filesystem path.
    *
    * \param mode An mode that will be used on library load.
    *
    * \throw boost::system::system_error.
    *
    */
    shared_library(const boost::filesystem::path& lib_path, load_mode::type mode) {
        load(lib_path, mode);
    }

    /*!
    * Creates a shared_library object and loads a library by specified path
    * with a specified mode.
    *
    * \param lib_path Library file name. Can handle std::string, char, std::wstring,
    *           wchar_t or filesystem path.
    *
    * \param ec Variable that will be set to the result of the operation.
    *
    * \param mode An mode that will be used on library load.
    *
    * \throw Nothing.
    *
    */
    shared_library(const boost::filesystem::path& lib_path, load_mode::type mode, boost::system::error_code &ec) BOOST_NOEXCEPT {
        load(lib_path, mode, ec);
    }
    
   /*!
    * Move a shared_library object.
    *
    * \param lib_path a shared_library to move from.
    *
    * \throw Nothing.
    */
    shared_library(BOOST_RV_REF(shared_library) lib) BOOST_NOEXCEPT // Move ctor
        : base_t(boost::move(static_cast<base_t&>(lib)))
    {}

   /*!
    * Move assign a shared_library object.
    *
    * \param lib_path a shared_library to move from.
    *
    * \throw Nothing.
    */
    shared_library& operator=(BOOST_RV_REF(shared_library) lib) BOOST_NOEXCEPT {
        base_t::operator=(boost::move(static_cast<base_t&>(lib)));
        return *this;
    }

    /*!
    * Destroys the shared_library.
    * `unload()` is called if the DLL/DSO was loaded. If library was loaded multiple times
    * by different instances of shared_library, the actual DLL/DSO won't be unloaded until
    * there is at least one instance of shared_library.
    *
    * \throw Nothing.
    */
    ~shared_library() BOOST_NOEXCEPT {}

    /*!
    * Loads a library by specified path.
    *
    * Note that if some library is already loaded, load will
    * unload it and then load the new provided library.
    *
    * \param lib_path Library file name. Can handle std::string, char, std::wstring,
    *           wchar_t or filesystem path.
    *
    * \throw boost::system::system_error.
    *
    */
    void load(const boost::filesystem::path& lib_path) {
        boost::system::error_code ec;
        base_t::load(lib_path, load_mode::default_mode, ec);

        if (ec) {
            boost::dll::detail::report_error(ec, "load() failed");
        }
    }

    /*!
    * Open myself (this executable) to have access to symbols. 
    *
    * Note that if some library is already loaded, load will
    * unload it and then load itself.
    *
    * Note that this can be used when DLLs/DSOs is directly linked into the 
    * executable.
    *
    * Flag '-rdynamic' must be used when linking the plugin into the executable
    * on Linux OS.
    *
    * See Tutorial "Linking plugin into the executable" for usage example.
    *
    * \throw boost::system::system_error.
    */
    void load_self() {
        boost::system::error_code ec;
        base_t::load_self(ec);

        if (ec) {
            boost::dll::detail::report_error(ec, "load_self() failed");
        }
    }

    /*!
    * Loads a library by specified path.
    *
    * Note that if some library is already loaded, load will
    * unload it and then load the new provided library.
    *
    * \param lib_path Library file name. Can handle std::string, char, std::wstring,
    *           wchar_t or filesystem path.
    *
    * \param ec Variable that will be set to the result of the operation.
    *
    * \throw Nothing.
    */
    void load(const boost::filesystem::path& lib_path, boost::system::error_code &ec) BOOST_NOEXCEPT {
        ec.clear();
        base_t::load(lib_path, load_mode::default_mode, ec);
    }

    /*!
    * Open myself (this executable) to have access to symbols. 
    *
    * Note that if some library is already loaded, load will
    * unload it and then load itself.
    *
    * Note that this can be used when DLLs/DSOs is directly linked into the 
    * executable.
    *
    * You must export the symbol even if you are on exe.
    *
    * Flag '-rdynamic' must be used when linking the plugin into the executable
    * on Linux OS.
    * 
    * See Tutorial "Linking plugin into the executable" for usage example.
    *
    * \param ec Variable that will be set to the result of the operation.
    *
    * \throw Nothing.
    */
    void load_self(boost::system::error_code &ec) BOOST_NOEXCEPT {
        ec.clear();
        base_t::load_self(ec);
    }

    /*!
    * Loads a library by specified path with a specified mode.
    *
    * Note that if some library is already loaded, load will
    * unload it and then load the new provided library.
    *
    * \param lib_path Library file name. Can handle std::string, char, std::wstring,
    *           wchar_t or filesystem path.
    *
    * \param mode An mode that will be used on library load.
    *
    * \throw boost::system::system_error.
    *
    */
    void load(const boost::filesystem::path& lib_path, load_mode::type mode) {
        boost::system::error_code ec;
        base_t::load(lib_path, mode, ec);

        if (ec) {
            boost::dll::detail::report_error(ec, "load() failed");
        }
    }

    /*!
    * Loads a library by specified path with a specified mode.
    *
    * Note that if some library is already loaded, load will
    * unload it and then load the new provided library.
    *
    * \param lib_path Library file name. Can handle std::string, char, std::wstring,
    *           wchar_t or filesystem path.
    *
    * \param ec Variable that will be set to the result of the operation.
    *
    * \param mode An mode that will be used on library load.
    *
    * \throw Nothing.
    *
    */
    void load(const boost::filesystem::path& lib_path, load_mode::type mode, boost::system::error_code &ec) BOOST_NOEXCEPT {
        ec.clear();
        base_t::load(lib_path, mode, ec);
    }

    /*!
    * Unloads a shared library. If library was loaded multiple times
    * by different instances of shared_library, the actual DLL/DSO won't be unloaded until
    * there is at least one instance of shared_library.
    *
    */
    void unload() BOOST_NOEXCEPT {
        base_t::unload();
    }

    /*!
    * Check if an library is loaded.
    *
    * \return true if a library has been loaded.
    *
    * \throw Nothing.
    *
    */
    bool is_loaded() const BOOST_NOEXCEPT {
        return base_t::is_loaded();
    }

    /*!
    * Seach for a given symbol on loaded library. Works for all symbols, including alias names.
    *
    * \param symbol Null-terminated symbol name. Can handle std::string, char*, const char*.
    *
    * \return `true` if the loaded library contains a symbol with a given name.
    *
    * \throw Nothing.
    *
    */
    bool search_symbol(boost::string_ref symbol_name) const BOOST_NOEXCEPT {
        boost::system::error_code ec;
        return is_loaded() && !!base_t::symbol_addr(symbol_name, ec) && !ec;
    }

    /*!
    * Returns reference to the symbol (function or variable) with the given name from the loaded library.
    * This call will always succeed and throw nothing if call to `search_symbol(boost::string_ref )`
    * member function with the same symbol name returned `true`.
    *
    * If using this call for an alias name do not forget to add a pointer to a resulting type.
    *
    * \b Example:
    * \code
    * shared_library lib("test_lib.so");
    * int& i0 = lib.get<int>("integer_name");
    * int& i1 = *lib.get<int*>("integer_alias_name");
    * \endcode
    *
    * \tparam T Type of the symbol that we are going to import. Must be explicitly specified.
    *
    * \param symbol Null-terminated symbol name. Can handle std::string, char*, const char*.
    *
    * \return Reference to the symbol.
    *
    * \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
    *
    */
    template <typename T>
    inline T& get(boost::string_ref symbol_name) const {
        return *boost::dll::detail::aggressive_ptr_cast<T*>(
            get_impl(symbol_name)
        );
    }

    /*!
    * Returns a symbol (function or variable) from a shared library by alias name of the symbol.
    *
    * \b Example:
    * \code
    * shared_library lib("test_lib.so");
    * int& i = lib.get_alias<int>("integer_alias_name");
    * \endcode
    *
    * \tparam T Type of the symbol that we are going to import. Must be explicitly specified..
    *
    * \param alias_name Null-terminated alias symbol name. Can handle std::string, char*, const char*.
    *
    * \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
    */
    template <typename T>
    inline T& get_alias(boost::string_ref alias_name) const {
        return *get<T*>(alias_name);
    }

private:
    // get_impl is required to reduce binary size: it does not depend on a template
    // parameter and will be instantiated only once.
    void* get_impl(boost::string_ref sb) const {
        boost::system::error_code ec;

        if (!is_loaded()) {
            ec = boost::system::error_code(
                boost::system::errc::bad_file_descriptor,
                boost::system::generic_category()
            );

            // report_error() calls dlsym, do not use it here!
            boost::throw_exception(
                boost::system::system_error(
                    ec, "get() failed (no library was loaded)"
                )
            );
        }

        void* const ret = base_t::symbol_addr(sb, ec);
        if (ec || !ret) {
            boost::dll::detail::report_error(ec, "get() failed");
        }

        return ret;
    }

public:

    /*!
    * Returns the native handler of the loaded library.
    *
    * \return Platform-specific handle.
    */
    native_handle_t native() const BOOST_NOEXCEPT {
        return base_t::native();
    }

   /*!
    * Returns full path of shared module.
    *
    * The ability to extract the full file system path of a loaded shared
    * library can be useful in case that use provide only library name on load, 
    * and on some time need to get full path of it. 
    *
    * \b Example:
    * \code
    * shared_library lib("test_lib.dll");
    * filesystem::path full_path = lib.full_module_path(); // C:\Windows\System32\test_lib.dll
    * \endcode
    *
    * \return The full path of shared module, like:
    *
    * c:\temp\mylib.dll (windows)
    * /temp/mylib.so (unix)
    * /temp/mylib.dylib (mac)
    *
    * \throw boost::system::system_error, std::bad_alloc.
    */
    boost::filesystem::path path() const {
        boost::system::error_code ec;
        if (!is_loaded()) {
            ec = boost::system::error_code(
                boost::system::errc::bad_file_descriptor,
                boost::system::generic_category()
            );

            boost::throw_exception(
                boost::system::system_error(
                    ec, "full_module_path() failed (no library was loaded)"
                )
            );
        }

        boost::filesystem::path full_path = base_t::full_module_path(ec);

        if (ec) {
            boost::dll::detail::report_error(ec, "full_module_path() failed");
        }

        return full_path;
    }

   /*!
    * Returns full path of shared module.
    *
    * The ability to extract the full file system path of a loaded shared
    * library can be useful in case that use provide only library name on load, 
    * and on some time need to get full path of it. 
    *
    * \b Example:
    * \code
    * shared_library lib("test_lib.dll");
    * filesystem::path full_path = lib.full_module_path(); // C:\Windows\System32\test_lib.dll
    * \endcode
    *
    * \return The full path of shared module, like:
    *
    * c:\temp\test_lib.dll (windows)
    * /temp/test_lib.so (unix)
    * /temp/test_lib.dylib (mac)
    *
    * \param ec Variable that will be set to the result of the operation.
    *
    * \throw std::bad_alloc.
    */
    boost::filesystem::path path(boost::system::error_code &ec) const {
        if (!is_loaded()) {
            ec = boost::system::error_code(
                boost::system::errc::bad_file_descriptor,
                boost::system::generic_category()
            );

            return boost::filesystem::path();
        }

        return base_t::full_module_path(ec);
    }

    /*!
    * Returns suffix od shared module:
    * in a call to load() or the constructor/load.
    *
    * \return The suffix od shared module, like:
    *
    * .dll (windows)
    * .so (unix)
    * .dylib (mac)
    *
    */
    static boost::filesystem::path suffix() {
        return base_t::suffix();
    }

    /*!
    * Swaps two libraries.
    * Does not invalidate existing symbols and functions loaded from libraries.
    *
    * \param rhs Library to swap with.
    *
    * \throw Nothing.
    */
    void swap(shared_library& rhs) BOOST_NOEXCEPT {
        base_t::swap(rhs);
    }
};


/*!
* Check equality of shared_library, libraries compared using native handles.
*
* \throw Nothing.
*/
inline bool operator==(const shared_library& lhs, const shared_library& rhs) BOOST_NOEXCEPT {
    return lhs.native() == rhs.native();
}

/*!
* Check inequality of shared_library, libraries compared using native handles.
*
* \throw Nothing.
*/
inline bool operator!=(const shared_library& lhs, const shared_library& rhs) BOOST_NOEXCEPT {
    return lhs.native() != rhs.native();
}

/*!
* Less compares loaded libraries using native handles.
*
* \throw Nothing.
*/
inline bool operator<(const shared_library& lhs, const shared_library& rhs) BOOST_NOEXCEPT {
    return lhs.native() < rhs.native();
}

/*!
* Swaps two shared libraries.
* Does not invalidate existing symbols and functions loaded from libraries.
*
* \throw Nothing.
*/
inline void swap(shared_library& lhs, shared_library& rhs) BOOST_NOEXCEPT {
    lhs.swap(rhs);
}

}} // boost::dll

#endif // BOOST_DLL_SHARED_LIBRARY_HPP

