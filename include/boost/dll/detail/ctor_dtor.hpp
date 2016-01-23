/*
 * ctor_dtor.hpp
 *
 *  Created on: 19.10.2015
 *      Author: Klemens
 */

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

template<typename Signature>
struct constructor
{
	typedef typename detail::ctor_t<Signature>::standard   standard_t;
	typedef typename detail::ctor_t<Signature>::allocating allocating_t;

	const standard_t standard;
	const allocating_t allocating;

	bool has_allocating() const { return allocating != nullptr; }

	bool is_empty() const { return !allocating; }
	operator bool() const { return allocating; }
	constructor() = delete;
	constructor(const constructor &) = default;

	explicit constructor(const standard_t & standard)
				: standard(standard), allocating(nullptr) {}

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
	const type standard;
	const type deleting;

	bool has_deleting() const { return deleting != nullptr; }

	bool is_empty() const { return !deleting; }
	operator bool() const { return deleting; }
	destructor() = delete;
	destructor(const destructor &) = default;

	explicit destructor(const type & standard)
					: standard(standard), deleting(nullptr) {}

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
