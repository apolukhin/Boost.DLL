// shared_library_mode.hpp ----------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti.
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 17-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_PLUGIN_SHARED_LIBRARY_MODE_HPP
#define BOOST_PLUGIN_SHARED_LIBRARY_MODE_HPP

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

namespace boost { namespace plugin {

   /*! \enum Modes of load library.
    *
    * Each of system family provides own flags. Flags not supported by a particular platform will be ignored.
    *
    * WINDOWS
    * -------
    *
    * DONT_RESOLVE_DLL_REFERENCES
    * If this value is used, and the executable module is a DLL, the system does
    * not call DllMain for process and thread initialization and termination.
    * Also, the system does not load additional executable modules that are
    * referenced by the specified module.
    *
    * Note Do not use this value; it is provided only for backward compatibility.
    * If you are planning to access only data or resources in the DLL, use
    * LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE or LOAD_LIBRARY_AS_IMAGE_RESOURCE
    * or both.
    *
    * LOAD_IGNORE_CODE_AUTHZ_LEVEL
    * If this value is used, the system does not check AppLocker rules or
    * apply Software Restriction Policies for the DLL.
    *
    * LOAD_LIBRARY_AS_DATAFILE
    * If this value is used, the system maps the file into the calling
    * process's virtual address space as if it were a data file.
    *
    * LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE
    * Similar to LOAD_LIBRARY_AS_DATAFILE, except that the DLL file is opened
    * with exclusive write access for the calling process. Other processes
    * cannot open the DLL file for write access while it is in use.
    * However, the DLL can still be opened by other processes.
    *
    * LOAD_LIBRARY_AS_IMAGE_RESOURCE
    * If this value is used, the system maps the file into the process's virtual
    * address space as an image file. However, the loader does not load the
    * static imports or perform the other usual initialization steps.
    *
    * Use this flag when you want to load a DLL only to extract messages
    * or resources from it.
    *
    * If forced integrity checking is desired for the loaded file then
    * LOAD_LIBRARY_AS_IMAGE is recommended instead.
    *
    *
    * All the LOAD_LIBRARY_SEARCH_* macro are not used... because of portability issues, even on Windows.
    *
    * LOAD_WITH_ALTERED_SEARCH_PATH
    * If this value is used and lpFileName specifies an absolute path,
    * the system uses the alternate file search strategy.
    *
    * This value cannot be combined with any LOAD_LIBRARY_SEARCH flag.
    *
    * Source :
    * http://msdn.microsoft.com/en-us/library/windows/desktop/ms684179(v=vs.85).aspx
    *
    * POSIX
    * -----
    *
    * RTLD_LAZY
    * Relocations shall be performed at an implementation-defined time, ranging
    * from the time of the dlopen() call until the first reference to a given
    * symbol occurs.
    *
    * Specifying RTLD_LAZY should improve performance on implementations
    * supporting dynamic symbol binding as a process may not reference all of
    * the functions in any given object. And, for systems supporting dynamic
    * symbol resolution for normal process execution, this behavior mimics
    * the normal handling of process execution.
    *
    * RTLD_NOW
    * All necessary relocations shall be performed when the object is first
    * loaded. This may waste some processing if relocations are performed for
    * functions that are never referenced. This behavior may be useful for
    * plugins that need to know as soon as an object is loaded that all
    * symbols referenced during execution are available.
    *
    * Any object loaded by dlopen() that requires relocations against global
    * symbols can reference the symbols in the original process image file,
    * any objects loaded at program start-up, from the object itself as well
    * as any other object included in the same dlopen() invocation, and any
    * objects that were loaded in any dlopen() invocation and which
    * specified the RTLD_GLOBAL flag. To determine the scope of visibility
    * for the symbols loaded with a dlopen() invocation, the mode parameter
    * should be a bitwise-inclusive OR with one of the following values:
    *
    * RTLD_GLOBAL
    * The object's symbols shall be made available for the relocation
    * processing of any other object. In addition, symbol lookup using
    * dlopen(0, mode) and an associated dlsym() allows objects loaded
    * with this mode to be searched.
    *
    * RTLD_LOCAL
    * The object's symbols shall not be made available for the relocation
    * processing of any other object.
    *
    * Source :
    * http://pubs.opengroup.org/onlinepubs/000095399/functions/dlopen.html
    *
    */
    struct load_mode {
        enum type {
            default_mode                          = 0,
#if BOOST_OS_WINDOWS
            // windows
            dont_resolve_dll_references           = boost::detail::winapi::DONT_RESOLVE_DLL_REFERENCES_,
            load_ignore_code_authz_level          = boost::detail::winapi::LOAD_IGNORE_CODE_AUTHZ_LEVEL_,
            load_library_as_datafile              = boost::detail::winapi::LOAD_LIBRARY_AS_DATAFILE_,
            load_library_as_datafile_exclusive    = boost::detail::winapi::LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE_,
            load_library_as_image_resource        = boost::detail::winapi::LOAD_LIBRARY_AS_IMAGE_RESOURCE_,
            load_with_altered_search_path         = boost::detail::winapi::LOAD_WITH_ALTERED_SEARCH_PATH_,

            // posix
            rtld_lazy                             = 0,
            rtld_now                              = 0,
            rtld_global                           = 0,
            rtld_local                            = 0,
            rtld_deepbind                         = 0,
#else
            // windows
            dont_resolve_dll_references           = 0,
            load_ignore_code_authz_level          = 0,
            load_library_as_datafile              = 0,
            load_library_as_datafile_exclusive    = 0,
            load_library_as_image_resource        = 0,
            load_with_altered_search_path         = 0,

            // posix
            rtld_lazy                             = RTLD_LAZY,
            rtld_now                              = RTLD_NOW,
            rtld_global                           = RTLD_GLOBAL,
            rtld_local                            = RTLD_LOCAL,
            rtld_deepbind                         = RTLD_DEEPBIND
#endif
        };
    };


    /*!
    * Free operators for load_mode::type flag manipulation.
    */
    inline load_mode::type operator|(load_mode::type left, load_mode::type right) {
        return (static_cast<load_mode::type>(
            static_cast<unsigned int>(left) | static_cast<unsigned int>(right))
        );
    }

    inline load_mode::type& operator|=(load_mode::type& left, load_mode::type right) {
        left = left | right;
        return (left);
    }

}} // boost::plugin

#endif // BOOST_PLUGIN_SHARED_LIBRARY_MODE_HPP
