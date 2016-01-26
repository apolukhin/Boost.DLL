// Copyright 2016 Klemens Morgenstern
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef INCLUDE_BOOST_DLL_DETAIL_CTOR_DTOR_HPP_
#define INCLUDE_BOOST_DLL_DETAIL_CTOR_DTOR_HPP_

#include <boost/config.hpp>
#include <boost/dll/detail/mem_fn_cast.hpp>

#if defined(BOOST_MSVC) || defined(BOOST_MSVC_VER)
#include <boost/dll/detail/demangling/msvc.hpp>
#else
#include <boost/dll/detail/demangling/itanium.hpp>
#endif

namespace boost
{
namespace dll
{

namespace detail
{

template<typename Signature>
struct ctor_t {};

template<typename Class, typename ...Args>
struct ctor_t<Class(Args...)>
{
    typedef void(Class::*standard)(Args...) ;
    typedef Class*(*allocating)(Args...) ;
};

}
/*!
 * This class stores a constructor.
 *
 * In some compilers there are several constructors in code, which may include an allocating one.
 * This can be used if the imported class shall be put on the heap, which is why the class provied both types.
 */
template<typename Signature>
struct constructor
{
    typedef typename detail::ctor_t<Signature>::standard   standard_t;
    typedef typename detail::ctor_t<Signature>::allocating allocating_t;
    //! The standard, i.e. not allocating constructor.
    const standard_t standard;
    //! The allocating constructor.
    const allocating_t allocating;

    //! True if a allocating constructor could be loaded.
    bool has_allocating() const { return allocating != nullptr; }

    //! False if neither the allocating nor the standard constructor is available.
    bool is_empty() const { return !((allocating == nullptr) && (standard != nullptr)) ; }
    //! False if either the allocating nor the standard constructor is available.
    operator bool() const { return (allocating != nullptr) || (standard != nullptr); }
    constructor() = delete;
    //! Copy Constructor.
    constructor(const constructor &) = default;

    //! Construct it from only the standard constructor
    explicit constructor(const standard_t & standard)
                : standard(standard), allocating(nullptr) {}
    //! Construct it from both the standard constructor and the allocating constructor
    constructor(const standard_t & standard, const allocating_t & allocating )
                : standard(standard), allocating(allocating) {}

};

template<typename Class>
struct destructor
{
#if defined(BOOST_MSVC) || defined(BOOST_MSVC_VER)
    typedef void(__thiscall * type)(Class* const);
#else
    typedef void(*type)(Class* const);
#endif
    //! The standard, i.e. not deleting destructor.
    const type standard;
    //! The deleting destructor.
    const type deleting;

    //! True if a deleting destructor could be loaded.
    bool has_deleting() const { return deleting != nullptr; }

    //! False if neither the deleting nor the standard destructor is available.
    bool is_empty() const { return !((deleting == nullptr) && (standard != nullptr)) ; }
    //! False if either the deleting nor the standard destructor is available.
    operator bool() const { return (deleting != nullptr) || (standard != nullptr); }
    destructor() = delete;

    //! Copy destructor.
    destructor(const destructor &) = default;

    //! Construct it from only the standard destructor
    explicit destructor(const type & standard)
                    : standard(standard), deleting(nullptr) {}

    //! Construct it from both the standard destructor and the allocating destructor
    destructor(const type & standard, const type & deleting)
                : standard(standard), deleting(deleting) {}

};

namespace detail
{
#if defined(BOOST_MSVC) || defined(BOOST_MSVC_VER)
template<typename Signature, typename Lib>
constructor<Signature> load_ctor(Lib & lib, const mangled_storage_impl::ctor_sym & ct)
{
    typedef typename detail::ctor_t<Signature>::standard standard;

    standard s = detail::mem_fn_cast<standard>(lib.get_void(ct));

    return constructor<Signature>(s);
}

template<typename Class, typename Lib>
destructor<Class> load_dtor(Lib & lib, const mangled_storage_impl::dtor_sym & dt)
{
    using f = typename destructor<Class>::type;

    f s = detail::mem_fn_cast<f>(lib.get_void(dt));

    return destructor<Class>(s);
}

#else
template<typename Signature, typename Lib>
constructor<Signature> load_ctor(Lib & lib, const mangled_storage_impl::ctor_sym & ct)
{
    typedef typename ctor_t<Signature>::standard f;
    typedef typename ctor_t<Signature>::allocating p;
    /* father Ctor */
    f C0 = nullptr;
    /* normal Ctor */
    f C1 = nullptr;
    /* allocating constructor */
    p C2 = nullptr;

    if (ct.C0.size() > 0)
        C0 =  detail::mem_fn_cast<f>(lib.get_void(ct.C0));
    if (ct.C1.size() > 0)
        C1 =  detail::mem_fn_cast<f>(lib.get_void(ct.C1));
    if (ct.C2.size() > 0)
        C2 =  detail::mem_fn_cast<p>(lib.get_void(ct.C2));

    return {C1,C2};
}

template<typename Class, typename Lib>
destructor<Class> load_dtor(Lib & lib, const mangled_storage_impl::dtor_sym & dt)
{
    typedef void(*f)(Class* const);

    /* father Dtor */
    f D0 = nullptr;
    /* normal Dtor */
    f D1 = nullptr;
    /* allocating constructor */
    f D2 = nullptr;

    if (dt.D0.size() > 0)
        D0 =  detail::mem_fn_cast<f>(lib.get_void(dt.D0));
    if (dt.D1.size() > 0)
        D1 =  detail::mem_fn_cast<f>(lib.get_void(dt.D1));
    if (dt.D2.size() > 0)
        D2 =  detail::mem_fn_cast<f>(lib.get_void(dt.D2));

    return {D1, D2};

}



#endif

}
}
}



#endif /* INCLUDE_BOOST_DLL_DETAIL_CTOR_DTOR_HPP_ */
