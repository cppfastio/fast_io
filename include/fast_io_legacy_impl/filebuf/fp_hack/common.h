#pragma once

namespace fast_io::details
{

template<typename CharT, typename Traits>
struct filebuf_guard
{
	using filebuf_type = ::std::basic_filebuf<CharT,Traits>;
	filebuf_type* new_filebuf{};

	constexpr filebuf_guard() noexcept = default;
	explicit constexpr filebuf_guard(filebuf_type *fb) noexcept :
		new_filebuf{fb}
	{
	}
	filebuf_guard(filebuf_guard const&)=delete;
	filebuf_guard& operator=(filebuf_guard const&)=delete;

	~filebuf_guard()
	{
		if(new_filebuf)
			delete new_filebuf;
	}
	inline constexpr filebuf_type* release() noexcept
	{
		auto temp{new_filebuf};
		new_filebuf=nullptr;
		return temp;
	}
};

}
