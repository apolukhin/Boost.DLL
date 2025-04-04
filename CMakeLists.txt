# Copyright 2020 Peter Dimov
# Copyright Antony Polukhin, 2021-2025
#
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5...3.16)

project(boost_dll VERSION "${BOOST_SUPERPROJECT_VERSION}" LANGUAGES CXX)

add_library(boost_dll INTERFACE)
add_library(Boost::dll ALIAS boost_dll)

option(BOOST_DLL_USE_STD_FS "Use std::filesystem instead of Boost.Filesystem" OFF)

target_include_directories(boost_dll INTERFACE include)

target_link_libraries(boost_dll
  INTERFACE
    Boost::assert
    Boost::config
    Boost::core
    Boost::predef
    Boost::system
    Boost::throw_exception
    Boost::type_index
    Boost::winapi
    ${CMAKE_DL_LIBS}
)

if (BOOST_DLL_USE_STD_FS)
  target_compile_definitions(boost_dll INTERFACE BOOST_DLL_USE_STD_FS)
else()
  target_link_libraries(boost_dll INTERFACE Boost::filesystem)
endif()

if(BUILD_TESTING AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
    
  add_subdirectory(test)

endif()

