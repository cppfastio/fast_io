#pragma once

namespace fast_io
{

struct scan_string_context
{
	bool copying{};
};

namespace details
{

template<bool noskipws,bool line,::fast_io::freestanding::forward_iterator Iter,typename traits,typename allocator_type>
inline constexpr ::fast_io::parse_result<Iter> scan_context_define_string_impl(bool& skip_space_done,Iter first,Iter last,::std::basic_string<::fast_io::freestanding::iter_value_t<Iter>,traits,allocator_type>& ref)
{
	Iter it{first};
	if constexpr(!noskipws&&!line)
	{
		if(!skip_space_done)
		{
			it=::fast_io::find_non_c_space(it,last);
			if(it==last)
				return {it,::fast_io::parse_code::partial};
			skip_space_done=true;
			ref.clear();
		}
	}
	Iter it_space{it};
	if constexpr(line)
		it_space=::fast_io::find_lf(it_space,last);
	else
		it_space=::fast_io::find_c_space(it_space,last);
	if constexpr(noskipws||line)
	{
		if(!skip_space_done)
		{
			ref.assign(it,it_space);
			skip_space_done=true;
		}
		else
			ref.append(it,it_space);
	}
	else
	{
		ref.append(it,it_space);
	}
	if(it_space==last)
		return {it_space,::fast_io::parse_code::partial};
	if constexpr(line)
		++it_space;
	return {it_space,::fast_io::parse_code::ok};
}

inline constexpr ::fast_io::parse_code scan_context_eof_string_define_impl(bool skip_space_done) noexcept
{
	if(skip_space_done)
		return ::fast_io::parse_code::ok;
	else
		return ::fast_io::parse_code::end_of_file;
}

template<bool noskipws,bool line>
inline constexpr ::fast_io::manipulators::scalar_flags string_default_scalar_flags{.noskipws=noskipws,.line=line};

}


template<std::integral char_type,::fast_io::manipulators::scalar_flags flags,typename traits,typename allocator_type>
inline constexpr io_type_t<scan_string_context> scan_context_type(io_reserve_type_t<char_type,::fast_io::manipulators::scalar_manip_t<flags,::std::basic_string<char_type,traits,allocator_type>&>>) noexcept
{
	return {};
}

template<::fast_io::freestanding::forward_iterator Iter,::fast_io::manipulators::scalar_flags flags,typename traits,typename allocator_type>
inline constexpr parse_result<Iter> scan_context_define(
	io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,::fast_io::manipulators::scalar_manip_t<flags,::std::basic_string<::fast_io::freestanding::iter_value_t<Iter>,traits,allocator_type>&>>,
	scan_string_context& skip_space_done,
	Iter first,Iter last,
	::fast_io::manipulators::scalar_manip_t<flags,::std::basic_string<::fast_io::freestanding::iter_value_t<Iter>,traits,allocator_type>&> str)
{
	return ::fast_io::details::scan_context_define_string_impl<flags.noskipws,flags.line>(skip_space_done.copying,first,last,str.reference);
}

template<::fast_io::manipulators::scalar_flags flags,std::integral char_type,typename traits,typename allocator_type>
inline constexpr ::fast_io::parse_code scan_context_eof_define(io_reserve_type_t<char_type,::fast_io::manipulators::scalar_manip_t<flags,::std::basic_string<char_type,traits,allocator_type>&>>,
	scan_string_context skip_space_done,::fast_io::manipulators::scalar_manip_t<flags,::std::basic_string<char_type,traits,allocator_type>&> str) noexcept
{
	if constexpr(flags.line||flags.noskipws)
	{
		if(str.reference.empty())
			return ::fast_io::parse_code::end_of_file;
		else
			return ::fast_io::parse_code::ok;
	}
	else
		return ::fast_io::details::scan_context_eof_string_define_impl(skip_space_done.copying);
}

template<std::integral char_type,typename traits,typename allocator_type>
inline constexpr ::fast_io::manipulators::scalar_manip_t<::fast_io::details::string_default_scalar_flags<false,false>,::std::basic_string<char_type,traits,allocator_type>&> scan_alias_define(io_alias_t,::std::basic_string<char_type,traits,allocator_type>& t) noexcept
{
	return {t};
}

namespace manipulators
{

template<std::integral char_type,typename traits,typename allocator_type>
inline constexpr ::fast_io::manipulators::scalar_manip_t<::fast_io::details::string_default_scalar_flags<false,true>,::std::basic_string<char_type,traits,allocator_type>&> line_get(::std::basic_string<char_type,traits,allocator_type>& line_str) noexcept
{
	return {line_str};
}

}

}
