// refccountable.hpp --------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2014 Renato Tegon Forti, Antony Polukhin.

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 06-08-2014 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_PLUGIN_REFCOUNTABLE_HPP
#define BOOST_PLUGIN_REFCOUNTABLE_HPP

#include <boost/config.hpp>
#include <boost/plugin/shared_library.hpp>
#include <boost/plugin/alias.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>

namespace boost { namespace plugin {

namespace detail {

    template <class Result>
    class refc_function {
        boost::shared_ptr<shared_library>   lib_;
        Result*                             func_ptr_;

    public:
        refc_function(const boost::shared_ptr<shared_library>& lib, Result* func_ptr)
            : lib_(lib)
            , func_ptr_(func_ptr)
        {}

        operator Result*() const BOOST_NOEXCEPT {
            return func_ptr_;
        }
    };


    struct ptr_holding_empty_deleter {
        boost::shared_ptr<shared_library> lib_;

        explicit ptr_holding_empty_deleter(const boost::shared_ptr<shared_library>& lib)
            : lib_(lib)
        {}

        inline void operator()(const void*) const BOOST_NOEXCEPT { /*do notjing*/ }
    };
}


// shared_function methods
template <class Result>
boost::function<Result> shared_function(const boost::shared_ptr<shared_library>& lib, boost::string_ref func_name) {
    return boost::plugin::detail::refc_function<Result>(lib, &lib->get<Result>(func_name));
}

template <class Result>
boost::function<Result> shared_function(const boost::filesystem::path& lib_path, boost::string_ref func_name) {
    return shared_function<Result>(
        boost::make_shared<shared_library>(lib_path),
        func_name
    );
}


// shared_function_alias methods
template <class Result>
boost::function<Result> shared_function_alias(const boost::shared_ptr<shared_library>& lib, boost::string_ref func_name) {
    return boost::plugin::detail::refc_function<Result>(lib, lib->get<Result*>(func_name));
}

template <class Result>
boost::function<Result> shared_function_alias(const boost::filesystem::path& lib_path, boost::string_ref func_name) {
    return shared_function_alias<Result>(
        boost::make_shared<shared_library>(lib_path),
        func_name
    );
}

// shared_variable methods
template <class Result>
boost::shared_ptr<Result> shared_variable(const boost::shared_ptr<shared_library>& lib, boost::string_ref variable_name) {
    return boost::shared_ptr<Result>(&lib->get<Result>(variable_name), detail::ptr_holding_empty_deleter(lib));
}

template <class Result>
boost::shared_ptr<Result> shared_variable(const boost::filesystem::path& lib_path, boost::string_ref variable_name) {
    return shared_variable<Result>(
        boost::make_shared<shared_library>(lib_path),
        variable_name
    );
}


// shared_variable_alias methods
template <class Result>
boost::shared_ptr<Result> shared_variable_alias(const boost::shared_ptr<shared_library>& lib, boost::string_ref variable_name) {
    return boost::shared_ptr<Result>(lib->get<Result*>(variable_name), detail::ptr_holding_empty_deleter(lib));
}

template <class Result>
boost::shared_ptr<Result> shared_variable_alias(const boost::filesystem::path& lib_path, boost::string_ref variable_name) {
    return shared_variable_alias<Result>(
        boost::make_shared<shared_library>(lib_path),
        variable_name
    );
}

}} // boost::plugin

#endif // BOOST_PLUGIN_REFCOUNTABLE_HPP

