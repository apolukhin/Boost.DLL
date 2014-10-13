// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_DLL_SHARED_LIBRARY_MODE_HPP
#define BOOST_DLL_SHARED_LIBRARY_MODE_HPP

#include <boost/config.hpp>
#include <boost/predef/os.h>

#if BOOST_OS_WINDOWS
//#include <boost/detail/winapi/dll.hpp>
#include <boost/detail/winapi/dll2.hpp> // TODO: FIXME
#else
#   include <dlfcn.h>
#endif

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

/// \file boost/dll/shared_library_load_mode.hpp
/// \brief Contains only the boost::dll::load_mode::type enum and operators related to it.

namespace boost { namespace dll { namespace load_mode {

/*! Library load modes.
*
* Each of system family provides own modes. Flags not supported by a particular platform will be silently ignored.
*
* For a detailed description of platform specific options see:
* <a href="http://msdn.microsoft.com/en-us/library/windows/desktop/ms684179(v=vs.85).aspx">Windows specific options</a>,
* <a href="http://pubs.opengroup.org/onlinepubs/000095399/functions/dlopen.html">POSIX specific options</a>.
*
*/

enum type {
#ifdef BOOST_DLL_DOXYGEN
    /*!
    * Default open mode. See the \b Default: comments below to find out the flags that are enabled by default.
    */
    default_mode,

    /*!
    * \b Platforms: Windows
    *
    * \b Default: disabled
    *
    * If this value is used, and the executable module is a DLL, the system does
    * not call DllMain for process and thread initialization and termination.
    * Also, the system does not load additional executable modules that are
    * referenced by the specified module.
    *
    * Note Do not use this value; it is provided only for backward compatibility.
    * If you are planning to access only data or resources in the DLL, use
    * LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE or LOAD_LIBRARY_AS_IMAGE_RESOURCE
    * or both.
    */
    dont_resolve_dll_references,

    /*!
    * \b Platforms: Windows
    *
    * \b Default: disabled
    *
    * If this value is used, the system does not check AppLocker rules or
    * apply Software Restriction Policies for the DLL.
    */
    load_ignore_code_authz_level,

    /*!
    * \b Platforms: Windows
    *
    * \b Default: disabled
    *
    * If this value is used, the system maps the file into the calling
    * process's virtual address space as if it were a data file.
    */
    load_library_as_datafile,

    /*!
    * \b Platforms: Windows
    *
    * \b Default: disabled
    *
    * Similar to LOAD_LIBRARY_AS_DATAFILE, except that the DLL file is opened
    * with exclusive write access for the calling process. Other processes
    * cannot open the DLL file for write access while it is in use.
    * However, the DLL can still be opened by other processes.
    */
    load_library_as_datafile_exclusive,

    /*!
    * \b Platforms: Windows
    *
    * \b Default: disabled
    *
    * If this value is used, the system maps the file into the process's virtual
    * address space as an image file. However, the loader does not load the
    * static imports or perform the other usual initialization steps.
    *
    * Use this flag when you want to load a DLL only to extract messages
    * or resources from it.
    *
    * If forced integrity checking is desired for the loaded file then
    * LOAD_LIBRARY_AS_IMAGE is recommended instead.
    */
    load_library_as_image_resource,

    /*!
    * \b Platforms: Windows
    *
    * \b Default: disabled
    *
    * If this value is used and lpFileName specifies an absolute path,
    * the system uses the alternate file search strategy.
    *
    * This value cannot be combined with any LOAD_LIBRARY_SEARCH flag.
    */
    load_with_altered_search_path,

    /*!
    * \b Platforms: POSIX
    *
    * \b Default: enabled
    *
    * Relocations shall be performed at an implementation-defined time, ranging
    * from the time of the dlopen() call until the first reference to a given
    * symbol occurs.
    *
    * Specifying RTLD_LAZY should improve performance on implementations
    * supporting dynamic symbol binding as a process may not reference all of
    * the functions in any given object. And, for systems supporting dynamic
    * symbol resolution for normal process execution, this behavior mimics
    * the normal handling of process execution.
    */
    rtld_lazy,

    /*!
    * \b Platforms: POSIX
    *
    * \b Default: disabled
    *
    * All necessary relocations shall be performed when the object is first
    * loaded. This may waste some processing if relocations are performed for
    * functions that are never referenced. This behavior may be useful for
    * plugins that need to know as soon as an object is loaded that all
    * symbols referenced during execution are available.
    */
    rtld_now,

    /*!
    * \b Platforms: POSIX
    *
    * \b Default: disabled
    *
    * The object's symbols shall be made available for the relocation
    * processing of any other object. In addition, symbol lookup using
    * dlopen(0, mode) and an associated dlsym() allows objects loaded
    * with this mode to be searched.
    */
    rtld_global,

    /*!
    * \b Platforms: POSIX
    *
    * \b Default: enabled
    *
    * The object's symbols shall not be made available for the relocation
    * processing of any other object.
    *
    * This is a default Windows behavior that can not be changed.
    */
    rtld_local,
    rtld_deepbind,

    /*!
    * \b Platforms: Windows, POSIX
    *
    * \b Default: disabled
    *
    * Append a platform specific extension and prefix to shared_library filename.
    *
    * \b Example:
    * \code
    * // Opens `./my_plugins/plugin1.dll` on Windows, `./my_plugins/libplugin1.so` on Linux and Apple
    * boost::dll::shared_library lib("./my_plugins/plugin1", load_mode::append_decorations);
    * \endcode
    */
    append_decorations
#elif BOOST_OS_WINDOWS
    default_mode                          = 0,
    dont_resolve_dll_references           = boost::detail::winapi::DONT_RESOLVE_DLL_REFERENCES_,
    load_ignore_code_authz_level          = boost::detail::winapi::LOAD_IGNORE_CODE_AUTHZ_LEVEL_,
    load_library_as_datafile              = boost::detail::winapi::LOAD_LIBRARY_AS_DATAFILE_,
    load_library_as_datafile_exclusive    = boost::detail::winapi::LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE_,
    load_library_as_image_resource        = boost::detail::winapi::LOAD_LIBRARY_AS_IMAGE_RESOURCE_,
    load_with_altered_search_path         = boost::detail::winapi::LOAD_WITH_ALTERED_SEARCH_PATH_,
    rtld_lazy                             = 0,
    rtld_now                              = 0,
    rtld_global                           = 0,
    rtld_local                            = 0,
    rtld_deepbind                         = 0,
    append_decorations                    = 0x00800000
#else
    default_mode                          = 0,
    dont_resolve_dll_references           = 0,
    load_ignore_code_authz_level          = 0,
    load_library_as_datafile              = 0,
    load_library_as_datafile_exclusive    = 0,
    load_library_as_image_resource        = 0,
    load_with_altered_search_path         = 0,
    rtld_lazy                             = RTLD_LAZY,
    rtld_now                              = RTLD_NOW,
    rtld_global                           = RTLD_GLOBAL,
    rtld_local                            = RTLD_LOCAL,
    rtld_deepbind                         = RTLD_DEEPBIND,
    append_decorations                    = 0x00800000
#endif
};


/// Free operators for load_mode::type flag manipulation.
inline type operator|(type left, type right) BOOST_NOEXCEPT {
    return (static_cast<type>(
        static_cast<unsigned int>(left) | static_cast<unsigned int>(right))
    );
}

inline type& operator|=(type& left, type right) BOOST_NOEXCEPT {
    left = left | right;
    return (left);
}

}}} // boost::dll::load_mode

#endif // BOOST_DLL_SHARED_LIBRARY_MODE_HPP
