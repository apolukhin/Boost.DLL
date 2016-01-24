/*
 * smart_library.hpp
 *
 *  Created on: 17.10.2015
 *      Author: Klemens
 */

#ifndef INCLUDE_BOOST_DLL_SMART_LIBRARY_HPP_
#define INCLUDE_BOOST_DLL_SMART_LIBRARY_HPP_

#include <boost/dll/shared_library.hpp>
#include <boost/dll/detail/get_mem_fn_type.hpp>
#include <boost/dll/detail/mem_fn_cast.hpp>
#include <boost/dll/detail/ctor_dtor.hpp>

#include <boost/predef/compiler.h>


#if BOOST_COMP_GNUC || BOOST_COMP_CLANG || BOOST_COMP_HPACC || BOOST_COMP_IBM
#include <boost/dll/detail/demangling/itanium.hpp>
#elif BOOST_COMP_MSVC
#include <boost/dll/detail/demangling/msvc.hpp>
#else
#error "Compiler not supported"
#endif

namespace boost {
namespace dll {
namespace experimental {

/*!
* \brief This class is an extension of \ref shared_library, which allows to load C++ symbols.
*
* This class allows type safe loading of overloaded functions, member-functions, constructors and variables.
* It also allows to overwrite classes so they can be loaded, while being declared with different names.
*
* \warning Is still very experimental.
*
* Currently known limitations:
*
* Member functions must be defined outside of the class to be exported. That is:
* \code
* //not exported:
* struct BOOST_SYMBOL_EXPORT my_class { void func() {}};
* //exported
* struct BOOST_SYMBOL_EXPORT my_class { void func();};
* void my_class::func() {};
* \endcode
*
* With the current analysis, the first version does get exported in MSVC.
* MinGW also does export it, BOOST_SYMBOL_EXPORT is written before it. To allow this on windows one can use
* BOOST_DLL_MEMBER_EXPORT for this, so that MinGW and MSVC can provide those functions. This does however not work with gcc on linux.
*
* Direct initialization of members.
* On linux the following member variable i will not be initialized when using the allocating contructor:
* \code
* struct BOOST_SYMBOL_EXPORT my_class { int i; my_class() : i(42) {} };
* \endcode
*
* This does however not happen when the value is set inside the constructor function.
*/
class smart_library
{
	shared_library _lib;
	detail::mangled_storage_impl _storage;
public:
	/*!
	 * Get the underlying shared_library
	 */

	const shared_library &shared_lib() const {return _lib;}

	using mangled_storage = detail::mangled_storage_impl;
    /*!
    * Acces to the mangled storage, which is created on construction.
    *
    * \throw Nothing.
    */
	const mangled_storage &symbol_storage() const {return _storage;}
    /*!
    * Creates empty smart_library.
    *
    * \throw Nothing.
    */
	smart_library() BOOST_NOEXCEPT;

    /*!
    * Creates a smart_library object and loads a library by specified path
    * with a specified mode.
    *
    * \param lib_path Library file name. Can handle std::string, const char*, std::wstring,
    *           const wchar_t* or boost::filesystem::path.
    *
    * \param mode A mode that will be used on library load.
    *
    * \throw boost::system::system_error, std::bad_alloc in case of insufficient memory.
    *
    */
	smart_library(const boost::filesystem::path& lib_path, load_mode::type mode = load_mode::default_mode)
	{
		_lib.load(lib_path, mode);
		_storage.load(lib_path, mode);
    }

    /*!
    * Creates a smart_library object and loads a library by specified path
    * with a specified mode.
    *
    * \param lib_path Library file name. Can handle std::string, const char*, std::wstring,
    *           const wchar_t* or boost::filesystem::path.
    *
    * \param mode A mode that will be used on library load.
    *
    * \param ec Variable that will be set to the result of the operation.
    *
    * \throw std::bad_alloc in case of insufficient memory.
    */
	smart_library(const boost::filesystem::path& lib_path, boost::system::error_code& ec, load_mode::type mode = load_mode::default_mode) {
		load(lib_path, mode, ec);
    }

    //! \overload smart_library(const boost::filesystem::path& lib_path, boost::system::error_code& ec, load_mode::type mode = load_mode::default_mode)
	smart_library(const boost::filesystem::path& lib_path, load_mode::type mode, boost::system::error_code& ec) {
		load(lib_path, mode, ec);
    }

   /*!
	* Move a shared_library object.
	*
	* \param lib A shared_library to move from.
	*
	* \throw Nothing.
	*/
	smart_library(BOOST_RV_REF(smart_library) lib) BOOST_NOEXCEPT // Move ctor
		: _lib(boost::move(static_cast<shared_library&>(lib._lib))), _storage(boost::move(lib._storage))
	{}
	/*!
    * Destroys the smart_library.
    * `unload()` is called if the DLL/DSO was loaded. If library was loaded multiple times
    * by different instances of shared_library, the actual DLL/DSO won't be unloaded until
    * there is at least one instance of shared_library.
    *
    * \throw Nothing.
    */
	~smart_library() BOOST_NOEXCEPT {};

    /*!
    * Loads a library by specified path with a specified mode.
    *
    * Note that if some library is already loaded in this shared_library instance, load will
    * call unload() and then load the new provided library.
    *
    * \param lib_path Library file name. Can handle std::string, const char*, std::wstring,
    *           const wchar_t* or boost::filesystem::path.
    *
    * \param mode A mode that will be used on library load.
    *
    * \throw boost::system::system_error, std::bad_alloc in case of insufficient memory.
    *
    */
	void load(const boost::filesystem::path& lib_path, load_mode::type mode = load_mode::default_mode)  {
        boost::system::error_code ec;
        _storage.load(lib_path);
        _lib.load(lib_path, mode, ec);

        if (ec) {
            boost::dll::detail::report_error(ec, "load() failed");
        }
    }

    /*!
    * Loads a library by specified path with a specified mode.
    *
    * Note that if some library is already loaded in this shared_library instance, load will
    * call unload() and then load the new provided library.
    *
    * \param lib_path Library file name. Can handle std::string, const char*, std::wstring,
    *           const wchar_t* or boost::filesystem::path.
    *
    * \param ec Variable that will be set to the result of the operation.
    *
    * \throw std::bad_alloc in case of insufficient memory.
    */
    void load(const boost::filesystem::path& lib_path, boost::system::error_code& ec, load_mode::type mode = load_mode::default_mode)  {
        ec.clear();
        _storage.load(lib_path);
        _lib.load(lib_path, mode, ec);
    }

    //! \overload void load(const boost::filesystem::path& lib_path, boost::system::error_code& ec, load_mode::type mode = load_mode::default_mode)
    void load(const boost::filesystem::path& lib_path, load_mode::type mode, boost::system::error_code& ec)  {
        ec.clear();
        _storage.load(lib_path);
        _lib.load(lib_path, mode, ec);
    }
    /*!
     * Load a variable from the referenced library.
     *
     * Unlinke shared_library::get this function will also load scoped variables, which also includes static class members.
     *
     * \note When mangled, MSVC will also check the type.
     *
     * \param name Name of the variable
     * \tparam T Type of the variable
     * \return A reference to the variable of type T.
     *
     * \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
     *
     */
	template<typename T>
	T& get_variable(const std::string &name)
	{
		return _lib.get<T>(_storage.get_variable<T>(name));
	}
    /*!
     * Load a function from the referenced library.
     *
     * \b Example:
     *
     * \code
     * smart_library lib("test_lib.so");
     * typedef int 	 (&add_ints)(int, int);
     * typedef double (&add_doubles)(double, double);
     * add_ints 	f1 = lib.get_function<int(int, int)>		 ("func_name");
     * add_doubles  f2 = lib.get_function<double(double, double)>("func_name");
     * \endcode
     *
     * \note When mangled, MSVC will also check the return type.
     *
     * \param name Name of the function.
     * \tparam Func Type of the function, required for determining the overload
     * \return A reference to the function of type F.
     *
     * \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
     *
     */
	template<typename Func>
	Func& get_function(const std::string &name)
	{
		return _lib.get<Func>(_storage.get_function<Func>(name));
	}
    /*!
     * Load a member-function from the referenced library.
     *
     * \b Example (import class is MyClass, which is available inside the library and the host):
     *
     * \code
     * smart_library lib("test_lib.so");
     *
     * typedef int 	 MyClass(*func)(int);
     * typedef int   MyClass(*func_const)(int) const;
     *
     * add_ints 	f1 = lib.get_mem_fn<MyClass, int(int)>			  ("MyClass::function");
     * add_doubles  f2 = lib.get_mem_fn<const MyClass, double(double)>("MyClass::function");
     * \endcode
     *
     * \note When mangled, MSVC will also check the return type.
     *
     * \param name Name of the function.
     * \tparam Class The class the function is a member of. If Class is const, the function will be assumed as taking a const this-pointer. The same applies for volatile.
     * \tparam Func Signature of the function, required for determining the overload
     * \return A pointer to the member-function with the signature provided
     *
     * \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
     *
     */
	template<typename Class, typename Func>
	typename detail::get_mem_fn_type<Class, Func>::mem_fn
			get_mem_fn(const std::string &name)
	{
		return detail::mem_fn_cast<typename detail::get_mem_fn_type<Class, Func>::mem_fn>
				(_lib.get_void(_storage.get_mem_fn<Class, Func>(name)));
	}

    /*!
     * Load a constructor from the referenced library.
     *
     * \b Example (import class is MyClass, which is available inside the library and the host):
     *
     * \code
     * smart_library lib("test_lib.so");
     *
     * constructor<MyClass(int)	f1 = lib.get_mem_fn<MyClass(int)>();
     * \endcode
     *
     * \tparam Signature Signature of the function, required for determining the overload. The return type is the class which this is the constructor of.
     * \return A constructor object.
     *
     * \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
     *
     */
	template<typename Signature>
	constructor<Signature> get_constructor()
	{
		return detail::load_ctor<Signature>(_lib, _storage.get_constructor<Signature>());
	}

	/*!
	 * Load a destructor from the referenced library.
	 *
	 * \b Example (import class is MyClass, which is available inside the library and the host):
	 *
	 * \code
	 * smart_library lib("test_lib.so");
	 *
	 * destructor<MyClass> 	f1 = lib.get_mem_fn<MyClass>();
	 * \endcode
	 *
	 * \tparam Class The class whichs destructor shall be loaded
	 * \return A destructor object.
	 *
	 * \throw boost::system::system_error if symbol does not exist or if the DLL/DSO was not loaded.
	 *
	 */
	template<typename Class>
	destructor<Class> get_destructor()
	{
		return detail::load_dtor<Class>(_lib, _storage.get_destructor<Class>());

	}
	/**
	 * This function can be used to add a type alias.
	 *
	 * This is to be used, when a class shall be imported, which is not declared on the host side.
	 *
	 * Example:
	 * \code
	 * smart_library lib("test_lib.so");
	 *
	 * lib.add_type_alias<MyAlias>("MyClass"); //when using MyAlias, the library will look for MyClass
	 *
	 * //get the destructor of MyClass
	 * destructor<MyAlias> dtor = lib.get_destructor<MyAlias>();
	 * \endcode
	 *
	 *
	 * \param name Name of the class the alias is for.
	 *
	 * \attention If the alias-type is not large enough for the imported class, it will result in undefined behaviour.
	 * \warning The alias will only be applied for the type signature, it will not replace the token in the scoped name.
	 */
	template<typename Alias> void add_type_alias(const std::string& name)
	{
		this->_storage.add_alias<Alias>(name);
	}

    /*!
    * Unloads a shared library. If library was loaded multiple times
    * by different instances of shared_library, the actual DLL/DSO won't be unloaded until
    * there is at least one instance of shared_library holding a reference to it.
    *
    * \post this->is_loaded() returns false.
    * \throw Nothing.
    */
    void unload() BOOST_NOEXCEPT {
    	_storage.clear();
        _lib.unload();
    }

    /*!
    * Check if an library is loaded.
    *
    * \return true if a library has been loaded.
    * \throw Nothing.
    */
    bool is_loaded() const BOOST_NOEXCEPT {
        return _lib.is_loaded();
    }

    /*!
    * Check if an library is not loaded.
    *
    * \return true if a library has not been loaded.
    * \throw Nothing.
    */
    bool operator!() const BOOST_NOEXCEPT {
        return !is_loaded();
    }

    /*!
    * Check if an library is loaded.
    *
    * \return true if a library has been loaded.
    * \throw Nothing.
    */
    BOOST_EXPLICIT_OPERATOR_BOOL()

    /*!
    * Search for a given symbol on loaded library. Works for all symbols, including alias names.
    *
    * \param symbol_name Null-terminated symbol name. Can handle std::string, char*, const char*.
    * \return `true` if the loaded library contains a symbol with a given name.
    * \throw Nothing.
    */
    bool has(const char* symbol_name) const BOOST_NOEXCEPT {
        return _lib.has(symbol_name);
    }

    //! \overload bool has(const char* symbol_name) const
    bool has(const std::string& symbol_name) const BOOST_NOEXCEPT {
        return _lib.has(symbol_name);
    }
    /*!
    * Makes *this share the same shared object as lib. If *this is loaded, then unloads it.
    *
    * \param lib A shared_library instance to share.
    * \post lib.location() == this->location()
    * \throw boost::system::system_error, std::bad_alloc in case of insufficient memory.
    */
    smart_library& assign(const smart_library& lib)
    {
       _lib.assign(lib._lib);
       _storage.assign(lib._storage);
       return *this;
    }
    /*!
    * Swaps two libraries. Does not invalidate existing symbols and functions loaded from libraries.
    *
    * \param rhs Library to swap with.
    * \throw Nothing.
    */
    void swap(smart_library& rhs) BOOST_NOEXCEPT
    {
        _lib.swap(rhs._lib);
        _storage.swap(rhs._storage);
    }
};

/// Very fast equality check that compares the actual DLL/DSO objects. Throws nothing.
inline bool operator==(const smart_library& lhs, const smart_library& rhs) BOOST_NOEXCEPT {
    return lhs.shared_lib().native() == rhs.shared_lib().native();
}

/// Very fast inequality check that compares the actual DLL/DSO objects. Throws nothing.
inline bool operator!=(const smart_library& lhs, const smart_library& rhs) BOOST_NOEXCEPT {
    return lhs.shared_lib().native() != rhs.shared_lib().native();
}

/// Compare the actual DLL/DSO objects without any guarantee to be stable between runs. Throws nothing.
inline bool operator<(const smart_library& lhs, const smart_library& rhs) BOOST_NOEXCEPT {
    return lhs.shared_lib().native() < rhs.shared_lib().native();
}

/// Swaps two shared libraries. Does not invalidate symbols and functions loaded from libraries. Throws nothing.
inline void swap(smart_library& lhs, smart_library& rhs) BOOST_NOEXCEPT {
    lhs.swap(rhs);
}


} /* namespace experimental */
} /* namespace dll */
} /* namespace boost */

#endif /* INCLUDE_BOOST_DLL_SMART_LIBRARY_HPP_ */
