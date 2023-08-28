#pragma once

namespace fast_io
{

template<typename T>
struct deco_partial_adapter
{
	using input_char_type = typename T::input_char_type;
	using output_char_type = typename T::output_char_type;
	static inline constexpr
		::std::size_t remained_max{T::remained_max};
	static inline constexpr
		::std::size_t output_final_max{T::output_final_max};
	using decorator_type = T;
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
	T deco;
	::std::size_t remained{};
	input_char_type remained_buffer[remained_max];
	inline constexpr deco_result<input_char_type,output_char_type>
		process_chars(input_char_type const *fromfirst,input_char_type const *fromlast,
				output_char_type *tofirst,output_char_type *tolast) noexcept(noexcept(deco.process_chars(fromfirst,fromlast,tofirst,tolast)))
	{
		::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
		::std::size_t diff{remained_max-remained};
		::std::size_t tocopy{diff};
		bool smaller{fromdiff<tocopy};
		if(smaller)
		{
			tocopy = fromdiff;
		}
		::fast_io::details::non_overlapped_copy_n(fromfirst,tocopy,remained_buffer);
		if(fromdiff<diff)
		{
			remained+=tocopy;
			return {fromlast,tofirst};
		}
		auto [fromit,toit]=deco.process_chars(fromfirst,fromlast,tofirst,tolast);
		remained=0;
		if(fromit!=fromlast&&toit!=tolast)
		{
			remained=static_cast<::std::size_t>(fromlast-fromit);
			::fast_io::details::non_overlapped_copy_n(fromit,remained,remained_buffer);
			fromit=fromlast;
		}
		return {fromit,toit};
	}

	inline constexpr output_char_type* do_final(output_char_type *tofirst,output_char_type *tolast) noexcept(noexcept(deco.do_final(remained_buffer,remained_buffer+remained,tofirst,tolast)))
	{
		return deco.do_final(remained_buffer,remained_buffer+remained,tofirst,tolast);
	}
};

}

