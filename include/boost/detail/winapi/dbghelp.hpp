/*
 * dbghelp.hpp
 *
 *  Created on: 05.10.2015
 *      Author: Klemens
 */

#ifndef INCLUDE_BOOST_DETAIL_WINAPI_DBGHELP_HPP_
#define INCLUDE_BOOST_DETAIL_WINAPI_DBGHELP_HPP_


#include <boost/detail/winapi/basic_types.hpp>

namespace boost
{
namespace detail
{
namespace winapi
{
extern "C"
{


typedef struct IMAGE_DATA_DIRECTORY_ {
  DWORD_ VirtualAddress;
  DWORD_ Size;
};

struct IMAGE_EXPORT_DIRECTORY_ {
  DWORD_ Characteristics;
  DWORD_ TimeDateStamp;
  WORD_ MajorVersion;
  WORD_ MinorVersion;
  DWORD_ Name;
  DWORD_ Base;
  DWORD_ NumberOfFunctions;
  DWORD_ NumberOfNames;
  DWORD_ AddressOfFunctions;
  DWORD_ AddressOfNames;
  DWORD_ AddressOfNameOrdinals;
};

typedef struct IMAGE_FILE_HEADER_ {
  WORD_ Machine;
  WORD_ NumberOfSections;
  DWORD_ TimeDateStamp;
  DWORD_ PointerToSymbolTable;
  DWORD_ NumberOfSymbols;
  WORD_ SizeOfOptionalHeader;
  WORD_ Characteristics;
} ;

static constexpr std::size_t IMAGE_NUMBEROF_DIRECTORY_ENTRIES_ = 16;

struct IMAGE_OPTIONAL_HEADER32_ {

  WORD_ Magic;
  BYTE_ MajorLinkerVersion;
  BYTE_ MinorLinkerVersion;
  DWORD_ SizeOfCode;
  DWORD_ SizeOfInitializedData;
  DWORD_ SizeOfUninitializedData;
  DWORD_ AddressOfEntryPoint;
  DWORD_ BaseOfCode;
  DWORD_ BaseOfData;
  DWORD_ ImageBase;
  DWORD_ SectionAlignment;
  DWORD_ FileAlignment;
  WORD_ MajorOperatingSystemVersion;
  WORD_ MinorOperatingSystemVersion;
  WORD_ MajorImageVersion;
  WORD_ MinorImageVersion;
  WORD_ MajorSubsystemVersion;
  WORD_ MinorSubsystemVersion;
  DWORD_ Win32VersionValue;
  DWORD_ SizeOfImage;
  DWORD_ SizeOfHeaders;
  DWORD_ CheckSum;
  WORD_ Subsystem;
  WORD_ DllCharacteristics;
  DWORD_ SizeOfStackReserve;
  DWORD_ SizeOfStackCommit;
  DWORD_ SizeOfHeapReserve;
  DWORD_ SizeOfHeapCommit;
  DWORD_ LoaderFlags;
  DWORD_ NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES_];
};

typedef struct IMAGE_OPTIONAL_HEADER64_ {
  WORD_ Magic;
  BYTE_ MajorLinkerVersion;
  BYTE_ MinorLinkerVersion;
  DWORD_ SizeOfCode;
  DWORD_ SizeOfInitializedData;
  DWORD_ SizeOfUninitializedData;
  DWORD_ AddressOfEntryPoint;
  DWORD_ BaseOfCode;
  ULONGLONG_ ImageBase;
  DWORD_ SectionAlignment;
  DWORD_ FileAlignment;
  WORD_ MajorOperatingSystemVersion;
  WORD_ MinorOperatingSystemVersion;
  WORD_ MajorImageVersion;
  WORD_ MinorImageVersion;
  WORD_ MajorSubsystemVersion;
  WORD_ MinorSubsystemVersion;
  DWORD_ Win32VersionValue;
  DWORD_ SizeOfImage;
  DWORD_ SizeOfHeaders;
  DWORD_ CheckSum;
  WORD_ Subsystem;
  WORD_ DllCharacteristics;
  ULONGLONG_ SizeOfStackReserve;
  ULONGLONG_ SizeOfStackCommit;
  ULONGLONG_ SizeOfHeapReserve;
  ULONGLONG_ SizeOfHeapCommit;
  DWORD_ LoaderFlags;
  DWORD_ NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES_];
};


typedef struct IMAGE_NT_HEADERS64_ {
  DWORD_ Signature;
  IMAGE_FILE_HEADER_ FileHeader;
  IMAGE_OPTIONAL_HEADER64_ OptionalHeader;
};

typedef struct IMAGE_NT_HEADERS32_ {
  DWORD_ Signature;
  IMAGE_FILE_HEADER_ FileHeader;
  IMAGE_OPTIONAL_HEADER32_ OptionalHeader;
};


#ifdef _WIN64
    typedef IMAGE_NT_HEADERS64_ IMAGE_NT_HEADERS_;
#else  /* _WIN64 */
    typedef IMAGE_NT_HEADERS32_ IMAGE_NT_HEADERS_;
#endif /* _WIN64 */


__declspec(dllimport) IMAGE_NT_HEADERS_*  WINAPI ImageNtHeader(PVOID_ Base);


}
}}}




#endif /* INCLUDE_BOOST_DETAIL_WINAPI_DBGHELP_HPP_ */
