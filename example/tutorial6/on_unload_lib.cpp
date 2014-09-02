// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

//[plugcpp_on_unload

#include <boost/dll.hpp>
#include <boost/function.hpp>
#include <vector>

namespace my_namespace {

struct on_unload {
    typedef boost::function<void()> callback_t;
    typedef on_unload this_type;

    ~on_unload() {
        for (std::size_t i = 0; i < callbacks_.size(); ++i) {
            callback_t& function = callbacks_[i];
            function();
        }
    }

    static void add(const callback_t& function) { // not thread safe
        static this_type instance;
        instance.callbacks_.push_back(function);
    }

private:
    std::vector<callback_t> callbacks_;
    on_unload() {} // prohibit construction outside of `add` function
};


BOOST_DLL_ALIAS(
    my_namespace::on_unload::add,
    on_unload
)

} // namespace my_namespace

//]



