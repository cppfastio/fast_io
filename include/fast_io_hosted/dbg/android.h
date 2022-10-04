#pragma once

namespace fast_io
{

struct android_logmessage_meta_base
{
	std::int_least32_t priority{4};
	char const* tag{""};
};

struct android_logmessage_meta_v30
{
	std::int_least32_t buffer_id{};
	std::int_least32_t priority{4};
	char const* tag{""};
	char const* file{""};
	std::uint_least32_t line{};
};

enum class android_logger_family_tag
{
base,
v30,
#if defined(__ANDROID__) && __ANDROID_API__ < 30
native=base,
#else
native=v30,
#endif
};

template<android_logger_family_tag fam,std::integral ch_type>
struct basic_android_family_logger
{
	using char_type = ch_type;
	std::conditional_t<fam==android_logger_family_tag::base,android_logmessage_meta_base,android_logmessage_meta_v30> meta{};
};

template<android_logger_family_tag fam,std::integral char_type>
requires (sizeof(basic_android_family_logger<fam,char_type>)<=(sizeof(std::size_t)*2))
inline constexpr basic_android_family_logger<fam,char_type> io_value_handle(basic_android_family_logger<fam,char_type> lg) noexcept
{
	return lg;
};

template<std::integral ch_type>
using basic_android_logger=basic_android_family_logger<android_logger_family_tag::native,ch_type>;

using android_logger = basic_android_logger<char>;
using wandroid_logger = basic_android_logger<wchar_t>;
using u8android_logger = basic_android_logger<char8_t>;
using u16android_logger = basic_android_logger<char16_t>;
using u32android_logger = basic_android_logger<char32_t>;

namespace details
{

struct cstr_ptr
{
	void* p{};
	constexpr cstr_ptr() noexcept = default;
	explicit cstr_ptr(std::size_t n) noexcept
	{
		if(n==SIZE_MAX)
			__builtin_trap();
		p=__builtin_malloc(n+1u);
		if(p==nullptr)
			__builtin_trap();
		reinterpret_cast<char*>(p)[n]=0;
	}
	cstr_ptr(cstr_ptr const&)=delete;
	cstr_ptr& operator=(cstr_ptr const&)=delete;
	~cstr_ptr()
	{
		__builtin_free(p);
	}
};

inline void copy_to_cstr_buffer(void* buffer,void const* source,std::size_t n) noexcept
{
	if(n)
		__builtin_memcpy(buffer,source,n);
}

template<typename callback>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline void cstr_concat_write_impl(void const* first,std::size_t n,callback func)
{
	cstr_ptr p{n};
	copy_to_cstr_buffer(p.p,first,n);
	func(reinterpret_cast<char const*>(p.p));
}

template<typename callback>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline void cstr_concat_writev_impl(io_scatter_t const* first,std::size_t n,callback func)
{
	std::size_t total_len{};
	for(auto i{first},e{i+n};i!=e;++i)
		total_len=::fast_io::details::intrinsics::add_or_overflow_die(total_len,i->len);
	if(total_len==SIZE_MAX)
		__builtin_trap();
	cstr_ptr p{total_len};
	std::byte* start{reinterpret_cast<std::byte*>(p.p)};
	for(auto i{first},e{i+n};i!=e;++i)
	{
		copy_to_cstr_buffer(start,i->base,i->len);
		start+=i->len;
	}
	func(reinterpret_cast<char const*>(p.p));
}

template<android_logger_family_tag fam>
inline constexpr bool android_smaller_than_pass_by_mem_size{sizeof(basic_android_family_logger<fam,char>)<=(sizeof(std::size_t)*2)};

extern int my_android_log_write(std::int_least32_t,char const*,char const*) noexcept __asm__("__android_log_write");

struct android_logmessage_v30
{
	std::size_t size{sizeof(android_logmessage_v30)};
	std::int_least32_t buffer_id{};
	std::int_least32_t priority{4};
	char const* tag{""};
	char const* file{""};
	std::uint_least32_t line{};
	char const* text;
};

extern void my_android_log_write_log_message(android_logmessage_v30* log_message) noexcept __asm__("__android_log_write_log_message");

inline void my_android_log_write_impl(std::int_least32_t prio,char const* tag,char const* text) noexcept
{
	my_android_log_write(prio,tag,text);
}

inline void my_android_log_write_message_impl(android_logmessage_meta_v30 const* meta,char const* text) noexcept
{
	android_logmessage_v30 v{.buffer_id=meta->buffer_id,.priority=meta->priority,.tag=meta->tag,.file=meta->file,.line=meta->line,.text=text};
	my_android_log_write_log_message(__builtin_addressof(v));
}


struct my_android_log_write_common
{
	android_logmessage_meta_base b{};
	void operator()(char const* text)
	{
		my_android_log_write_impl(b.priority,b.tag,text);
	}
};

inline void android_logger_write_impl(android_logmessage_meta_base b,void const* base,std::size_t n)
{
	cstr_concat_write_impl(base,n,my_android_log_write_common{b});
}

inline void android_logger_writev_impl(android_logmessage_meta_base b,io_scatter_t const* first,std::size_t n)
{
	cstr_concat_writev_impl(first,n,my_android_log_write_common{b});
}

struct my_android_log_write_v30_common
{
	android_logmessage_meta_v30 const* pmeta{};
	void operator()(char const* text)
	{
		my_android_log_write_message_impl(pmeta,text);
	}
};

inline void android_logger_write_impl(android_logmessage_meta_v30 const& meta,void const* base,std::size_t n)
{
	cstr_concat_write_impl(base,n,my_android_log_write_v30_common{__builtin_addressof(meta)});
}

inline void android_logger_writev_impl(android_logmessage_meta_v30 const& meta,io_scatter_t const* first,std::size_t n)
{
	cstr_concat_writev_impl(first,n,my_android_log_write_v30_common{__builtin_addressof(meta)});
}
}

template<android_logger_family_tag fam,std::integral char_type,std::contiguous_iterator Iter>
inline void write(basic_android_family_logger<fam,char_type> const& b,Iter first,Iter last)
{
	::fast_io::details::android_logger_write_impl(b.meta,::std::to_address(first),static_cast<std::size_t>(last-first)*sizeof(*first));
}

template<android_logger_family_tag fam,std::integral char_type>
inline void scatter_write(basic_android_family_logger<fam,char_type> const& b,io_scatters_t scatters)
{
	::fast_io::details::android_logger_writev_impl(b.meta,scatters.base,scatters.len);
}

#if (!defined(__ANDROID__) || __ANDROID_API__ > 30) && __cpp_lib_source_location >= 201907

namespace details
{

template<std::integral char_type>
inline constexpr auto android_dbg_impl(std::int_least32_t priorit,char const* tg,std::source_location const& loc)
{
	return basic_android_logger<char_type>{.meta={.priority=priorit,.tag=tg,.file=loc.file_name(),.line=static_cast<std::uint_least32_t>(loc.line())}};
}

}

inline constexpr auto dbg(std::int_least32_t priority=4,char const* tg="",std::source_location loc=std::source_location::current()) noexcept
{
	return ::fast_io::details::android_dbg_impl<char>(priority,tg,loc);
}
inline constexpr auto wdbg(std::int_least32_t priority=4,char const* tg="",std::source_location loc=std::source_location::current()) noexcept
{
	return ::fast_io::details::android_dbg_impl<wchar_t>(priority,tg,loc);
}
inline constexpr auto u8dbg(std::int_least32_t priority=4,char const* tg="",std::source_location loc=std::source_location::current()) noexcept
{
	return ::fast_io::details::android_dbg_impl<char8_t>(priority,tg,loc);
}
inline constexpr auto u16dbg(std::int_least32_t priority=4,char const* tg="",std::source_location loc=std::source_location::current()) noexcept
{
	return ::fast_io::details::android_dbg_impl<char16_t>(priority,tg,loc);
}
inline constexpr auto u32dbg(std::int_least32_t priority=4,char const* tg="",std::source_location loc=std::source_location::current()) noexcept
{
	return ::fast_io::details::android_dbg_impl<char32_t>(priority,tg,loc);
}

#else
inline constexpr auto dbg(std::int_least32_t priorit=4,char const* tg="") noexcept
{
	return android_logger{.meta={.priority=priorit,.tag=tg}};
}
inline constexpr auto wdbg(std::int_least32_t priorit=4,char const* tg="") noexcept
{
	return wandroid_logger{.meta={.priority=priorit,.tag=tg}};
}
inline constexpr auto u8dbg(std::int_least32_t priorit=4,char const* tg="") noexcept
{
	return u8android_logger{.meta={.priority=priorit,.tag=tg}};
}
inline constexpr auto u16dbg(std::int_least32_t priorit=4,char const* tg="") noexcept
{
	return u16android_logger{.meta={.priority=priorit,.tag=tg}};
}
inline constexpr auto u32dbg(std::int_least32_t priorit=4,char const* tg="") noexcept
{
	return u32android_logger{.meta={.priority=priorit,.tag=tg}};
}
#endif

}
