#pragma once

namespace fast_io::win32::nt
{

struct rtl_unicode_string_unique_ptr
{
	unicode_string* heap_ptr{};
	constexpr rtl_unicode_string_unique_ptr()=default;
	constexpr rtl_unicode_string_unique_ptr(unicode_string* ptr):heap_ptr(ptr){}
	rtl_unicode_string_unique_ptr(rtl_unicode_string_unique_ptr const&)=delete;
	rtl_unicode_string_unique_ptr& operator=(rtl_unicode_string_unique_ptr const&)=delete;
	constexpr rtl_unicode_string_unique_ptr(rtl_unicode_string_unique_ptr&& other) noexcept:heap_ptr(other.heap_ptr)
	{
		other.heap_ptr=nullptr;
	}
	rtl_unicode_string_unique_ptr& operator=(rtl_unicode_string_unique_ptr&& other) noexcept
	{
		if(other.heap_ptr==heap_ptr)
			return *this;
		if(heap_ptr)[[likely]]
			rtl_free_unicode_string(heap_ptr);
		heap_ptr=other.heap_ptr;
		other.heap_ptr=nullptr;
		return *this;
	}
	~rtl_unicode_string_unique_ptr()
	{
		if(heap_ptr)[[likely]]
			rtl_free_unicode_string(heap_ptr);
	}
};

}