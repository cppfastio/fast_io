#pragma once

namespace fast_io
{

template<typename inputdeco,
	typename outputdeco>
struct basic_bidirectional_decorator_adaptor
{
	using output_char_type = typename outputdeco::output_char_type;
	using input_char_type = typename inputdeco::output_char_type;

	using input_decorator_type = inputdeco;
	using output_decorator_type = outputdeco;
	input_decorator_type idecorator;
	output_decorator_type odecorator;
};

template<typename T>
struct basic_bidirectional_decorator_adaptor_ref
{
	using bidirectional_decorator_adaptor_type = T;
	using output_char_type = typename bidirectional_decorator_adaptor_type::output_char_type;
	using input_char_type = typename bidirectional_decorator_adaptor_type::output_char_type;
	bidirectional_decorator_adaptor_type *bdoaptr{};
	inline constexpr deco_result<input_char_type,output_char_type>
		output_process_chars(input_char_type const *fromfirst,input_char_type const *fromlast,
				output_char_type *tofirst,output_char_type *tolast)
	{
		return bdoaptr->odecorator.process_chars(fromfirst,fromlast,tofirst,tolast);
	}

	inline constexpr deco_result<output_char_type,input_char_type>
		input_process_chars(output_char_type const *fromfirst,output_char_type const *fromlast,
				input_char_type *tofirst,input_char_type *tolast)
	{
		return bdoaptr->idecorator.process_chars(fromfirst,fromlast,tofirst,tolast);
	}
};

template<typename inputdeco,
	typename outputdeco>
inline constexpr basic_bidirectional_decorator_adaptor_ref<basic_bidirectional_decorator_adaptor<inputdeco,outputdeco>>
	output_decorators_ref_define(basic_bidirectional_decorator_adaptor<inputdeco,outputdeco>& iod) noexcept
{
	return {__builtin_addressof(iod)};
}

template<typename inputdeco,
	typename outputdeco>
inline constexpr basic_bidirectional_decorator_adaptor_ref<basic_bidirectional_decorator_adaptor<inputdeco,outputdeco>>
	input_decorators_ref_define(basic_bidirectional_decorator_adaptor<inputdeco,outputdeco>& iod) noexcept
{
	return {__builtin_addressof(iod)};
}

}
