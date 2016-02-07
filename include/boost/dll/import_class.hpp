// Copyright 2015-2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_DLL_IMPORT_CLASS_HPP_
#define BOOST_DLL_IMPORT_CLASS_HPP_

#include <boost/dll/smart_library.hpp>
#include <boost/dll/import_mangled.hpp>
#include <memory>


#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace dll { namespace experimental {

namespace detail
{

template<typename T>
struct deleter
{
    destructor<T> dtor;
    bool use_deleting;

    deleter(const destructor<T> & dtor, bool use_deleting = false) :
        dtor(dtor), use_deleting(use_deleting) {}

    void operator()(T*t)
    {
        if (use_deleting)
            dtor.call_deleting(t);
        else
        {
            dtor.call_standard(t);
            //the thing is actually an array, so delete[]
            auto p = reinterpret_cast<char*>(t);
            delete [] p;
        }
    }
};

template<class T, class = void>
struct mem_fn_call_proxy;

template<class Class, class U>
struct mem_fn_call_proxy<Class, boost::dll::experimental::detail::mangled_library_mem_fn<Class, U>>
{
    typedef boost::dll::experimental::detail::mangled_library_mem_fn<Class, U> mem_fn_t;
    Class* t;
    mem_fn_t & mem_fn;

    mem_fn_call_proxy(mem_fn_call_proxy&&) = default;
    mem_fn_call_proxy(const mem_fn_call_proxy & ) = delete;
    mem_fn_call_proxy(Class * t, mem_fn_t & mem_fn)
                : t(t), mem_fn(mem_fn) {}

    template<typename ...Args>
    auto operator()(Args&&...args) const
    {
        return mem_fn(t, std::forward<Args>(args)...);
    }

};

template<class T, class Return, class ...Args>
struct mem_fn_call_proxy<T, Return(Args...)>
{
    T* t;
    const std::string &name;
    smart_library &_lib;

    mem_fn_call_proxy(mem_fn_call_proxy&&) = default;
    mem_fn_call_proxy(const mem_fn_call_proxy&) = delete;
    mem_fn_call_proxy(T *t, const std::string &name, smart_library & _lib)
        : t(t), name(name), _lib(_lib) {};

    Return operator()(Args...args) const
    {
        auto f = _lib.get_mem_fn<T, Return(Args...)>(name);
        return (t->*f)(static_cast<Args>(args)...);
    }
};

}

template<typename T>
class imported_class;

template<typename T, typename ... Args> imported_class<T>
import_class(const boost::shared_ptr<smart_library>& lib, Args...args);
template<typename T, typename ... Args> imported_class<T>
import_class(const boost::shared_ptr<smart_library>& lib, const std::string & alias_name, Args...args);
template<typename T, typename ... Args> imported_class<T>
import_class(const boost::shared_ptr<smart_library>& lib, std::size_t size, Args...args);
template<typename T, typename ... Args> imported_class<T>
import_class(const boost::shared_ptr<smart_library>& lib, std::size_t size,
             const std::string & alias_name, Args...args);

template<typename T>
class imported_class
{
    boost::shared_ptr<smart_library> _lib;
    std::unique_ptr<T, detail::deleter<T>> _data;
    bool _is_allocating;
    std::size_t _size;
    const std::type_info& _ti;

    template<typename ... Args>
    inline std::unique_ptr<T, detail::deleter<T>> make_data(const boost::shared_ptr<smart_library>& lib, Args ... args);
    template<typename ... Args>
    inline std::unique_ptr<T, detail::deleter<T>> make_data(const boost::shared_ptr<smart_library>& lib, std::size_t size, Args...args);

    template<typename ...Args>
    imported_class(detail::sequence<Args...> *, const boost::shared_ptr<smart_library>& lib,  Args...args);

    template<typename ...Args>
    imported_class(detail::sequence<Args...> *, const boost::shared_ptr<smart_library>& lib, std::size_t size,  Args...args);
public:
    //alias to construct with explicit parameter list
    template<typename ...Args>
    static imported_class<T> make(const boost::shared_ptr<smart_library>& lib,  Args...args)
    {
        typedef detail::sequence<Args...> *seq;
        return imported_class(seq(), lib, static_cast<Args>(args)...);
    }

    template<typename ...Args>
    static imported_class<T> make(const boost::shared_ptr<smart_library>& lib, std::size_t size,  Args...args)
    {
        typedef detail::sequence<Args...> *seq;
        return imported_class(seq(), lib, size, static_cast<Args>(args)...);
    }

    typedef imported_class<T> base_t;
    T* get() {return _data.get();}
    imported_class() = delete;

    imported_class(imported_class&) = delete;
    imported_class(imported_class&&) = default;
    imported_class& operator=(imported_class&) = delete;
    imported_class& operator=(imported_class&&) = default;


#if (!defined(BOOST_MSVC) && !defined(BOOST_MSVC_FULL_VER))
    bool is_move_constructible() {return !_lib->symbol_storage().template get_constructor<T(T&&)>     ().empty();}
    bool is_move_assignable()    {return !_lib->symbol_storage().template get_mem_fn<T, T&(T&&)>     ("operator=").empty();}
#endif
    bool is_copy_constructible() {return !_lib->symbol_storage().template get_constructor<T(const T&)>().empty();}
    bool is_copy_assignable()    {return !_lib->symbol_storage().template get_mem_fn<T, T&(const T&)>("operator=").empty();}

    imported_class<T> copy();
    imported_class<T> move();

    void copy_assign(const imported_class<T> & lhs) const;
    void move_assign(      imported_class<T> & lhs);

    explicit operator bool() const  {return _data;}

    std::type_info& get_type_info() {return _ti;};

    template<class Signature>
    const detail::mem_fn_call_proxy<T, Signature> call(const std::string& name)
    {
        return detail::mem_fn_call_proxy<T, Signature>(_data.get(), name, *_lib);
    }
    //overload for const/volatile functions.
    template<class Tin, class Signature, class = boost::enable_if<detail::unqalified_is_same<T, Tin>>>
    const detail::mem_fn_call_proxy<Tin, Signature> call(const std::string& name)
    {
        return detail::mem_fn_call_proxy<Tin, Signature>(_data.get(), name, *_lib);
    }

    template<class Tin, class T2>
    const detail::mem_fn_call_proxy<Tin, boost::dll::experimental::detail::mangled_library_mem_fn<Tin, T2>>
            operator->*(detail::mangled_library_mem_fn<Tin, T2>& mn)
    {
        return detail::mem_fn_call_proxy<Tin, boost::dll::experimental::detail::mangled_library_mem_fn<Tin, T2>>(_data.get(), mn);
    }


    template <class ...Args>
    typename boost::dll::experimental::detail::mangled_import_type<boost::dll::experimental::detail::sequence<T, Args...>>::type
    import(const std::string & name)
    {
        return boost::dll::experimental::import_mangled<T, Args...>(_lib, name);
    }
};



//helper function, uses the allocating
template<typename T>
template<typename ... Args>
inline std::unique_ptr<T, detail::deleter<T>> imported_class<T>::make_data(const boost::shared_ptr<smart_library>& lib, Args ... args)
{
    constructor<T(Args...)> ctor = lib->get_constructor<T(Args...)>();
    destructor<T>           dtor = lib->get_destructor <T>();
    if (!ctor.has_allocating() || !dtor.has_deleting())
    {
        boost::system::error_code ec;

        ec = boost::system::error_code(
            boost::system::errc::bad_file_descriptor,
            boost::system::generic_category()
        );

        // report_error() calls dlsym, do not use it here!
        boost::throw_exception(
            boost::system::system_error(
                ec, "boost::dll::detail::make_data() failed: no allocating ctor or dtor was found"
            )
        );
    }

     return std::unique_ptr<T, detail::deleter<T>> (
            ctor.call_allocating(static_cast<Args>(args)...),
            detail::deleter<T>(dtor, false /* not deleting dtor*/));
}

//helper function, using the standard
template<typename T>
template<typename ... Args>
inline std::unique_ptr<T, detail::deleter<T>> imported_class<T>::make_data(const boost::shared_ptr<smart_library>& lib, std::size_t size, Args...args)
{
    constructor<T(Args...)> ctor = lib->get_constructor<T(Args...)>();
    destructor<T>           dtor = lib->get_destructor <T>();

    if (!ctor.has_standard() || !dtor.has_standard())
    {
        boost::system::error_code ec;

        ec = boost::system::error_code(
            boost::system::errc::bad_file_descriptor,
            boost::system::generic_category()
        );

        // report_error() calls dlsym, do not use it here!
        boost::throw_exception(
            boost::system::system_error(
                ec, "boost::dll::detail::make_data() failed: no regular ctor or dtor was found"
            )
        );
    }

    T *data = reinterpret_cast<T*>(new char[size]);

    ctor.call_standard(data, static_cast<Args>(args)...);

    return std::unique_ptr<T, detail::deleter<T>> (
            reinterpret_cast<T*>(data),
            detail::deleter<T>(dtor, false /* not deleting dtor*/));

}


template<typename T>
template<typename ...Args>
imported_class<T>::imported_class(detail::sequence<Args...> *, const boost::shared_ptr<smart_library> & lib,  Args...args)
    : _lib(lib),
      _data(make_data<Args...>(lib, static_cast<Args>(args)...)),
      _is_allocating(false),
      _size(0),
      _ti(lib->get_type_info<T>())
{

}

template<typename T>
template<typename ...Args>
imported_class<T>::imported_class(detail::sequence<Args...> *, const boost::shared_ptr<smart_library> & lib, std::size_t size,  Args...args)
    : _lib(lib),
      _data(make_data<Args...>(lib, size, static_cast<Args>(args)...)),
      _is_allocating(true),
      _size(size),
      _ti(lib->get_type_info<T>())
{

}

template<typename T>
inline imported_class<T> boost::dll::experimental::imported_class<T>::copy()
{
    if (this->_is_allocating)
        return imported_class<T>::template make<const T&>(_lib, *_data);
    else
        return imported_class<T>::template make<const T&>(_lib, _size, *_data);
}

template<typename T>
inline imported_class<T> boost::dll::experimental::imported_class<T>::move()
{
    if (this->_is_allocating)
        return imported_class<T>::template make<T&&>(_lib, *_data);
    else
        return imported_class<T>::template make<T&&>(_lib, _size, *_data);
}

template<typename T>
inline void boost::dll::experimental::imported_class<T>::copy_assign(const imported_class<T>& lhs) const
{
    this->call<T&(const T&)>("operator=")(*lhs._data);
}

template<typename T>
inline void boost::dll::experimental::imported_class<T>::move_assign(imported_class<T>& lhs)
{
    this->call<T&(T&&)>("operator=")(static_cast<T&&>(*lhs._data));
}

template<typename T, typename ... Args> imported_class<T>
import_class(const boost::shared_ptr<smart_library>& lib, Args...args)
{
    return imported_class<T>::template make<Args...>(lib, static_cast<Args>(args)...);
}
template<typename T, typename ... Args> imported_class<T>
import_class(const boost::shared_ptr<smart_library>& lib, const std::string & alias_name, Args...args)
{
    lib->add_type_alias<T>(alias_name);
    return imported_class<T>::template make<Args...>(lib, static_cast<Args>(args)...);
}
template<typename T, typename ... Args> imported_class<T>
import_class(const boost::shared_ptr<smart_library>& lib, std::size_t size, Args...args)
{
    return imported_class<T>::template make<Args...>(lib, size, static_cast<Args>(args)...);
}
template<typename T, typename ... Args> imported_class<T>
import_class(const boost::shared_ptr<smart_library>& lib, std::size_t size, const std::string & alias_name, Args...args)
{
    lib->add_type_alias<T>(alias_name);
    return imported_class<T>::template make<Args...>(lib, size, static_cast<Args>(args)...);
}

template<typename T, typename ... Args> imported_class<T>
import_class(const boost::shared_ptr<smart_library>& lib, const std::string & alias_name, std::size_t size, Args...args)
{
    lib->add_type_alias<T>(alias_name);
    return imported_class<T>::template make<Args...>(lib, size, static_cast<Args>(args)...);
}


}
}
}



#endif /* INCLUDE_BOOST_DLL_IMPORT_CLASS_HPP_ */
