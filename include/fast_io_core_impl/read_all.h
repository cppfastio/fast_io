#pragma once

namespace fast_io
{
namespace details
{

template <bool throweh = true, ::fast_io::input_stream input>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr ::std::conditional_t<throweh, void, bool>
read_all_impl_decay_cold(input in, typename input::char_type *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto const e{first + count};
	while (first != e)
	{
		auto it{::fast_io::details::read_some_impl(in, first, static_cast<::std::size_t>(e - first))};
		if (it == first) [[unlikely]]
		{
			if constexpr (throweh)
			{
				::fast_io::herbceptions::throws_parse_errc(::fast_io::freestanding::parse_errc::end_of_file);
			}
			else
			{
				return false;
			}
		}
		first = it;
	}
	if constexpr (!throweh)
	{
		return true;
	}
}


template <::fast_io::input_stream input>
inline constexpr void read_all_impl_decay(input in, typename input::char_type *first, ::std::size_t count)
{
	::std::ptrdiff_t diff{count};
	if constexpr (buffer_input_stream<input>)
	{
		auto curr{ibuffer_curr(in)};
		auto end{ibuffer_end(in)};
		::std::ptrdiff_t remain_space{end - curr};
		if (diff < remain_space) [[likely]]
		{
			non_overlapped_copy_n(curr, static_cast<::std::size_t>(diff), first);
			curr += diff;
			ibuffer_set_curr(in, curr);
		}
		else
		{
			return read_all_impl_decay_cold(in, first, count);
		}
	}
	else
	{
		return read_all_impl_decay_cold(in, first, count);
	}
}

template <::fast_io::input_stream input, ::std::input_or_output_iterator Iter>
	requires ::std::same_as<typename input::char_type, char> ||
			 ::std::same_as<typename input::char_type, ::std::iter_value_t<Iter>>
inline constexpr void read_all_impl(input in, Iter first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	using iter_char_type = ::std::iter_value_t<Iter>;
#if 0
	using char_type = typename input::char_type;
	if constexpr(::std::contiguous_iterator<Iter>)
	{
		if constexpr(::std::same_as<typename input::char_type,iter_char_type>)
		{
			read_all_impl_decay(in,::std::to_address(first),::std::to_address(last));
		}
		else
		{
			read_all_impl_decay(in,reinterpret_cast<char*>(::std::to_address(first)),reinterpret_cast<char*>(::std::to_address(last)));
		}
	}
	else
	{
		for(;first!=last;++first)
		{
			auto addr{__builtin_addressof(*first)};
			read_all_impl(in,addr,addr+1);
		}
	}
#else
	static_assert(::std::contiguous_iterator<Iter>);
	if constexpr (::std::same_as<typename input::char_type, iter_char_type>)
	{
		read_all_impl_decay(in, ::std::to_address(first), count);
	}
	else
	{
		read_all_impl_decay(in, reinterpret_cast<char *>(::std::to_address(first)),
							count * sizeof(iter_char_type));
	}
#endif
}

} // namespace details

template <typename input, ::std::forward_iterator Iter>
inline constexpr void read_all(input &&in, Iter first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::details::read_all_impl(::fast_io::operations::input_stream_ref(in), first, count);
}

template <typename input, ::std::ranges::forward_range rg>
inline constexpr void read_all(input &&in, rg &&r)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::details::read_all_impl(::fast_io::operations::input_stream_ref(in), ::std::ranges::begin(r),
									  static_cast<::std::size_t>(::std::ranges::distance(r)));
}

} // namespace fast_io
