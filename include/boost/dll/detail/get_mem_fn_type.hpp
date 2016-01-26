/*
 * smart_library.hpp
 *
 *  Created on: 17.10.2015
 *      Author: Klemens
 */

#ifndef BOOST_DLL_SMART_LIBRARY_HPP_
#define BOOST_DLL_SMART_LIBRARY_HPP_

namespace boost
{
namespace dll
{

namespace detail
{

template<typename Class, typename Func>
struct get_mem_fn_type{};

template<typename Class, typename Return, typename ...Args>
struct get_mem_fn_type<Class, Return(Args...)>
{
    typedef Return (Class::*mem_fn)(Args...);
};

template<typename Class, typename Return, typename ...Args>
struct get_mem_fn_type<const Class, Return(Args...)>
{
    typedef Return (Class::*mem_fn)(Args...) const ;
};

template<typename Class, typename Return, typename ...Args>
struct get_mem_fn_type<volatile Class, Return(Args...)>
{
    typedef Return (Class::*mem_fn)(Args...) volatile;
};

template<typename Class, typename Return, typename ...Args>
struct get_mem_fn_type<const volatile Class, Return(Args...)>
{
    typedef Return (Class::*mem_fn)(Args...) const volatile ;
};

}



}
}


#endif /* INCLUDE_BOOST_DLL_SMART_LIBRARY_HPP_ */
