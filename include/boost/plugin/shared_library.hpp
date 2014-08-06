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

#ifndef BOOST_PLUGIN_SHARED_LIBRARY_HPP
#define BOOST_PLUGIN_SHARED_LIBRARY_HPP

#include <boost/config.hpp>
#include <boost/predef/os.h>

#include <boost/plugin/shared_library_load_mode.hpp>
#include <boost/plugin/detail/system_error.hpp>

#if BOOST_OS_WINDOWS
#   include <boost/plugin/detail/windows/shared_library_impl.hpp>
#else
#   include <boost/plugin/detail/posix/shared_library_impl.hpp>
#endif


namespace boost { namespace plugin {

/*!
* \brief This class can be used to load a
*        Dynamic link libraries (DLL's) or Shared Library, also know
*        as dynamic shared objects (DSO's) and invoke their exported
*        symbols.
*
* Provides a means to extend your plugin using plugins way.
*
*/
class shared_library: private shared_library_impl {
    typedef shared_library_impl base_t;
    boost::filesystem::path path_;

public:
    /*!
    * Creates empty shared_library obj, need call load to
    * load a shared object.
    *
    */
    shared_library() BOOST_NOEXCEPT {}

    /*!
    * Creates a shared_library object loading a library
    * from library() initializer function.
    *
    * The library() can handle std::string, char, std::wstring,
    * wchar_t or filesystem path.
    *
    * \param sl An initializer free function, e.g.:
    *        shared_library sh(library("c:\\mylib.dll"));
    *
    * Throw a boost::system::system_error on a execption.
    *
    */
    explicit shared_library(const library_path &sl) {
        load(sl);
    }

    /*!
    * Creates a shared_library object loading a library
    * from library() initializer function.
    *
    * The library() can handle std::string, char, std::wstring,
    * wchar_t or filesystem path.
    *
    * \param sl An initializer free function, e.g.:
    *        shared_library sh(library("c:\\mylib.dll"), ec);
    *
    * \param ec Variable (boost::system::error_code) that will be
    *        set to the result of the operation.
    *
    * Check ec for errors.
    *
    */
    shared_library(const library_path &sl, boost::system::error_code &ec) {
        load(sl, ec);
    }

    /*!
    * Creates a shared_library object loading a library
    * from library() initializer function.
    *
    * The library() can handle std::string, char, std::wstring,
    * wchar_t or filesystem path.
    *
    * \param sl An initializer free function, e.g.:
    *        shared_library sh(library("c:\\mylib.dll"), 0x00000001);
    *
    * \param mode An mode that will be used on load.
    *
    * Throw a boost::system::system_error on a execption.
    *
    */
    shared_library(const library_path &sl, shared_library_load_mode mode) {
        load(sl, mode);
    }

    /*!
    * Creates a shared_library object loading a library
    * from library() initializer function.
    *
    * The library() can handle std::string, char, std::wstring,
    * wchar_t or filesystem path.
    *
    * \param sl An initializer free function, e.g.:
    *        shared_library sh(library("c:\\mylib.dll"), 0x00000001, ec);
    *
    * \param mode An mode that will be used on load.
    *
    * \param ec Variable (boost::system::error_code) that will be
    *        set to the result of the operation.
    *
    * Check ec for errors.
    *
    */
    shared_library(const library_path &sl, shared_library_load_mode mode, boost::system::error_code &ec) {
        load(sl, mode, ec);
    }

    /*!
    * Destructor
    * Destroys the SharedLibrary.
    * The actual library is unloaded. The unload() is called!
    *
    */
    ~shared_library() BOOST_NOEXCEPT {}


    /*!
    * Loads a shared library from library() initializer function.
    *
    * The library() can handle std::string, char, std::wstring,
    * wchar_t or filesystem path.
    *
    * \param sl An initializer free function, e.g.:
    *        sh.load(library("c:\\mylib.dll"));
    *
    * Throw a boost::system::system_error on a execption.
    *
    * Note that if a library is aread loaded, load will
    * unload it and then load the new provided library.
    *
    */
    void load(const library_path &sl) {
        path_ = sl;
        boost::system::error_code ec;
        base_t::load(sl, base_t::default_mode(), ec);

        if (ec) {
            path_.clear();
            boost::plugin::detail::report_error(ec, "load() failed");
        }
    }

    /*!
    * Loads a shared library from library() initializer function.
    *
    * The library() can handle std::string, char, std::wstring,
    * wchar_t or filesystem path.
    *
    * \param sl An initializer free function, e.g.:
    *        sh.load(library("c:\\mylib.dll"), ec);
    *
    * \param ec Variable (boost::system::error_code) that will be
    *        set to the result of the operation.
    *
    * Check ec for errors.
    *
    * Note that if a library is aread loaded, load will
    * unload it and then load the new provided library.
    *
    */
    void load(const library_path &sl, boost::system::error_code &ec) {
        path_ = sl;
        ec.clear();
        base_t::load(sl, base_t::default_mode(), ec);

        if (ec) {
            path_.clear();
        }
    }

    /*!
    * Loads a shared library from library() initializer function.
    *
    * The library() can handle std::string, char, std::wstring,
    * wchar_t or filesystem path.
    *
    * \param sl An initializer free function, e.g.:
    *        sl.load(library("c:\\mylib.dll"), 0);
    *
    * \param mode An mode that will be used on load.
    *
    * Throw a boost::system::system_error on a execption.
    *
    * Note that if a library is aread loaded, load will
    * unload it and then load the new provided library.
    *
    */
    void load(const library_path &sl, shared_library_load_mode mode) {
        path_ = sl;
        boost::system::error_code ec;
        base_t::load(sl, mode, ec);

        if (ec) {
            path_.clear();
            boost::plugin::detail::report_error(ec, "load() failed");
        }
    }

    /*!
    * Loads a shared library from library() initializer function.
    *
    * The library() can handle std::string, char, std::wstring,
    * wchar_t or filesystem path.
    *
    * \param sl An initializer free function, e.g.:
    *        sl.load("c:\\mylib.dll", 0, ec);
    *
    * \param mode An mode that will be used on load.
    *
    * \param ec Variable (boost::system::error_code) that will be
    *        set to the result of the operation.
    *
    * Check ec for errors.
    *
    * Note that if a library is aread loaded, load will
    * unload it and then load the new provided library.
    *
    */
    void load(const library_path &sl, shared_library_load_mode mode, boost::system::error_code &ec) {
        path_ = sl;
        ec.clear();
        base_t::load(sl, mode, ec);
        
        if (ec) {
            path_.clear();
        }
    }

    /*!
    * Unloads a shared library.
    *
    */
    void unload() BOOST_NOEXCEPT {
        base_t::unload();
        path_.clear();
    }

    /*!
    * Check if an library is loaded.
    *
    * \return true if a library has been loaded
    *
    */
    bool is_loaded() const BOOST_NOEXCEPT {
        return base_t::is_loaded();
    }

    /*!
    * Seach for d givem symbol on loaded library.
    *
    * The symbol() can handle std::string, char,
    * std::wstring or wchar_t.
    *
    * \param symbol_type An initializer free function, e.g.:
    *        sl.search_symbol(symbol("do_anything"))
    *
    * \return true if the loaded library contains
    *         a symbol from a given name.
    *
    */
    bool search_symbol(const symbol_type &sb) const BOOST_NOEXCEPT {
        boost::system::error_code ec;
        return !!base_t::symbol_addr(sb, ec) && !ec;
    }

    /*!
    * Returns the address of the symbol with the
    * given name to posterior call.
    *
    * Name must be provided using symbol() initializer
    * function that can handle std::string, char,
    * std::wstring or wchar_t.
    *
    * \param symbol_type An initializer free function, e.g.:
    *        sl.get_symbol(symbol("do_anything"))
    *
    * \return the address of symbol.
    *
    * Throw a boost::system::system_error on a execption, or
    * if symbol do not exist.
    *
    */
    template <typename Result>
    Result& get(const symbol_type &sb) const {
        boost::system::error_code ec;
        void* ret = base_t::symbol_addr(sb, ec);

        if (ec || !ret) {
            boost::plugin::detail::report_error(ec, "get() failed");
        }

        return *reinterpret_cast<Result*>(ret);
    }

    /*!
    * Returns the path of the loaded library, as specified
    * in a call to load() or the constructor/load.
    *
    * \return the boost::filesystem::path path of module.
    *
    */
    const boost::filesystem::path& path() const BOOST_NOEXCEPT {
        return path_;
    }

    /*!
    * Returns suffix od shared module:
    * in a call to load() or the constructor/load.
    *
    * \return the suffix od shared module, like:
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
    */
    void swap(shared_library& rhs) BOOST_NOEXCEPT {
        base_t::swap(rhs);
        path_.swap(rhs.path_);
    }
};


/*!
* Check equality of shared_library
* If the same shared library is loaded, means: same path
*
*/
inline bool operator==(const shared_library& lhs, const shared_library& rhs) BOOST_NOEXCEPT {
    return lhs.path() == rhs.path();
}

/*!
* Check equality of shared_library
* If the same shared library is loaded, means: same path
*
*/
inline bool operator<(const shared_library& lhs, const shared_library& rhs) BOOST_NOEXCEPT {
    return lhs.path() < rhs.path();
}

/*!
* Swaps two shared libraries.
* Does not invalidate existing symbols and functions loaded from libraries.
*/
inline void swap(shared_library& lhs, shared_library& rhs) BOOST_NOEXCEPT {
    lhs.swap(rhs);
}

}} // boost::plugin

#endif // BOOST_PLUGIN_SHARED_LIBRARY_HPP

