#pragma once

namespace fast_io
{

namespace rtti_hack
{
template <typename T>
inline char const *abi_type_info_name_or_nullptr(T *mythis) noexcept
{
#if (defined(_MSC_VER) && _HAS_RTTI != 0) || (!defined(_MSC_VER) && defined(__cpp_rtti))
#if (defined(_MSC_VER) && _HAS_EXCEPTIONS != 0) || (!defined(_MSC_VER) && defined(__cpp_exceptions))
	try
	{
#endif
		return typeid(*mythis).name();
#if (defined(_MSC_VER) && _HAS_EXCEPTIONS != 0) || (!defined(_MSC_VER) && defined(__cpp_exceptions))
	}
	catch (...)
	{
		return nullptr;
	}
#endif
#else
	return nullptr;
#endif
}
} // namespace rtti_hack

} // namespace fast_io
