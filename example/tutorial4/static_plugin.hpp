// Copyright 2011-2012 Renato Tegon Forti
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/config.hpp>
#include <boost/plugin/alias.hpp>

// Forward declaration
int sumexe(int x, int y);

#if !defined(SUMEXE_PLUGIN_STATIC) || defined(SUMEXE_PLUGIN_LINK_IN)
    BOOST_PLUGIN_ALIAS(sumexe, sumexe_alias)
#endif



/*
// DLL/DSO with load time linkage
#if !defined(SUMEXE_PLUGIN_STATIC)
#   ifdef SUMEXE_PLUGIN_EXPORTS
#       define SUMEXE_PLUGIN_API BOOST_SYMBOL_EXPORT
#   else
#       define SUMEXE_PLUGIN_API BOOST_SYMBOL_IMPORT
#   endif
#else
#   define SUMEXE_PLUGIN_API
#endif

SUMEXE_PLUGIN_API int sumexe(int x, int y);

#if !defined(SUMEXE_PLUGIN_STATIC) || defined(SUMEXE_LINK_IN)
    BOOST_PLUGIN_ALIAS(sumexe, sumexe_alias)
#endif
*/