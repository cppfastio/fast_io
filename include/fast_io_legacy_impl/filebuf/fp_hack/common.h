#pragma once

namespace fast_io::details
{

inline constexpr std::ios::openmode calculate_fstream_file_open_mode(open_mode om) noexcept
{
	std::ios::openmode ios_om{};
	if((om&open_mode::app)!=open_mode::none)
		ios_om=ios_om|std::ios::app;
	if((om&open_mode::text)==open_mode::none)
		ios_om=ios_om|std::ios::binary;
	if((om&open_mode::in)!=open_mode::none)
		ios_om=ios_om|std::ios::in;
	if((om&open_mode::out)!=open_mode::none)
		ios_om=ios_om|std::ios::out;
	if((om&open_mode::trunc)!=open_mode::none)
		ios_om=ios_om|std::ios::trunc;
#if defined(__cpp_lib_ios_noreplace)
	if((om&open_mode::noreplace)!=open_mode::noreplace)
		ios_om=ios_om|std::ios::noreplace;
#endif
	if(((om&open_mode::directory)!=open_mode::none)&&ios_om==std::ios::openmode{})
		ios_om=ios_om|std::ios::in;
	return ios_om;
}

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
