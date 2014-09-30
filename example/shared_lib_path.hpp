// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_DLL_EXAMPLE_COMMON_SHARED_LIB_PATH_HPP
#define BOOST_DLL_EXAMPLE_COMMON_SHARED_LIB_PATH_HPP

#include <boost/filesystem.hpp>
boost::filesystem::path shared_lib_path(const boost::filesystem::path& root, const std::wstring& filename_part) {
    namespace fs = ::boost::filesystem;
    fs::directory_iterator endit;

    for (fs::directory_iterator it(root); it != endit; ++it) {
        if (fs::is_regular_file(*it)
            && it->path().filename().wstring().find(filename_part) != std::wstring::npos
            && (it->path().extension() == ".dll" || it->path().extension() == ".so" || it->path().extension() == ".dylib")) {
            return *it;
        }
    }

    throw std::runtime_error("Failed to find library");
}

// This ugly copypaste from auto_link.hpp is required to get the name of a library with bjam decorations.
// I'd love to specify library layout from Jamfile but nobody on Boost-build mailing list answert how to do it and
// bjam docs does not cover that...
#if defined(_MSC_VER) || defined(__MWERKS__)

#ifdef __cplusplus
#  ifndef BOOST_CONFIG_HPP
#     include <boost/config.hpp>
#  endif
#elif defined(_MSC_VER) && !defined(__MWERKS__) && !defined(__EDG_VERSION__)
//
// C language compatability (no, honestly)
//
#  define BOOST_MSVC _MSC_VER
#  define BOOST_STRINGIZE(X) BOOST_DO_STRINGIZE(X)
#  define BOOST_DO_STRINGIZE(X) #X
#endif
//
// Only include what follows for known and supported compilers:
//
#if defined(BOOST_MSVC) \
    || defined(__BORLANDC__) \
    || (defined(__MWERKS__) && defined(_WIN32) && (__MWERKS__ >= 0x3000)) \
    || (defined(__ICL) && defined(_MSC_EXTENSIONS) && (_MSC_VER >= 1200))

#ifndef BOOST_VERSION_HPP
#  include <boost/version.hpp>
#endif

#ifndef BOOST_LIB_NAME
#  error "Macro BOOST_LIB_NAME not set (internal error)"
#endif

//
// error check:
//
#if defined(__MSVC_RUNTIME_CHECKS) && !defined(_DEBUG)
#  pragma message("Using the /RTC option without specifying a debug runtime will lead to linker errors")
#  pragma message("Hint: go to the code generation options and switch to one of the debugging runtimes")
#  error "Incompatible build options"
#endif
//
// select toolset if not defined already:
//
#ifndef BOOST_LIB_TOOLSET
#  if defined(BOOST_MSVC) && (BOOST_MSVC < 1200)
    // Note: no compilers before 1200 are supported
#  elif defined(BOOST_MSVC) && (BOOST_MSVC < 1300)

#    ifdef UNDER_CE
       // eVC4:
#      define BOOST_LIB_TOOLSET "evc4"
#    else
       // vc6:
#      define BOOST_LIB_TOOLSET "vc6"
#    endif

#  elif defined(BOOST_MSVC) && (BOOST_MSVC < 1310)

     // vc7:
#    define BOOST_LIB_TOOLSET "vc7"

#  elif defined(BOOST_MSVC) && (BOOST_MSVC < 1400)

     // vc71:
#    define BOOST_LIB_TOOLSET "vc71"

#  elif defined(BOOST_MSVC) && (BOOST_MSVC < 1500)

     // vc80:
#    define BOOST_LIB_TOOLSET "vc80"

#  elif defined(BOOST_MSVC) && (BOOST_MSVC < 1600)

     // vc90:
#    define BOOST_LIB_TOOLSET "vc90"

#  elif defined(BOOST_MSVC) && (BOOST_MSVC < 1700)

     // vc10:
#    define BOOST_LIB_TOOLSET "vc100"

#  elif defined(BOOST_MSVC) && (BOOST_MSVC < 1800)

     // vc11:
#    define BOOST_LIB_TOOLSET "vc110"

#  elif defined(BOOST_MSVC)

     // vc12:
#    define BOOST_LIB_TOOLSET "vc120"

#  elif defined(__BORLANDC__)

     // CBuilder 6:
#    define BOOST_LIB_TOOLSET "bcb"

#  elif defined(__ICL)

     // Intel C++, no version number:
#    define BOOST_LIB_TOOLSET "iw"

#  elif defined(__MWERKS__) && (__MWERKS__ <= 0x31FF )

     // Metrowerks CodeWarrior 8.x
#    define BOOST_LIB_TOOLSET "cw8"

#  elif defined(__MWERKS__) && (__MWERKS__ <= 0x32FF )

     // Metrowerks CodeWarrior 9.x
#    define BOOST_LIB_TOOLSET "cw9"

#  endif
#endif // BOOST_LIB_TOOLSET

//
// select thread opt:
//
#if defined(_MT) || defined(__MT__)
#  define BOOST_LIB_THREAD_OPT "-mt"
#else
#  define BOOST_LIB_THREAD_OPT
#endif

#if defined(_MSC_VER) || defined(__MWERKS__)

#  ifdef _DLL

#     if (defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)) && (defined(_STLP_OWN_IOSTREAMS) || defined(__STL_OWN_IOSTREAMS))

#        if defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))\
               && defined(BOOST_DEBUG_PYTHON) && defined(BOOST_LINKING_PYTHON)
#            define BOOST_LIB_RT_OPT "-gydp"
#        elif defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))
#            define BOOST_LIB_RT_OPT "-gdp"
#        elif defined(_DEBUG)\
               && defined(BOOST_DEBUG_PYTHON) && defined(BOOST_LINKING_PYTHON)
#            define BOOST_LIB_RT_OPT "-gydp"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        elif defined(_DEBUG)
#            define BOOST_LIB_RT_OPT "-gdp"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        else
#            define BOOST_LIB_RT_OPT "-p"
#        endif

#     elif defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)

#        if defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))\
               && defined(BOOST_DEBUG_PYTHON) && defined(BOOST_LINKING_PYTHON)
#            define BOOST_LIB_RT_OPT "-gydpn"
#        elif defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))
#            define BOOST_LIB_RT_OPT "-gdpn"
#        elif defined(_DEBUG)\
               && defined(BOOST_DEBUG_PYTHON) && defined(BOOST_LINKING_PYTHON)
#            define BOOST_LIB_RT_OPT "-gydpn"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        elif defined(_DEBUG)
#            define BOOST_LIB_RT_OPT "-gdpn"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        else
#            define BOOST_LIB_RT_OPT "-pn"
#        endif

#     else

#        if defined(_DEBUG) && defined(BOOST_DEBUG_PYTHON) && defined(BOOST_LINKING_PYTHON)
#            define BOOST_LIB_RT_OPT "-gyd"
#        elif defined(_DEBUG)
#            define BOOST_LIB_RT_OPT "-gd"
#        else
#            define BOOST_LIB_RT_OPT
#        endif

#     endif

#  else

#     if (defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)) && (defined(_STLP_OWN_IOSTREAMS) || defined(__STL_OWN_IOSTREAMS))

#        if defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))\
               && defined(BOOST_DEBUG_PYTHON) && defined(BOOST_LINKING_PYTHON)
#            define BOOST_LIB_RT_OPT "-sgydp"
#        elif defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))
#            define BOOST_LIB_RT_OPT "-sgdp"
#        elif defined(_DEBUG)\
               && defined(BOOST_DEBUG_PYTHON) && defined(BOOST_LINKING_PYTHON)
#             define BOOST_LIB_RT_OPT "-sgydp"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        elif defined(_DEBUG)
#             define BOOST_LIB_RT_OPT "-sgdp"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        else
#            define BOOST_LIB_RT_OPT "-sp"
#        endif

#     elif defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)

#        if defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))\
               && defined(BOOST_DEBUG_PYTHON) && defined(BOOST_LINKING_PYTHON)
#            define BOOST_LIB_RT_OPT "-sgydpn"
#        elif defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))
#            define BOOST_LIB_RT_OPT "-sgdpn"
#        elif defined(_DEBUG)\
               && defined(BOOST_DEBUG_PYTHON) && defined(BOOST_LINKING_PYTHON)
#             define BOOST_LIB_RT_OPT "-sgydpn"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        elif defined(_DEBUG)
#             define BOOST_LIB_RT_OPT "-sgdpn"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        else
#            define BOOST_LIB_RT_OPT "-spn"
#        endif

#     else

#        if defined(_DEBUG)\
               && defined(BOOST_DEBUG_PYTHON) && defined(BOOST_LINKING_PYTHON)
#             define BOOST_LIB_RT_OPT "-sgyd"
#        elif defined(_DEBUG)
#             define BOOST_LIB_RT_OPT "-sgd"
#        else
#            define BOOST_LIB_RT_OPT "-s"
#        endif

#     endif

#  endif

#elif defined(__BORLANDC__)

//
// figure out whether we want the debug builds or not:
//
#if __BORLANDC__ > 0x561
#pragma defineonoption BOOST_BORLAND_DEBUG -v
#endif
//
// sanity check:
//
#if defined(__STL_DEBUG) || defined(_STLP_DEBUG)
#error "Pre-built versions of the Boost libraries are not provided in STLport-debug form"
#endif

#  ifdef _RTLDLL

#     if defined(BOOST_BORLAND_DEBUG)\
               && defined(BOOST_DEBUG_PYTHON) && defined(BOOST_LINKING_PYTHON)
#         define BOOST_LIB_RT_OPT "-yd"
#     elif defined(BOOST_BORLAND_DEBUG)
#         define BOOST_LIB_RT_OPT "-d"
#     elif defined(BOOST_DEBUG_PYTHON) && defined(BOOST_LINKING_PYTHON)
#         define BOOST_LIB_RT_OPT -y
#     else
#         define BOOST_LIB_RT_OPT
#     endif

#  else

#     if defined(BOOST_BORLAND_DEBUG)\
               && defined(BOOST_DEBUG_PYTHON) && defined(BOOST_LINKING_PYTHON)
#         define BOOST_LIB_RT_OPT "-syd"
#     elif defined(BOOST_BORLAND_DEBUG)
#         define BOOST_LIB_RT_OPT "-sd"
#     elif defined(BOOST_DEBUG_PYTHON) && defined(BOOST_LINKING_PYTHON)
#         define BOOST_LIB_RT_OPT "-sy"
#     else
#         define BOOST_LIB_RT_OPT "-s"
#     endif

#  endif

#endif

//
// select linkage opt:
//
#if (defined(_DLL) || defined(_RTLDLL)) && defined(BOOST_DYN_LINK)
#  define BOOST_LIB_PREFIX
#elif defined(BOOST_DYN_LINK)
#  error "Mixing a dll boost library with a static runtime is a really bad idea..."
#else
#  define BOOST_LIB_PREFIX "lib"
#endif

#ifdef BOOST_AUTO_LINK_TAGGED
#   define BJAM_LIBRARY_DECORATIONS BOOST_LIB_THREAD_OPT BOOST_LIB_RT_OPT
#elif defined(BOOST_AUTO_LINK_NOMANGLE)
#   define BJAM_LIBRARY_DECORATIONS
#else
#   define BJAM_LIBRARY_DECORATIONS  "-" BOOST_LIB_TOOLSET BOOST_LIB_THREAD_OPT BOOST_LIB_RT_OPT "-" BOOST_LIB_VERSION
#endif

#endif

#else
#   define BJAM_LIBRARY_DECORATIONS
#endif

#endif // BOOST_DLL_EXAMPLE_COMMON_SHARED_LIB_PATH_HPP

