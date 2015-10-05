/*
 * read_elf.hpp
 *
 *  Created on: 05.10.2015
 *      Author: Klemens
 */

#ifndef BOOST_DLL_DETAIL_POSIX_READ_ELF_HPP_
#define BOOST_DLL_DETAIL_POSIX_READ_ELF_HPP_

#include <cstdint>


namespace boost {
namespace dll {
namespace detail {
namespace elf {

struct elf_t {};
struct section_header_t {};

struct elf32
{
	using addr 	= std::uint32_t;
	using half 	= std::uint16_t;
	using off	= std::uint32_t;
	using sword = std::uint32_t;
	using xword = std::uint32_t;
	using word  = std::uint32_t;
	using uchar = std::uint8_t;
};

struct elf64
{
	using addr 	= std::uint64_t;
	using half 	= std::uint16_t;
	using off	= std::uint64_t;
	using sword = std::uint32_t;
	using xword = std::uint64_t;
	using word  = std::uint32_t;
	using uchar = std::uint8_t;
};


template<typename T> using addr 	= typename T::addr 	;
template<typename T> using half 	= typename T::half 	;
template<typename T> using off		= typename T::off		;
template<typename T> using sword 	= typename T::sword    ;
template<typename T> using xword	= typename T::xword   ;
template<typename T> using word  	= typename T::word     ;
template<typename T> using uchar 	= typename T::uchar    ;

template<typename T>
struct eh_header
{
	uchar<T> e_ident[16];/**< ELF identification */
	half<T>  e_type; 	 /**< Object file type */
	half<T>  e_machine;  /**< Machine type */
	word<T>  e_version;  /**< Object file version */
	addr<T>  e_entry; 	 /**< Entry point address */
	off<T>   e_phoff; 	 /**< Program header offset */
	off<T>   e_shoff; 	 /**< Section header offset */
	word<T>  e_flags; 	 /**< Processor-specific flags */
	half<T>  e_ehsize; 	 /**< ELF header size */
	half<T>  e_phentsize;/**< Size of program header entry */
	half<T>  e_phnum; 	 /**< Number of program header entries */
	half<T>  e_shentsize;/**< Size of section header entry */
	half<T>  e_shnum; 	 /**< Number of section header entries */
	half<T>  e_shstrndx; /**< Section name string table index */

	static_assert(
			sizeof( uchar<T>) +
			sizeof( half<T> ) +
			sizeof( half<T> ) +
			sizeof( word<T> ) +
			sizeof( addr<T> ) +
			sizeof( off<T>  ) +
			sizeof( off<T>  ) +
			sizeof( word<T> ) +
			sizeof( half<T> ) +
			sizeof( half<T> ) +
			sizeof(	half<T> ) +
			sizeof( half<T> ) +
			sizeof( half<T> ) +
			sizeof( half<T> ), "Size error");

} __attribute__((__packed__));



enum e_ident_t
{
	ei_mag0 		= 0, ///<File identification
	ei_mag1 		= 1,
	ei_mag2 		= 2,
	ei_mag3 		= 3,
	ei_class		= 4, ///< File class
	ei_data 		= 5, ///< Data encoding
	ei_version 	  	= 6, ///< File version
	ei_osabi  	  	= 7, ///< OS/ABI identification
	ei_abiversion 	= 8, ///< ABI version
	ei_pad  	  	= 9, ///< Start of padding bytes
	ei_nident 	  	= 16 ///< Size of e_ident[]
};

enum ei_class_t
{
	elf_class32 = 1, ///<32-bit objects
	elf_class64 = 2, ///<64-bit objects
};

enum ei_data_t : std::uint8_t
{
	elf_data_none = 0, //< Invalid data encoding
	elf_data_2lsb = 1, //< Lsb
	elf_data_2msb = 2, //< Msb
};

enum ei_type_t
{
	et_none 	= 0, ///<No file type
	et_rel   	= 1, ///<Relocatable object file
	et_exec 	= 2, ///<Executable file
	et_dyn  	= 3, ///<Shared object file
	et_core  	= 4, ///<Core file
	et_loos  	= 0xFE00, ///<Environment-specific use
	et_hios  	= 0xFEFF, ///<
	et_loproc  	= 0xFF00, ///<Processor-specific use
	et_hiproc  	= 0xFFFF, ///<
};

template<typename T>
struct section_header
{
	word<T>  sh_name; 		/**< Section name */
	word<T>  sh_type; 		/**< Section type */
	xword<T> sh_flags; 		/**< Section attributes */
	addr<T>  sh_addr; 		/**< Virtual address in memory */
	off<T>   sh_offset; 	/**< Offset in file */
	xword<T> sh_size; 		/**< Size of section */
	word<T>  sh_link; 		/**< Link to other section */
	word<T>  sh_info; 		/**< Miscellaneous information */
	xword<T> sh_addralign; 	/**< Address alignment boundary */
	xword<T> sh_entsize; 	/**< Size of entries, if section has table */

	static_assert(
				sizeof( word<T>  ) +
				sizeof( word<T>  ) +
				sizeof( xword<T> ) +
				sizeof( addr<T>  ) +
				sizeof( off<T>   ) +
				sizeof( xword<T> ) +
				sizeof( word<T>  ) +
				sizeof( word<T>  ) +
				sizeof( xword<T> ) +
				sizeof( xword<T> ), "Size error");

} __attribute__((__packed__));

enum sh_type_t
{
	sht_null 	 = 0, /**< Marks an unused section header                                       */
	sht_progbits = 1, /**< Contains information defined by 	the program                      	*/
	sht_symtab 	 = 2, /**< Contains a linker symbol table                                       */
	sht_strtab 	 = 3, /**< Contains a string table                                              */
	sht_rela 	 = 4, /**< Contains “Rela” type relocation 	entries                          	*/
	sht_hash 	 = 5, /**< Contains a symbol hash table                                         */
	sht_dynamic  = 6, /**< Contains dynamic linking tables                                      */
	sht_note 	 = 7, /**< Contains note information                                            */
	sht_nobits 	 = 8, /**< Contains uninitialized space; does 	not occupy any space in the file*/
	sht_rel 	 = 9, /**< Contains “Rel” type relocation entries                               */
	sht_shlib 	 = 10,/**< Reserved                                                             */
	sht_dynsym 	 = 11,/**< Contains a dynamic loader symbol table                               */
	sht_loos 	 = 0x60000000, /**< Environment-specific use                                    */
	sht_hios 	 = 0x6FFFFFFF,
	sht_loproc 	 = 0x70000000, /**< Processor-specific use                                      */
	sht_hiproc 	 = 0x7FFFFFFF,
};



template<template <typename> class Return, typename Elf>
inline const Return<Elf> &make_offset(off<Elf> offset, const eh_header<Elf>& hd)
{
	typedef Return<Elf> const eh_header<Elf>::*p_t;
	std::uintptr_t ptr = offset;
	auto p = *reinterpret_cast<p_t*>(&ptr);
	return hd.*p;
}
using namespace std;


inline std::uint8_t   make_endian(ei_data_t type, std::uint8_t  data) {return data;}
inline std::uint16_t  make_endian(ei_data_t type, std::uint16_t data)
{
	using namespace std;
	auto &arr = reinterpret_cast<array<unsigned char, sizeof(uint16_t)>&>(data);
	if (type == elf_data_2msb)
		return (static_cast<uint16_t>(arr[0])<< 8)
			 | (static_cast<uint16_t>(arr[1]));
	else
		return (static_cast<uint16_t>(arr[1])<< 8)
			 | (static_cast<uint16_t>(arr[0]));
}
inline std::uint32_t  make_endian(ei_data_t type, std::uint32_t data)
{
	using namespace std;
	auto &arr = reinterpret_cast<array<unsigned char, sizeof(uint32_t)>&>(data);
	if (type == elf_data_2msb)
		return (static_cast<uint32_t>(arr[0]) << 24)
			 | (static_cast<uint32_t>(arr[1]) << 16)
			 | (static_cast<uint32_t>(arr[2]) << 8)
			 | (static_cast<uint32_t>(arr[3]));
	else
		return (static_cast<uint32_t>(arr[3]) << 24)
			 | (static_cast<uint32_t>(arr[2]) << 16)
			 | (static_cast<uint32_t>(arr[1]) << 8)
			 | (static_cast<uint32_t>(arr[0]));
}
inline std::uint64_t make_endian(ei_data_t type, std::uint64_t data)
{
	using namespace std;
	auto &arr = reinterpret_cast<array<unsigned char, sizeof(uint64_t)>&>(data);
	if (type == elf_data_2msb)
		return (static_cast<uint64_t>(arr[0]) << 56)
			 | (static_cast<uint64_t>(arr[1]) << 48)
			 | (static_cast<uint64_t>(arr[2]) << 40)
			 | (static_cast<uint64_t>(arr[3]) << 32)
			 | (static_cast<uint64_t>(arr[4]) << 24)
			 | (static_cast<uint64_t>(arr[5]) << 16)
			 | (static_cast<uint64_t>(arr[6]) << 8)
			 | (static_cast<uint64_t>(arr[7]));
	else
		return (static_cast<uint64_t>(arr[7]) << 56)
			 | (static_cast<uint64_t>(arr[6]) << 48)
			 | (static_cast<uint64_t>(arr[5]) << 40)
			 | (static_cast<uint64_t>(arr[4]) << 32)
			 | (static_cast<uint64_t>(arr[3]) << 24)
			 | (static_cast<uint64_t>(arr[2]) << 16)
			 | (static_cast<uint64_t>(arr[1]) << 8)
			 | (static_cast<uint64_t>(arr[0]));
}

inline std::vector<std::string> read_elf_32(const elf_t& elf)
{
	auto &hd = reinterpret_cast<const eh_header<elf32>&>(elf);

	auto df =  static_cast<ei_data_t>(hd.e_ident[ei_data]);

	const auto &sh = make_offset<section_header>(make_endian(df, hd.e_shoff), hd);

	auto sh_r = boost::make_iterator_range(&sh, &sh + hd.e_shnum);
	std::vector<std::string> buf;
	for (auto s : sh_r)
	{
		if (s.sh_type == sht_strtab)
		{
			auto off = make_endian(df, s.sh_offset);
			auto str = reinterpret_cast<const char*>(&hd) + off;

			if (str[0] != '\0')
				return {};
			unsigned int cnt = 0;
			while (cnt < s.sh_size)
			{
				if (str[cnt] == '\0')
					buf.emplace_back(str + cnt +1);;
				cnt++;
			}
		}
	}
	return buf;
}

inline std::vector<std::string> read_elf_64(const elf_t& elf)
{
	auto &hd = reinterpret_cast<const eh_header<elf64>&>(elf);

	auto df =  static_cast<ei_data_t>(hd.e_ident[ei_data]);

	const auto &sh = make_offset<section_header>(make_endian(df, hd.e_shoff), hd);

	auto sh_r = boost::make_iterator_range(&sh, &sh + hd.e_shnum);
	std::vector<std::string> buf;
	for (auto s : sh_r)
	{
		if (s.sh_type == sht_strtab)
		{
			auto off = make_endian(df, s.sh_offset);
			auto str = reinterpret_cast<const char*>(&hd) + off;

			if (str[0] != '\0')
				return {};
			unsigned int cnt = 0;
			while (cnt < s.sh_size)
			{
				if (str[cnt] == '\0')
					buf.emplace_back(str + cnt +1);;
				cnt++;
			}
		}
	}
	return buf;
}

inline std::vector<std::string> read_elf(const elf_t& elf)
{

	using namespace std;

	static array<uint8_t, 4> mag = { 0x7F, 'E', 'L', 'F'};

	typedef  array<uint8_t, 4> elf_t::*ei_mag_t;
	uintptr_t ei_mag_p = 0x00;

	auto ei_mag = *reinterpret_cast<ei_mag_t*>(&ei_mag_p);


	if (!equal(mag.begin(), mag.end(), (elf.*ei_mag).begin()))
		return {};

	typedef uint16_t elf_t::*e_type_t;
	uintptr_t e_type_p = 0x010;
	auto e_type = *reinterpret_cast<e_type_t*>(&e_type_p);

	if (elf.*e_type != 2)
		return {};

	typedef uint8_t elf_t::*ei_class_t;
	uintptr_t ei_class_p = 0x04;
	auto ei_class = *reinterpret_cast<ei_class_t*>(&ei_class_p);

	if (elf.*ei_class == 1)
		return read_elf_32(elf);
	else if (elf.*ei_class == 2)
		return read_elf_64(elf);
	else
		return {};
}
//atm there is no check of the range of the data.
inline std::vector<std::string> read_elf(void* begin)
{
	return read_elf(*reinterpret_cast<elf_t*>(begin));
}


}}}}




#endif /* INCLUDE_BOOST_DLL_DETAIL_POSIX_READ_ELF_HPP_ */
