#pragma once

/*
 * async_iobuffer_print_decay — the basic_io_buffer_ref branch of
 * async_print_decay, kept at the io_buffer layer where iobptr /
 * iobuffer_allocate / detach live. Found by ADL on the stream ref —
 * the core decay only names it inside a requires check.
 */

namespace fast_io
{

namespace details
{

/* allocate the stream's output buffer through its own allocator and
 * point obuffer at it */
template <typename outstmtype>
inline void async_iobuffer_print_alloc_buffer(outstmtype outstm)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		::fast_io::typed_generic_allocator_adapter<
			typename outstmtype::io_buffer_type::traits_type::allocator_type,
			typename outstmtype::output_char_type>::throws_on_allocation_failure)
{
	using traits_type = typename outstmtype::io_buffer_type::traits_type;
	using char_type = typename outstmtype::output_char_type;
	auto &obuffer{outstm.iobptr->output_buffer};
	auto *begin{::fast_io::details::io_buffer::iobuffer_allocate<
		char_type, typename traits_type::allocator_type>(
		outstm.iobptr->allocator_handle, traits_type::output_buffer_size)};
	obuffer.buffer_begin = begin;
	obuffer.buffer_curr = begin;
	obuffer.buffer_end = begin + traits_type::output_buffer_size;
}

} // namespace details

template <bool line, typename async_scheduler_type, typename outstmtype, typename... Args>
	requires requires { typename outstmtype::io_buffer_type; }
inline auto async_iobuffer_print_decay(async_scheduler_type sched,
									   ::fast_io::posix_statx_timestamp_opt timeout,
									   outstmtype outstm, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	using char_type = typename outstmtype::output_char_type;
	using iobuf_type = typename outstmtype::io_buffer_type;
	using traits_type = typename iobuf_type::traits_type;
	using detached_type =
		typename ::fast_io::details::async_print_detached_type<outstmtype>::type;
	constexpr ::std::size_t bufsize{traits_type::output_buffer_size};
	/* payload buffers follow the device's allocator — the iobuf's own
	 * allocator here. A status allocator carries its handle, so the
	 * payload is the handle-owning strlike buffer */
	using payload_alloc_type =
		::fast_io::operations::decay::output_stream_allocator_t<
			outstmtype, ::fast_io::native_global_allocator>;
	using payload_typed_alloc_type =
		::fast_io::typed_generic_allocator_adapter<payload_alloc_type, char_type>;
	constexpr bool payload_status{payload_typed_alloc_type::has_status};
	using payload_string_type =
		::fast_io::details::async_print_strlike_buffer<char_type, payload_alloc_type>;
	auto handle_ref{::fast_io::operations::output_stream_ref(outstm.iobptr->handle)};
	using ret_awaiter = ::fast_io::details::async_print_awaiter<
		async_scheduler_type, decltype(handle_ref), detached_type, payload_string_type>;
	using work_state_type = typename ret_awaiter::work_state_type;
	auto &obuffer{outstm.iobptr->output_buffer};
	auto make_work{[&]() FAST_IO_HERBCEPTIONS_THROWS {
		return ::fast_io::details::async_new_state_plain<work_state_type>(sched);
	}};
	auto alloc_buffer{[&]() FAST_IO_HERBCEPTIONS_THROWS {
		::fast_io::details::async_iobuffer_print_alloc_buffer(outstm);
	}};
/* every argument formattable into raw buffer space: reserve,
	 * dynamic_reserve, scatter_printable or a ready-made scatter */
	constexpr bool all_direct{
		((::fast_io::reserve_printable<char_type, ::std::remove_cvref_t<Args>> ||
		  ::fast_io::dynamic_reserve_printable<char_type, ::std::remove_cvref_t<Args>> ||
		  ::fast_io::scatter_printable<char_type, ::std::remove_cvref_t<Args>> ||
		  ::std::same_as<::std::remove_cvref_t<Args>,
						 ::fast_io::basic_io_scatter_t<char_type>>) &&
		 ...)};

	if constexpr (all_direct)
	{
		/* total size: the constexpr reserve sum plus runtime sizes of
		 * scatter/dynamic args */
		::std::size_t needed{
			::fast_io::details::compute_total_normal_reserved_size<char_type, line,
																   Args...>()};
		template for (constexpr auto i :
					  ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
		{
			using arg_type = ::std::remove_cvref_t<Args...[i]>;
			if constexpr (::std::same_as<arg_type,
										 ::fast_io::basic_io_scatter_t<char_type>>)
			{
				needed += args...[i].len;
			}
			else if constexpr (::fast_io::scatter_printable<char_type, arg_type>)
			{
				needed += print_reserve_size(
							  ::fast_io::io_reserve_type<char_type, arg_type>, args...[i])
							  .len;
			}
			else if constexpr (::fast_io::dynamic_reserve_printable<char_type,
																	arg_type>)
			{
				needed += print_reserve_size(
					::fast_io::io_reserve_type<char_type, arg_type>, args...[i]);
			}
		}
		if (obuffer.buffer_begin != nullptr &&
			needed <= static_cast<::std::size_t>(obuffer.buffer_end -
												 obuffer.buffer_curr))
		{
			/* buffer has room: format straight into it — the sync print
			 * fast path; nothing is submitted. Pending bytes ahead are
			 * untouched — appending behind them preserves order */
			auto *curr{obuffer.buffer_curr};
			template for (constexpr auto i :
						  ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
			{
				curr = ::fast_io::details::async_print_format_one<char_type>(
					curr, args...[i]);
			}
			if constexpr (line)
			{
				*curr = ::fast_io::char_literal_v<u8'\n', char_type>;
				++curr;
			}
			obuffer.buffer_curr = curr;
			return ret_awaiter{};
		}
		if (needed <= bufsize)
		{
			if (obuffer.buffer_curr == obuffer.buffer_begin)
			{
				/* empty or never-allocated buffer: ensure storage and
				 * format into it — still no I/O */
				if (obuffer.buffer_begin == nullptr)
				{
					alloc_buffer();
				}
			}
			else
			{
				/* pending bytes would have to drain before the buffer can
				 * take this print — detach them so they ride the submission
				 * zero-copy and format into a fresh buffer right away */
				ret_awaiter ret;
				ret.sched = sched;
				ret.outstm = handle_ref;
				ret.timeout = timeout;
				ret.work = make_work();
				ret.work->detached = output_stream_buffer_detach_define(outstm);
				/* detach leaves the buffer null — allocate a fresh one */
				alloc_buffer();
				auto *curr{obuffer.buffer_curr};
				template for (constexpr auto i :
							  ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
				{
					curr = ::fast_io::details::async_print_format_one<char_type>(
						curr, args...[i]);
				}
				if constexpr (line)
				{
					*curr = ::fast_io::char_literal_v<u8'\n', char_type>;
					++curr;
				}
				obuffer.buffer_curr = curr;
				return ret;
			}
			auto *curr{obuffer.buffer_curr};
			template for (constexpr auto i :
						  ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
			{
				curr = ::fast_io::details::async_print_format_one<char_type>(
					curr, args...[i]);
			}
			if constexpr (line)
			{
				*curr = ::fast_io::char_literal_v<u8'\n', char_type>;
				++curr;
			}
			obuffer.buffer_curr = curr;
			return ret_awaiter{};
		}
	}
	/* oversized or non-direct arguments: format into the payload
	 * first, then decide where the bytes go. The strlike payload
	 * goes through the ordinary print machinery, so print_define
	 * arguments work here too */
	payload_string_type payload = [&]() FAST_IO_HERBCEPTIONS_THROWS -> payload_string_type {
		payload_string_type str;
		if constexpr (payload_status)
		{
			str = payload_string_type{::fast_io::details::
										  print_output_stream_allocator_handle<outstmtype,
																			   payload_typed_alloc_type>(
											  outstm)};
		}
		::fast_io::operations::decay::print_freestanding_decay<line>(
			io_strlike_ref(::fast_io::io_alias, str), args...);
		return str;
	}();
	if (obuffer.buffer_begin != nullptr &&
		payload.size() <= static_cast<::std::size_t>(obuffer.buffer_end - obuffer.buffer_curr))
	{
		/* fits in the buffer (behind any pending bytes — order kept):
		 * pure append, nothing submitted */
		::fast_io::details::non_overlapped_copy_n(
			payload.data(), payload.size(), obuffer.buffer_curr);
		obuffer.buffer_curr += payload.size();
		return ret_awaiter{};
	}
	ret_awaiter ret;
	if (obuffer.buffer_curr != obuffer.buffer_begin)
	{
		/* pending bytes must reach the device before the payload —
		 * detach them so the submission carries them zero-copy and the
		 * fresh buffer can take what fits */
		ret.work = make_work();
		ret.work->detached = output_stream_buffer_detach_define(outstm);
	}
	if (obuffer.buffer_begin == nullptr &&
		payload.size() <= bufsize)
	{
		alloc_buffer();
	}
	if (obuffer.buffer_begin != nullptr &&
		payload.size() <= static_cast<::std::size_t>(obuffer.buffer_end - obuffer.buffer_curr))
	{
		/* fits the (fresh) buffer: only the detached pending bytes
		 * need a submission, or nothing at all */
		::fast_io::details::non_overlapped_copy_n(
			payload.data(), payload.size(), obuffer.buffer_curr);
		obuffer.buffer_curr += payload.size();
	}
	else
	{
		if (ret.work == nullptr)
		{
			ret.work = make_work();
		}
		ret.work->payload = ::std::move(payload);
	}
	if (ret.work == nullptr)
	{
		return ret_awaiter{};
	}
	ret.sched = sched;
	ret.outstm = handle_ref;
	ret.timeout = timeout;
	return ret;
}

} // namespace fast_io
