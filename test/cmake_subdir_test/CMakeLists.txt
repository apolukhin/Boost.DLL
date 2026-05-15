# Copyright 2026 Fedor Osetrov
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5...4.20)

project(cmake_subdir_test LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(deps

atomic
align
assert
config
core
predef
system
throw_exception
type_index
winapi

compat
container_hash
describe
variant
variant2
integer
mp11
fusion
function_types
detail
preprocessor
mpl
tuple
type_traits
static_assert
utility
io
typeof
functional
function
bind
filesystem
iterator
concept_check
optional
smart_ptr
scope

)

foreach(dep IN LISTS deps)

  add_subdirectory(../../../${dep} boostorg/${dep})

endforeach()

enable_testing()
add_subdirectory(../.. boostorg/dll)
