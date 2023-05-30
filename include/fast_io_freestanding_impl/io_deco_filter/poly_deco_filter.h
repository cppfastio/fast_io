#pragma once

namespace fast_io
{

template<buffer_mode md,
	::std::integral inin_char_type,::std::integral inout_char_type,
	::std::integral outin_char_type,::std::integral outout_char_type>
struct poly_deco_mode
{
	using input_input_char_type = inin_char_type;
	using input_output_char_type = inout_char_type;
	using output_input_char_type = outin_char_type;
	using output_output_char_type = outout_char_type;
	static inline constexpr buffer_mode mode = md;
};

namespace details
{

template<typename T>
struct basic_poly_deco_filter_base
{
	using traits_type = T;
	using input_input_char_type = typename traits_type::input_input_char_type;
	using input_output_char_type = typename traits_type::input_output_char_type;
	using output_input_char_type = typename traits_type::output_input_char_type;
	using output_output_char_type = typename traits_type::output_output_char_type;
	virtual constexpr deco_result<output_input_char_type,output_output_char_type>
		output_process_chars_impl(output_input_char_type const*,output_input_char_type const*,
			output_output_char_type*,output_output_char_type*)=0;
	virtual constexpr deco_result<input_input_char_type,input_output_char_type>
		input_process_chars_impl(input_input_char_type const*,input_input_char_type const*,
			input_output_char_type*,input_output_char_type*)=0;
	virtual ~basic_poly_deco_filter_base() = default;
};

template<typename T,typename P>
struct basic_poly_deco_filter_derv:basic_poly_deco_filter_base<T>
{
	using traits_type = T;
	using input_input_char_type = typename traits_type::input_input_char_type;
	using input_output_char_type = typename traits_type::input_output_char_type;
	using output_input_char_type = outin_char_type;
	using output_output_char_type = outout_char_type;
	T t;
	constexpr basic_poly_deco_filter_derv()
	virtual constexpr deco_result<output_input_char_type,output_output_char_type> output_process_chars_impl(
			output_input_char_type const* inputfirst,output_input_char_type const* inputlast,
			output_output_char_type* outputfirst,output_output_char_type* outputlast)
	{
		return t.output_process_chars(inputfirst,inputlast,outputfirst,outputlast);
	}
	virtual constexpr deco_result<input_input_char_type,input_output_char_type> input_process_chars_impl(
			input_input_char_type const* inputfirst,input_input_char_type const* inputlast,
			input_output_char_type* outputfirst,input_output_char_type* outputlast)
	{
		return t.input_process_chars(inputfirst,inputlast,outputfirst,outputlast);
	}
};

}

template<typename T>
struct basic_poly_deco_filter_io_observer
{
	using traits_type = T;
	using input_input_char_type = typename traits_type::input_input_char_type;
	using input_output_char_type = typename traits_type::input_output_char_type;
	using output_input_char_type = outin_char_type;
	using output_output_char_type = outout_char_type;
	static inline constexpr buffer_mode mode = traits_type::mode;
};

template<typename T>
struct basic_poly_deco_filter_file
{
	using traits_type = T;
	using input_input_char_type = typename traits_type::input_input_char_type;
	using input_output_char_type = typename traits_type::input_output_char_type;
	using output_input_char_type = outin_char_type;
	using output_output_char_type = outout_char_type;
	static inline constexpr buffer_mode mode = traits_type::mode;
};

}
