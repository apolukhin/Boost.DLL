// Copyright 2011-2012 Renato Tegon Forti
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/config.hpp>
#include <boost/plugin/alias.hpp>

extern "C" BOOST_SYMBOL_EXPORT int sumexe(int x, int y)
{
   return x+y;
}

