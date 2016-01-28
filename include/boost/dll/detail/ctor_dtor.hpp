// Copyright 2016 Klemens Morgenstern
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef INCLUDE_BOOST_DLL_DETAIL_CTOR_DTOR_HPP_
#define INCLUDE_BOOST_DLL_DETAIL_CTOR_DTOR_HPP_

#include <boost/config.hpp>

#if defined(BOOST_MSVC) || defined(BOOST_MSVC_VER)
#   include <boost/dll/detail/demangling/msvc.hpp>
#else
#   include <boost/dll/detail/demangling/itanium.hpp>
#endif

namespace boost { namespace dll { namespace detail {

/*!
 * This class stores a constructor.
 *
 * In some compilers there are several constructors in code, which may include an allocating one.
 * This can be used if the imported class shall be put on the heap, which is why the class provied both types.
 */
template<typename Signature>
struct constructor;

template<typename Class, typename ...Args>
struct constructor<Class(Args...)> {
    typedef void(standard_t)(Class*, Args...);
    typedef Class*(allocating_t)(Args...);

    //! The standard, i.e. not allocating constructor.
    standard_t* standard;
    //! The allocating constructor.
    allocating_t* allocating;

    //! True if a allocating constructor could be loaded.
    bool has_allocating() const { return allocating != nullptr; }

    //! False if neither the allocating nor the standard constructor is available.
    bool is_empty() const { return !((allocating == nullptr) && (standard != nullptr)) ; }

    constructor() = delete;
    constructor(const constructor &) = default;

    explicit constructor(standard_t* standard, allocating_t* allocating = nullptr)
        : standard(standard)
        , allocating(allocating)
    {}
};

template <typename Class>
struct destructor {
#if defined(BOOST_MSVC) || defined(BOOST_MSVC_VER)
    typedef void(__thiscall& type)(Class* const);
#else
    typedef void(type)(Class* const);
#endif
    //! The standard, i.e. not deleting destructor.
    type* standard;
    //! The deleting destructor.
    type* deleting;

    //! True if a deleting destructor could be loaded.
    bool has_deleting() const { return deleting != nullptr; }

    //! False if neither the deleting nor the standard destructor is available.
    bool is_empty() const { return !((deleting == nullptr) && (standard != nullptr)) ; }
    destructor() = delete;

    //! Copy destructor.
    destructor(const destructor &) = default;

    //! Construct it from both the standard destructor and the allocating destructor
    explicit destructor(const type* standard, const type* deleting = nullptr)
        : standard(standard)
        , deleting(deleting)
    {}
};

#if defined(BOOST_MSVC) || defined(BOOST_MSVC_VER)
template<typename Signature, typename Lib>
constructor<Signature> load_ctor(Lib & lib, const mangled_storage_impl::ctor_sym & ct) {
    typedef typename constructor<Signature>::standard_t standard_t;
    return constructor<Signature>(
        &lib.template get<standard>(ct)
    );
}

template<typename Class, typename Lib>
destructor<Class> load_dtor(Lib & lib, const mangled_storage_impl::dtor_sym & dt) {
    typedef typename destructor<Class>::type f;
    return destructor<Class>(
        &lib.template get<f>(dt)
    );
}

#else

template<typename Signature, typename Lib>
constructor<Signature> load_ctor(Lib & lib, const mangled_storage_impl::ctor_sym & ct) {
    typedef typename constructor<Signature>::standard_t f;
    typedef typename constructor<Signature>::allocating_t p;

    return constructor<Signature>(
        (ct.C1.size() > 0 ? &lib.template get<f>(ct.C1) : nullptr), // normal constructor
        (ct.C2.size() > 0 ? &lib.template get<p>(ct.C2) : nullptr)  // allocating constructor
    );
}

template<typename Class, typename Lib>
destructor<Class> load_dtor(Lib & lib, const mangled_storage_impl::dtor_sym & dt) {
    typedef typename destructor<Class>::type f;

    return destructor<Class>(
        (dt.D1.size() > 0 ? &lib.template get<f>(dt.D1) : nullptr), // normal destructor
        (dt.D2.size() > 0 ? &lib.template get<f>(dt.D2) : nullptr)  // deallocating destructor
    );
}

#endif

}}} // namespace boost::dll::detail

#endif /* INCLUDE_BOOST_DLL_DETAIL_CTOR_DTOR_HPP_ */
