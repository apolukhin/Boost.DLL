//  demangle.hpp  --------------------------------------------------------------//

//  Copyright 2015 Klemens Morgenstern

// This file is put into the boost namespace because demangling is useful anyhow.

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/config.hpp>
#include <string>

#ifndef INCLUDE_BOOST_DEMANGLE_HPP_
#define INCLUDE_BOOST_DEMANGLE_HPP_



#if defined(BOOST_MSVC) || defined(BOOST_MSVC_FULL_VER)
#include <boost/detail/winapi/dbghelp.hpp>

namespace boost
{

inline std::string demangle(const std::string& mangled_name)
{
	return demangle(mangled_name.c_str());
}
inline std::string demangle(const char *mangled_name)
{
	char unmangled_name[512];

	UnDecorateSymbolName(mangled_name, unmangled_name, 512, UNDNAME_COMPLETE);

	return std::string(unmangled_name);
}
}
#elif defined(BOOST_GCC) || defined(BOOST_INTEL) || defined(BOOST_CLANG)
namespace boost
{
#include <cxxabi.h>
inline std::string demangle(const char *mangled_name)
{
	int status;
	char *ret = abi::__cxa_demangle(mangled_name, 0, 0, &status);

	std::string retval;
	if (status)
		retval = "";
	else
		retval = ret;
	delete ret;
	return retval;
};

inline std::string demangle(const std::string &mangled_name)
{
	return demangle(mangled_name.c_str());
}
}
#endif


#endif /* INCLUDE_BOOST_DEMANGLE_HPP_ */
