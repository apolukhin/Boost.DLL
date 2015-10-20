/*
 * ctor_dtor.hpp
 *
 *  Created on: 19.10.2015
 *      Author: Klemens
 */

#ifndef INCLUDE_BOOST_DLL_DETAIL_CTOR_DTOR_HPP_
#define INCLUDE_BOOST_DLL_DETAIL_CTOR_DTOR_HPP_

#include <boost/config.hpp>
#include <boost/function.hpp>
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
	typedef void(Class::*mem_fn)(Args...) ;
	typedef Class*(*pln_fn)(Args...) ;
	typedef boost::function<void(Class * const, Args...)> standard;
	typedef boost::function<Class * (Args...)> allocating;

};

}

template<typename Signature>
struct constructor
{
	typedef BOOST_DEDUCED_TYPENAME detail::ctor_t<Signature>::standard   standard_t;
	typedef BOOST_DEDUCED_TYPENAME detail::ctor_t<Signature>::allocating allocating_t;

	const standard_t standard;
	const allocating_t allocating;

	bool has_allocating() const { return allocating; }

	bool is_empty() const { return !allocating; }
	operator bool() const { return allocating; }
	constructor() = delete;
	constructor(const constructor &) = default;

	constructor(const standard_t & standard, const allocating_t & allocating = nullptr)
				: standard(standard), allocating(allocating) {}

};

template<typename Class>
struct destructor
{
	typedef boost::function<void(Class*)> type;

	const type standard;
	const type deleting;

	bool has_deleting() const { return deleting; }

	bool is_empty() const { return !deleting; }
	operator bool() const { return deleting; }
	destructor() = delete;
	destructor(const destructor &) = default;

	destructor(const type & standard, const type & deleting = nullptr)
				: standard(standard), deleting(deleting) {}

};

namespace detail
{
#if defined(BOOST_MSVC) || defined(BOOST_MSVC_VER)
template<typename Signature, typename Lib>
constructor<Signature> load_ctor(Lib & lib, const mangled_storage_impl::ctor_sym & ct)
{
	typedef BOOST_DEDUCED_TYPENAME ctor_t<Signature>::mem_fn f;
	typedef BOOST_DEDUCED_TYPENAME detail::ctor_t<Signature>::standard standard;

	standard s = detail::mem_fn_cast<f>(lib.get_void(ct.C0));

	return {s};
}

template<typename Signature, typename Lib>
destructor<Signature> load_dtor(Lib & lib, const mangled_storage_impl::dtor_sym & dt)
{
	typedef void(*f)(Class* const);
	typedef boost::function<void(Class* const)> fn;

	fn s = detail::mem_fn_cast<f>(lib.get_void(dt.D0));

	return {s};
}

#else
template<typename Signature, typename Lib>
constructor<Signature> load_ctor(Lib & lib, const mangled_storage_impl::ctor_sym & ct)
{
	typedef BOOST_DEDUCED_TYPENAME ctor_t<Signature>::mem_fn f;
	typedef BOOST_DEDUCED_TYPENAME ctor_t<Signature>::pln_fn p;
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


	typedef BOOST_DEDUCED_TYPENAME detail::ctor_t<Signature>::standard standard;
	typedef BOOST_DEDUCED_TYPENAME detail::ctor_t<Signature>::allocating allocating;

	standard s;
	if (C1)
		s = C1;

	allocating a;

	if (C2)
		a = C2;

	return {s,a};
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
		D0 =  detail::mem_fn_cast<f>(lib.get(dt.D0));
	if (dt.D1.size() > 0)
		D1 =  detail::mem_fn_cast<f>(lib.get(dt.D1));
	if (dt.D1.size() > 0)
		D2 =  detail::mem_fn_cast<f>(lib.get(dt.D2));

	typedef boost::function<void(Class* const)> fn;
	fn s;
	if (D1)
		s = D1;
	fn a;
	if (D2)
		a = D2;

	return {s,a};

}



#endif

}
}
}



#endif /* INCLUDE_BOOST_DLL_DETAIL_CTOR_DTOR_HPP_ */
