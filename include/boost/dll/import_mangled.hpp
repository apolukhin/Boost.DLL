// Copyright 2015-2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_DLL_IMPORT_MANGLED_HPP_
#define BOOST_DLL_IMPORT_MANGLED_HPP_

#include <boost/config.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_object.hpp>
#include <boost/make_shared.hpp>
#include <boost/utility/addressof.hpp>
#include <boost/move/move.hpp>
#include <boost/dll/smart_library.hpp>
#include <boost/type_traits/conditional.hpp>
#include <boost/dll/detail/import_mangled_helpers.hpp>



#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace dll { namespace experimental {

namespace detail
{



template <class ... Ts>
class mangled_library_function {
    // Copying of `boost::dll::shared_library` is very expensive, so we use a `shared_ptr` to make it faster.
    boost::shared_ptr<function_tuple<Ts...>>   f_;

public:
    inline mangled_library_function(const boost::shared_ptr<smart_library>& lib, Ts*... func_ptr) BOOST_NOEXCEPT
        : f_(lib, function_tuple<Ts...>(func_ptr...))
    {}


    // Compilation error at this point means that imported function
    // was called with unmatching parameters.
    //
    // Example:
    // auto f = dll::import_mangled<void(int), void(double)>("function", "lib.so");
    // f("Hello");  // error: invalid conversion from 'const char*' to 'int'
    // f(1, 2);     // error: too many arguments to function
    // f();         // error: too few arguments to function
    template <class... Args>
    inline auto operator()(Args&&... args) const
        -> decltype( (*f_)(static_cast<Args&&>(args)...) )
    {
        return (*f_)(static_cast<Args&&>(args)...);
    }
};



template <class ... Ts>
class mangled_library_mem_fn {
    // Copying of `boost::dll::shared_library` is very expensive, so we use a `shared_ptr` to make it faster.
    typedef mem_fn_tuple<Ts...> call_tuple_t;
    typedef mem_fn_call_proxy<Ts...> call_proxy_t;
    boost::shared_ptr<call_tuple_t>   f_;

public:
    inline mangled_library_mem_fn(const boost::shared_ptr<smart_library>& lib,
                                  Ts... func_ptr) BOOST_NOEXCEPT
        : f_(lib, call_tuple_t(func_ptr...))
    {}

 /*   template <class ClassIn, class... Args>
    inline auto operator()(ClassIn *cl, Args&&... args,
            typename boost::enable_if<boost::is_same<typename boost::remove_cv<ClassIn>, Class>>::type * = nullptr
                    ) const -> decltype( (*f_)(cl, static_cast<Args&&>(args)...) )
    {
        return (*f_)(cl, static_cast<Args&&>(args)...);
    }
    template<class ClassIn, class = typename boost::enable_if<boost::is_same<typename boost::remove_cv<ClassIn>, Class>>::type>
    inline call_proxy_t operator->*(ClassIn* cl)
    {
        return mem_fn_call_proxy<ClassIn, call_tuple_t>(cl, *f_);
    }*/
};




// simple enough to be here
template<class Seq>  struct is_variable : boost::false_type {};
template<typename T> struct is_variable<sequence<T>> : boost::is_object<T> {};

template <class Sequence,
          bool isFunction = is_function_seq<Sequence>::value,
          bool isMemFn    = is_mem_fn_seq  <Sequence>::value,
          bool isVariable = is_variable    <Sequence>::value>
struct mangled_import_type;

template <class ...Args>
struct mangled_import_type<sequence<Args...>, true,false,false> //is function
{
    typedef boost::dll::experimental::detail::mangled_library_function<Args...> type;
    static type make(
           const boost::shared_ptr<boost::dll::experimental::smart_library>& p,
           const std::string& name)
    {
        return type(p, boost::addressof(p->get_function<Args>(name))...);
    }
};

template <class Class, class ...Args>
struct mangled_import_type<sequence<Class, Args...>, false, true, false> //is member-function
{
    typedef boost::dll::experimental::detail::mangled_library_mem_fn<Args...> type;
    typedef typename boost::dll::experimental::detail::make_mem_fn_seq<Args...>::type actual_sequence;


    template<class ... ArgsIn>
    static type make_impl(
            const boost::shared_ptr<boost::dll::experimental::smart_library>& p,
            const std::string & name,
            sequence<ArgsIn...> * )
    {
        type(p, boost::addressof(p->get_mem_fn<ArgsIn::class_type, ArgsIn::func_type>(name))...);
    }

    static type make(
           const boost::shared_ptr<boost::dll::shared_library>& p,
           const std::string& name)
    {
        return make_impl(p, name, static_cast<actual_sequence*>(nullptr));
    }

};

template <class T>
struct mangled_import_type<sequence<T>, false, false, true> //is variable
{
    typedef boost::shared_ptr<T> type;

    static type make(
           const boost::shared_ptr<boost::dll::experimental::smart_library>& p,
           const std::string& name)
    {
        return type(p, boost::addressof(p->get_variable<T>(name)));
    }

};


} // namespace detail


#ifndef BOOST_DLL_DOXYGEN
#   define BOOST_DLL_MANGLED_IMPORT_RESULT_TYPE inline typename \
    boost::dll::experimental::detail::mangled_import_type<detail::sequence<Args...>>::type
#endif

/*
 * Variants:
 * import_mangled<int>("Stuff");
 * import_mangled<thingy(xyz)>("Function");
 * import mangled<thingy, void(int)>("Function");
 */

template <class ...Args>
BOOST_DLL_MANGLED_IMPORT_RESULT_TYPE import_mangled(const boost::filesystem::path& lib, const char* name,
    load_mode::type mode = load_mode::default_mode)
{
    typedef typename boost::dll::experimental::detail::mangled_import_type<
                     boost::dll::experimental::detail::sequence<Args...>> type;

    boost::shared_ptr<boost::dll::experimental::smart_library> p =
             boost::make_shared<boost::dll::experimental::smart_library>(lib, mode);
    //the load
    return type::make(p, name);
}



//! \overload boost::dll::import(const boost::filesystem::path& lib, const char* name, load_mode::type mode)
template <class ...Args>
BOOST_DLL_MANGLED_IMPORT_RESULT_TYPE import_mangled(const boost::filesystem::path& lib, const std::string& name,
    load_mode::type mode = load_mode::default_mode)
{
    return import_mangled<Args...>(lib, name.c_str(), mode);
}

//! \overload boost::dll::import(const boost::filesystem::path& lib, const char* name, load_mode::type mode)
template <class ...Args>
BOOST_DLL_MANGLED_IMPORT_RESULT_TYPE import_mangled(const smart_library& lib, const char* name) {
    typedef typename boost::dll::experimental::detail::mangled_import_type<detail::sequence<Args...>> type;

    boost::shared_ptr<boost::dll::experimental::smart_library> p = boost::make_shared<boost::dll::experimental::smart_library>(lib);
    return type::make(p, name);
}

//! \overload boost::dll::import(const boost::filesystem::path& lib, const char* name, load_mode::type mode)
template <class ...Args>
BOOST_DLL_MANGLED_IMPORT_RESULT_TYPE import_mangled(const smart_library& lib, const std::string& name) {
    return import_mangled<Args...>(lib, name.c_str());
}

//! \overload boost::dll::import(const boost::filesystem::path& lib, const char* name, load_mode::type mode)
template <class ...Args>
BOOST_DLL_MANGLED_IMPORT_RESULT_TYPE import_mangled(BOOST_RV_REF(smart_library) lib, const char* name) {
    typedef typename boost::dll::experimental::detail::mangled_import_type<detail::sequence<Args...>> type;

    boost::shared_ptr<boost::dll::experimental::smart_library> p = boost::make_shared<boost::dll::experimental::smart_library>(
        boost::move(lib)
    );
    return type::make(p, name);
}

//! \overload boost::dll::import(const boost::filesystem::path& lib, const char* name, load_mode::type mode)
template <class ...Args>
BOOST_DLL_MANGLED_IMPORT_RESULT_TYPE import_mangled(BOOST_RV_REF(smart_library) lib, const std::string& name) {
    return import_mangled<Args...>(boost::move(lib), name.c_str());
}


#undef BOOST_DLL_MANGLED_IMPORT_RESULT_TYPE

}}}


#endif /* INCLUDE_BOOST_DLL_IMPORT_MANGLED_HPP_ */
