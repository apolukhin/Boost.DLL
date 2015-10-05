/*
 * read_dll.hpp
 *
 *  Created on: 05.10.2015
 *      Author: Klemens
 */

#ifndef BOOST_DLL_DETAIL_WINDOWS_READ_DLL_HPP_
#define BOOST_DLL_DETAIL_WINDOWS_READ_DLL_HPP_

#include <boost/detail/winapi/dbghelp.hpp>
#include <boost/range/iterator_range.hpp>
#include <vector>
#include <string>

namespace boost {
namespace dll {
namespace detail {
namespace win_dll {


inline std::vector<std::string> read_dll(void* dll_)
{
	using namespace boost::detail::winapi;
	auto p = reinterpret_cast<std::uintptr_t>(lh);

	auto hd = ImageNtHeader(lh);

	if (!hd)
		return {};

	auto &offset = hd->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	auto &exp_dir = *reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(p + offset.VirtualAddress);

	std::vector<std::string> buffer;


	auto beg = reinterpret_cast<DWORD*>(p + exp_dir.AddressOfNames);
	auto end = beg + exp_dir.NumberOfNames;
	for (auto &nm : boost::make_iterator_range(beg, end))
	{
		auto name = reinterpret_cast<char*>(p + nm);
		buffer.emplace_back(name);
	}

	return buffer;
}



}}}}



#endif /* INCLUDE_BOOST_DLL_DETAIL_WINDOWS_READ_DLL_HPP_ */
