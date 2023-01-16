#pragma once

namespace fast_io::details
{

template<typename T>
concept stream_char_type_requirement = requires()
{
	typename std::remove_cvref_t<T>::char_type;
};

template<typename T>
concept input_stream_impl = 
(stream_char_type_requirement<T>||
(requires()
{
	typename std::remove_cvref_t<T>::input_char_type;
})
)&&requires(T&& in,typename std::remove_cvref_t<T>::char_type* b)
{
	read(in,b,b);
};

template<typename T>
concept output_stream_impl = (stream_char_type_requirement<T>||
(requires()
{
	typename std::remove_cvref_t<T>::output_char_type;
})
)&&requires(T&& out,typename std::remove_cvref_t<T>::char_type const* b)
{
	write(out,b,b);
};

template<typename T>
concept mutex_stream_impl = requires(T&& t)
{
	t.lock();
	t.unlock();
	t.unlocked_handle();
};

template<typename T>
concept random_access_stream_impl = requires(T&& t)
{
	seek(t,5);
};

template<typename T>
concept buffer_input_stream_impl = requires(T&& in)
{
	ibuffer_begin(in);
	ibuffer_set_curr(in,ibuffer_curr(in));
	ibuffer_end(in);
	{ibuffer_underflow(in)}->std::convertible_to<bool>;
};

template<typename T>
concept contiguous_input_stream_impl = requires(T&& in)
{
	ibuffer_underflow_never(in);
};

template<typename T>
concept contiguous_output_stream_impl = requires(T&& out)
{
	obuffer_overflow_never(out);
};

template<typename T>
concept refill_buffer_input_stream_impl = requires(T&& in)
{
	{irefill(in)}->std::convertible_to<bool>;
};

template<typename T>
concept buffer_output_stream_impl = requires(T&& out,typename std::remove_cvref_t<T>::char_type ch)
{
	obuffer_begin(out);
	obuffer_end(out);
	obuffer_set_curr(out,obuffer_curr(out));
	obuffer_overflow(out,ch);
};

template<typename T>
concept constant_buffer_output_stream_impl = requires(T&& out)
{
	{obuffer_constant_size(io_reserve_type<typename std::remove_cvref_t<T>::char_type,std::remove_cvref_t<decltype(out)>>)}->std::same_as<std::size_t>;
	obuffer_constant_flush_prepare(out);
};

template<typename T>
concept flush_output_stream_impl = requires(T&& out)
{
	flush(out);
};

template<typename T>
concept fill_nc_output_stream_impl = requires(T&& out,std::size_t n,typename std::remove_cvref_t<T>::char_type ch)
{
	fill_nc_define(out,n,ch);
};

template<typename T>
concept dynamic_output_stream_impl = requires(T&& out,std::size_t size,typename std::remove_cvref_t<T>::char_type* ptr)
{
	oreserve(out,size);
	oshrink_to_fit(out);
};

template<typename T>
concept zero_copy_input_stream_impl = requires(T&& in)
{
	zero_copy_in_handle(in);
};

template<typename T>
concept zero_copy_output_stream_impl = requires(T&& out)
{
	zero_copy_out_handle(out);
};

template<typename T>
concept redirect_stream_impl = requires(T&& h)
{
	redirect_handle(h);
};

template<typename T>
concept status_stream_impl = requires(T&& stm)
{
	typename std::remove_cvref_t<T>::status_type;
};

template<typename T>
concept scatter_input_stream_impl = requires(T&& in,io_scatters_t sp)
{
	scatter_read(in,sp);
};

template<typename T>
concept scatter_output_stream_impl = requires(T&& out,io_scatters_t sp)
{
	scatter_write(out,sp);
};

template<typename T>
concept scatter_constant_output_stream_impl = requires(T&& out,io_scatter_t const* sp)
{
	scatter_constant_write<1>(out,sp);
};


template<std::integral ch_type>
struct dummy_buffer_output_stream
{
	using char_type = ch_type;
};

template<std::integral char_type>
inline constexpr dummy_buffer_output_stream<char_type> io_value_handle(dummy_buffer_output_stream<char_type>) noexcept
{
	return {};
}

template<std::integral char_type>
inline constexpr char_type* obuffer_begin(dummy_buffer_output_stream<char_type>) noexcept
{
	return nullptr;
}

template<std::integral char_type>
inline constexpr char_type* obuffer_curr(dummy_buffer_output_stream<char_type>) noexcept
{
	return nullptr;
}

template<std::integral char_type>
inline constexpr char_type* obuffer_end(dummy_buffer_output_stream<char_type>) noexcept
{
	return nullptr;
}

template<std::integral char_type>
inline constexpr void obuffer_set_curr(dummy_buffer_output_stream<char_type>,char_type*) noexcept{}

template<std::integral char_type>
inline constexpr void obuffer_overflow(dummy_buffer_output_stream<char_type>,char_type) noexcept{}

template<std::integral char_type>
inline constexpr void write(dummy_buffer_output_stream<char_type>,char_type const*,char_type const*) noexcept{}

template<std::integral ch_type>
struct dummy_buffer_input_stream
{
	using char_type = ch_type;
};

template<std::integral char_type>
inline constexpr dummy_buffer_input_stream<char_type> io_value_handle(dummy_buffer_input_stream<char_type>) noexcept
{
	return {};
}

template<std::integral char_type>
inline constexpr char_type* ibuffer_begin(dummy_buffer_input_stream<char_type>) noexcept
{
	return nullptr;
}

template<std::integral char_type>
inline constexpr char_type* ibuffer_curr(dummy_buffer_input_stream<char_type>) noexcept
{
	return nullptr;
}

template<std::integral char_type>
inline constexpr char_type* ibuffer_end(dummy_buffer_input_stream<char_type>) noexcept
{
	return nullptr;
}

template<std::integral char_type>
inline constexpr void ibuffer_set_curr(dummy_buffer_input_stream<char_type>,char_type*) noexcept{}

template<std::integral char_type>
inline constexpr bool ibuffer_underflow(dummy_buffer_input_stream<char_type>) noexcept
{
	return true;
}

template<std::integral char_type>
inline constexpr char_type* read(dummy_buffer_input_stream<char_type>,char_type* first,char_type*) noexcept
{
	return first;
}

#if 0

template<typename T>
concept async_input_stream64_impl = requires(T in,typename T::char_type* first,typename T::char_type* last,std::int_least64_t offset)
{
	async_read_define64(in,first,last,offset,[](typename T::char_type*,::std::errc ec)
	{
	});
};

template<typename T>
concept async_input_stream64_impl = requires(T in,typename T::char_type* first,typename T::char_type* last,std::int_least64_t offset)
{
	async_write_define64(in,first,last,offset,[](typename T::char_type*,::std::errc ec)
	{
	});
};

#endif

}
