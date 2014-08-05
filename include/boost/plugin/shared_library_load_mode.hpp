// shared_library_mode.hpp ----------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 17-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_SHARED_LIBRARY_MODE_HPP
#define BOOST_APPLICATION_SHARED_LIBRARY_MODE_HPP

#include <boost/application/config.hpp>

// MINGW NOTE
//
// in mingw we have some problems here, at this time we don't support shared_library for __MINGW32__
// you can download port yourself from:  https://code.google.com/p/dlfcn-win32/downloads/list

#if defined( BOOST_POSIX_API ) 
#include <dlfcn.h>
#elif defined( BOOST_WINDOWS_API ) 
// workaround [

#   ifndef DONT_RESOLVE_DLL_REFERENCES
#      define DONT_RESOLVE_DLL_REFERENCES 0x00000001
#   endif

#   ifndef LOAD_IGNORE_CODE_AUTHZ_LEVEL
#      define LOAD_IGNORE_CODE_AUTHZ_LEVEL 0x00000010
#   endif

#   ifndef LOAD_LIBRARY_AS_DATAFILE
#      define LOAD_LIBRARY_AS_DATAFILE 0x00000002
#   endif

#   ifndef LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE
#      define LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE 0x00000040
#   endif

#   ifndef LOAD_LIBRARY_AS_IMAGE_RESOURCE
#      define LOAD_LIBRARY_AS_IMAGE_RESOURCE 0x00000020
#   endif

// -
      
#   ifndef LOAD_LIBRARY_SEARCH_APPLICATION_DIR
#      define LOAD_LIBRARY_SEARCH_APPLICATION_DIR 0x00000200
#   endif

#   ifndef LOAD_LIBRARY_SEARCH_DEFAULT_DIRS
#      define LOAD_LIBRARY_SEARCH_DEFAULT_DIRS    0x00001000
#   endif

#   ifndef LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR
#      define LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR    0x00000100
#   endif

#   ifndef LOAD_LIBRARY_SEARCH_SYSTEM32
#      define LOAD_LIBRARY_SEARCH_SYSTEM32        0x00000800
#   endif

#   ifndef LOAD_LIBRARY_SEARCH_USER_DIRS
#      define LOAD_LIBRARY_SEARCH_USER_DIRS       0x00000400
#   endif
// ] end workaround
#endif

namespace boost { namespace application {

   /*! \enum Modes of load library.
    *
    * Each of system family provides your own way:
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
    * LOAD_LIBRARY_SEARCH_APPLICATION_DIR
    * If this value is used, the application's installation directory is searched
    * for the DLL and its dependencies. Directories in the standard search path
    * are not searched.
    * This value cannot be combined with LOAD_WITH_ALTERED_SEARCH_PATH.
    *
    * LOAD_LIBRARY_SEARCH_DEFAULT_DIRS
    * This value is a combination of LOAD_LIBRARY_SEARCH_APPLICATION_DIR,
    * LOAD_LIBRARY_SEARCH_SYSTEM32, and LOAD_LIBRARY_SEARCH_USER_DIRS.
    *
    * Directories in the standard search path are not searched.
    * This value cannot be combined with LOAD_WITH_ALTERED_SEARCH_PATH.
    * This value represents the recommended maximum number of directories
    * an application should include in its DLL search path.
    *
    * LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR
    * If this value is used, the directory that contains the DLL is temporarily
    * added to the beginning of the list of directories that are searched for
    * the DLL's dependencies. Directories in the standard search path are not
    * searched.
    *
    * The lpFileName parameter must specify a fully qualified path.
    * This value cannot be combined with LOAD_WITH_ALTERED_SEARCH_PATH.
    *
    * LOAD_LIBRARY_SEARCH_SYSTEM32
    * If this value is used, %windows%\system32 is searched for the DLL and its
    * dependencies. Directories in the standard search path are not searched.
    * This value cannot be combined with LOAD_WITH_ALTERED_SEARCH_PATH.
    *
    * LOAD_LIBRARY_SEARCH_USER_DIRS
    * If this value is used, directories added using the AddDllDirectory or
    * the SetDllDirectory function are searched for the DLL and its
    * dependencies.
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
    * applications that need to know as soon as an object is loaded that all
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
   enum shared_library_load_mode
   {
#if defined( BOOST_WINDOWS_API ) 
      // windows
      load_library_default_mode               = 0,
      dont_resolve_dll_references             = DONT_RESOLVE_DLL_REFERENCES,         // 0x00000001
      load_ignore_code_authz_level            = LOAD_IGNORE_CODE_AUTHZ_LEVEL,        // 0x00000010
      load_library_as_datafile                = LOAD_LIBRARY_AS_DATAFILE,            // 0x00000002
      load_library_as_datafile_exclusive      = LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE,  // 0x00000040
      load_library_as_image_resource          = LOAD_LIBRARY_AS_IMAGE_RESOURCE,      // 0x00000020

      // About LOAD_LIBRARY_SEARCH_APPLICATION_DIR, 
      //       LOAD_LIBRARY_SEARCH_DEFAULT_DIRS, 
      //       LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR,
      //       LOAD_LIBRARY_SEARCH_SYSTEM32 and
      //       LOAD_LIBRARY_SEARCH_USER_DIRS
      //
      // Minimum supported client :  Windows 8 
      // Minimum supported server : Windows Server 2012 
      //
      // To Windows 7, Windows Server 2008 R2, Windows Vista, 
      // and Windows Server 2008: KB2533623 must be installed on the target platform.

#   ifndef _USING_V110_SDK71_
      // when user uses: Visual Studio 2012 - Windows XP (v110_xp), we need hide following enums :
      load_library_search_application_dir     = LOAD_LIBRARY_SEARCH_APPLICATION_DIR, // 0x00000200
      load_library_search_default_dirs        = LOAD_LIBRARY_SEARCH_DEFAULT_DIRS,    // 0x00001000
      load_library_search_dll_load_dir        = LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR,    // 0x00000100
      load_library_search_system32            = LOAD_LIBRARY_SEARCH_SYSTEM32,        // 0x00000800
      load_library_search_user_dirs           = LOAD_LIBRARY_SEARCH_USER_DIRS,       // 0x00000400
      // <-
#   endif

      load_with_altered_search_path           = LOAD_WITH_ALTERED_SEARCH_PATH        // 0x00000008

#elif defined( BOOST_POSIX_API ) 
      // posix
      rtld_lazy   = RTLD_LAZY,   // 1
      rtld_now    = RTLD_NOW,    // 2
      rtld_global = RTLD_GLOBAL, // 3
      rtld_local  = RTLD_LOCAL   // 4
#endif
   };

   /*!
    * Free operators for shared_library_load_mode flag manibulation.
    */

   inline shared_library_load_mode operator&(
      shared_library_load_mode left,
      shared_library_load_mode right)
   {
      return (static_cast<shared_library_load_mode>(
         static_cast<unsigned int>(left) & static_cast<unsigned int>(right)));
   }

   inline shared_library_load_mode operator|(
      shared_library_load_mode left,
      shared_library_load_mode right)
   {
      return (static_cast<shared_library_load_mode>(
         static_cast<unsigned int>(left) | static_cast<unsigned int>(right)));
   }

   inline shared_library_load_mode operator^(
      shared_library_load_mode left,
      shared_library_load_mode right)
   {
      return (static_cast<shared_library_load_mode>(
         static_cast<unsigned int>(left) ^ static_cast<unsigned int>(right)));
   }

   inline shared_library_load_mode operator~(shared_library_load_mode left)
   {
      return (static_cast<shared_library_load_mode>(
         ~static_cast<unsigned int>(left)));
   }

   inline shared_library_load_mode& operator&=(
      shared_library_load_mode& left, shared_library_load_mode right)
   {
      left = left & right;
      return (left);
   }

   inline shared_library_load_mode& operator|=(
      shared_library_load_mode& left, shared_library_load_mode right)
   {
      left = left | right;
      return (left);
   }

   inline shared_library_load_mode& operator^=(
      shared_library_load_mode& left, shared_library_load_mode right)
   {
      left = left ^ right;
      return (left);
   }

}} // boost::application

#endif // BOOST_APPLICATION_SHARED_LIBRARY_MODE_HPP
