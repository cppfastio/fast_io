#pragma once
/*
Referenced from
https://stackoverflow.com/questions/44618230/in-the-msvc-abi-how-do-i-reliably-find-the-vtable-given-only-a-void
*/

namespace fast_io
{

namespace rtti_hack
{
inline char const *abi_type_info_name_or_nullptr(void *mythis) noexcept
{
	auto mythis_ptr{reinterpret_cast<std::type_info ***>(mythis)};
	auto type_info_ptr{reinterpret_cast<std::type_info *>((*mythis_ptr)[-1])};
	if (type_info_ptr == nullptr)
	{
		return nullptr;
	}
	return type_info_ptr->name();
}

} // namespace rtti_hack
} // namespace fast_io
