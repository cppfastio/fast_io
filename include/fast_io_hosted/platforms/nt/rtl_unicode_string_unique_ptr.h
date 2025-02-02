#pragma once

namespace fast_io::win32::nt
{

struct rtl_unicode_string_unique_ptr
{
	unicode_string *heap_ptr{};
	inline constexpr rtl_unicode_string_unique_ptr() = default;
	inline constexpr rtl_unicode_string_unique_ptr(unicode_string *ptr)
		: heap_ptr(ptr)
	{}
	inline rtl_unicode_string_unique_ptr(rtl_unicode_string_unique_ptr const &) = delete;
	inline rtl_unicode_string_unique_ptr &operator=(rtl_unicode_string_unique_ptr const &) = delete;
	inline constexpr rtl_unicode_string_unique_ptr(rtl_unicode_string_unique_ptr &&__restrict other) noexcept
		: heap_ptr(other.heap_ptr)
	{
		other.heap_ptr = nullptr;
	}
	inline rtl_unicode_string_unique_ptr &operator=(rtl_unicode_string_unique_ptr &&__restrict other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		if (heap_ptr) [[likely]]
		{
			rtl_free_unicode_string(heap_ptr);
		}
		heap_ptr = other.heap_ptr;
		other.heap_ptr = nullptr;
		return *this;
	}
	inline ~rtl_unicode_string_unique_ptr()
	{
		if (heap_ptr) [[likely]]
		{
			rtl_free_unicode_string(heap_ptr);
		}
	}
};

} // namespace fast_io::win32::nt
