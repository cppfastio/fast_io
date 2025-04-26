#pragma once

namespace fast_io
{

template <typename T>
struct deco_partial_adapter
{
	using input_char_type = typename T::input_char_type;
	using output_char_type = typename T::output_char_type;
	static inline constexpr ::std::size_t remained_max{T::remained_max};
	static inline constexpr ::std::size_t output_final_max{T::output_final_max};
	using decorator_type = T;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	T deco;
	::std::size_t remained{};
	input_char_type remained_buffer[remained_max];
	template <::std::integral char_type>
		requires(sizeof(char_type) == sizeof(input_char_type))
	inline constexpr deco_result<char_type, output_char_type>
	process_chars(char_type const *fromfirst, char_type const *fromlast, output_char_type *tofirst,
				  output_char_type *tolast)
	{
		if constexpr (!::std::same_as<char_type, input_char_type>)
		{
			if (__builtin_is_constant_evaluated())
			{
				::std::size_t fromdiff{static_cast<::std::size_t>(fromlast - fromfirst)};
				::fast_io::details::local_operator_new_array_ptr<input_char_type> fromptr(fromdiff);
				::fast_io::details::non_overlapped_copy_n(fromfirst, fromdiff, fromptr.ptr);
				auto [fromit, toit] = this->process_chars(fromptr.ptr, fromptr.ptr + fromdiff, tofirst, tolast);
				return {(fromit - fromptr.ptr) + fromfirst, toit};
			}
			using may_alias_input_char_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= input_char_type const *;
			auto [fromit, toit] =
				this->process_chars(reinterpret_cast<may_alias_input_char_ptr>(fromfirst),
									reinterpret_cast<may_alias_input_char_ptr>(fromlast), tofirst, tolast);
			return {(fromit - reinterpret_cast<may_alias_input_char_ptr>(fromfirst)) + fromfirst, toit};
		}
		else
		{
			::std::size_t fromdiff{static_cast<::std::size_t>(fromlast - fromfirst)};
			::std::size_t diff{remained_max - remained};
			auto remainedend{remained_buffer + remained_max};
			auto remainedit{remained_buffer + remained};
			if (fromdiff < diff)
			{
				auto remainedstop{::fast_io::details::non_overlapped_copy_n(fromfirst, fromdiff, remainedit)};
				auto [fromit, toit] = deco.process_chars(remained_buffer, remainedstop, tofirst, tolast);
				tofirst = toit;
				fromfirst += fromdiff;
				remained = static_cast<::std::size_t>(
					::fast_io::freestanding::copy_n(fromit, static_cast<::std::size_t>(remainedstop - fromit),
													remained_buffer) -
					remained_buffer);
				return {fromfirst, tofirst};
			}
			if (remained)
			{
				::fast_io::details::non_overlapped_copy_n(fromfirst, diff, remainedit);
				auto [fromit, toit] = deco.process_chars(remained_buffer, remainedend, tofirst, tolast);
				fromfirst += fromit - remainedit;
				tofirst = toit;
			}
			auto [fromit, toit] = deco.process_chars(fromfirst, fromlast, tofirst, tolast);
			remained = 0;
			if (fromit != fromlast && toit != tolast)
			{
				remained = static_cast<::std::size_t>(fromlast - fromit);
				::fast_io::details::non_overlapped_copy_n(fromit, remained, remained_buffer);
				fromit = fromlast;
			}
			return {fromit, toit};
		}
	}

	inline constexpr output_char_type *do_final(output_char_type *tofirst)
	{
		return deco.do_final(remained_buffer, remained_buffer + remained, tofirst);
	}
};

} // namespace fast_io
