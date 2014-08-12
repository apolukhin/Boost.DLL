// Copyright 2011-2012 Renato Tegon Forti.
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// -------------------------------------------------------------------------------------
// This example shows how to use load_self to load symbols direct on executable
// -------------------------------------------------------------------------------------

#include <boost/plugin.hpp>
#include <iostream>

extern "C" BOOST_SYMBOL_EXPORT int sumexe(int x, int y)
{
   return x+y;
}

int main(int argc, char* argv[]) { 
    std::cout << "Application started" << std::endl;
    
    boost::plugin::shared_library self;
    self.load_self();

    if(self.search_symbol("sumexe"))
    {
       std::cout << "Call function 'int sumexe(int x, int y)'" << std::endl;
       boost::function<int(int, int)> sum = self.get<int(int, int)>("sumexe");
       std::cout << "Computed Value: " << sum(2,2) << std::endl;
    }
}

