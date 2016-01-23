/*
 * smart_library.hpp
 *
 *  Created on: 17.10.2015
 *      Author: Klemens
 */

#ifndef INCLUDE_BOOST_DLL_SMART_LIBRARY_HPP_
#define INCLUDE_BOOST_DLL_SMART_LIBRARY_HPP_

#include <boost/dll/shared_library.hpp>
#include <boost/dll/mangled_storage.hpp>
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

class smart_library : public detail::mangled_storage_impl
{
	mangled_storage _storage;
public:
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
        load(lib_path, mode);
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
		: shared_library(boost::move(static_cast<shared_library&>(lib))), _storage(boost::move(lib._storage))
	{}
	/*!
    * Destroys the smart_library.
    * `unload()` is called if the DLL/DSO was loaded. If library was loaded multiple times
    * by different instances of shared_library, the actual DLL/DSO won't be unloaded until
    * there is at least one instance of shared_library.
    *
    * \throw Nothing.
    */
	virtual ~smart_library() BOOST_NOEXCEPT {};

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
	virtual void load(const boost::filesystem::path& lib_path, load_mode::type mode = load_mode::default_mode) override {
        boost::system::error_code ec;
        _storage.load(lib_path);
        shared_library::load(lib_path, mode, ec);

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
    virtual void load(const boost::filesystem::path& lib_path, boost::system::error_code& ec, load_mode::type mode = load_mode::default_mode) override {
        ec.clear();
        _storage.load(lib_path);
        shared_library::load(lib_path, mode, ec);
    }

    //! \overload void load(const boost::filesystem::path& lib_path, boost::system::error_code& ec, load_mode::type mode = load_mode::default_mode)
    virtual void load(const boost::filesystem::path& lib_path, load_mode::type mode, boost::system::error_code& ec) override {
        ec.clear();
        _storage.load(lib_path);
        shared_library::load(lib_path, mode, ec);
    }

	template<typename T>
	T& get_variable(const std::string &name)
	{
		return get<T>(_storage.get_variable<T>(name));
	}

	template<typename Func>
	Func& get_function(const std::string &name)
	{
		return get<Func>(_storage.get_function<Func>(name));
	}

	template<typename Class, typename Func>
	typename detail::get_mem_fn_type<Class, Func>::mem_fn
			get_mem_fn(const std::string &name)
	{
		return detail::mem_fn_cast<typename detail::get_mem_fn_type<Class, Func>::mem_fn>
				(get_void(_storage.get_mem_fn<Class, Func>(name)));
	}

	template<typename Signature>
	constructor<Signature> get_constructor()
	{
		return detail::load_ctor<Signature>(*this, _storage.get_constructor<Signature>());
	}

	template<typename Class>
	destructor<Class> get_destructor()
	{
		return detail::load_dtor<Class>(*this, _storage.get_destructor<Class>());

	}
	template<typename Alias> void add_type_alias(const std::string& name)
	{
		this->_storage.add_alias<Alias>(name);
	}
};

} /* namespace dll */
} /* namespace boost */

#endif /* INCLUDE_BOOST_DLL_SMART_LIBRARY_HPP_ */
