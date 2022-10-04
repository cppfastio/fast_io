#pragma once

namespace fast_io
{
//This is DAMN bullshit
struct task
{
struct promise_type
{
constexpr auto get_return_object() { return task{}; }
constexpr auto initial_suspend() { return std::suspend_never{}; }
constexpr auto final_suspend() { return std::suspend_never{}; }
void unhandled_exception() { std::terminate(); }
constexpr void return_void() {}
};
};

namespace details
{

template<typename stm,::std::input_iterator Iter,bool write>
requires ((write&&async_output_stream<stm>)||(!write&&async_input_stream<stm>))
class async_io_coroutine
{
public:
	typename io_async_scheduler_t<stm>::type& scheduler;
	stm& stream;
	Iter first,last;
	std::ptrdiff_t offset{write?-1:0};
	std::size_t transferred_bytes{};
	int err{};
	typename io_async_overlapped_t<stm>::type overlapped;
	constexpr bool await_ready() const { return false; }
	constexpr Iter await_resume() const
	{
		if(err)
			throw_posix_error(err);
		return first+transferred_bytes/sizeof(*first);
	}
	void await_suspend(std::coroutine_handle<> handle)
	{
		overlapped=typename io_async_overlapped_t<stm>::type(std::in_place,[handle,this](std::size_t calb,int errn)
		{
			this->transferred_bytes=calb;
			this->err=errn;
			handle.resume();
		});
		if constexpr(write)
			async_read_callback(scheduler,stream,first,last,overlapped,offset);
		else
			async_write_callback(scheduler,stream,first,last,overlapped,offset);
	}
};





template<typename stm,bool write>
requires ((write&&async_scatter_output_stream<stm>)||(!write&&async_scatter_input_stream<stm>))
class async_scatter_io_coroutine
{
public:
	typename io_async_scheduler_t<stm>::type& scheduler;
	stm& stream;
	std::span<io_scatter_t> scatters;
	std::ptrdiff_t offset{write?-1:0};
	typename io_async_overlapped_t<stm>::type overlapped;
	std::size_t transferred_bytes{};
	int err{};
	constexpr bool await_ready() const { return false; }
	constexpr std::size_t await_resume() const
	{
		if(err)
			throw_posix_error(err);
		return transferred_bytes;
	}
	void await_suspend(std::coroutine_handle<> handle)
	{
		overlapped=typename io_async_overlapped_t<stm>::type(std::in_place,[handle,this](std::size_t calb,int errn)
		{
			this->transferred_bytes=calb;
			this->err=errn;
			handle.resume();
		});
		if constexpr(write)
			async_scatter_read_callback(scheduler,stream,scatters,overlapped,offset);
		else
			async_scatter_write_callback(scheduler,stream,scatters,overlapped,offset);
	}
};


template<async_output_stream stm,bool line>
class async_print_coroutine
{
public:
	typename io_async_scheduler_t<stm>::type& scheduler;
	stm& stream;
	std::ptrdiff_t offset{};
	typename io_async_overlapped_t<stm>::type overlapped;
	std::size_t transferred_bytes{};
	int err{};
	dynamic_io_buffer<typename stm::char_type> buffer;
	template<typename ...Args>
	async_print_coroutine(typename io_async_scheduler_t<stm>::type& sh,std::ptrdiff_t off,stm& s,Args&& ...args):scheduler(sh),stream(s)
	{
		if constexpr(line)
			println_freestanding(buffer,std::forward<Args>(args)...);
		else
			print_freestanding(buffer,std::forward<Args>(args)...);
	}
	constexpr bool await_ready() const { return false; }
	constexpr std::size_t await_resume() const
	{
		if(err)
			throw_posix_error(err);
		return transferred_bytes;
	}
	void await_suspend(std::coroutine_handle<> handle)
	{
		overlapped=typename io_async_overlapped_t<stm>::type(std::in_place,[handle,this](std::size_t calb,int errn)
		{
			this->transferred_bytes=calb;
			this->err=errn;
			handle.resume();
		});
		async_write_callback(scheduler,stream,buffer.beg_ptr,buffer.end_ptr,overlapped,offset);
	}
};
}

template<async_input_stream stm,::std::input_iterator Iter>
constexpr inline details::async_io_coroutine<stm,Iter,false> async_read(typename io_async_scheduler_t<stm>::type& scheduler,stm& sm,Iter begin,Iter end,std::ptrdiff_t offset=0)
{
	return {scheduler,sm,begin,end,offset};
}

template<async_output_stream stm,::std::input_iterator Iter>
constexpr inline details::async_io_coroutine<stm,Iter,true> async_write(typename io_async_scheduler_t<stm>::type& scheduler,stm& sm,Iter begin,Iter end,std::ptrdiff_t offset=-1)
{
	return {scheduler,sm,begin,end,offset};
}

template<async_scatter_input_stream stm>
constexpr inline details::async_scatter_io_coroutine<stm,false> async_scatter_read(typename io_async_scheduler_t<stm>::type& scheduler,stm& sm,std::span<io_scatter_t> sc,std::ptrdiff_t offset=0)
{
	return {scheduler,sm,sc,offset};
}

template<async_scatter_output_stream stm>
constexpr inline details::async_scatter_io_coroutine<stm,true> async_scatter_write(typename io_async_scheduler_t<stm>::type& scheduler,stm& sm,std::span<io_scatter_t> sc,std::ptrdiff_t offset=-1)
{
	return {scheduler,sm,sc,offset};
}


template<async_output_stream stm,typename... Args>
constexpr inline details::async_print_coroutine<stm,false> async_print(typename io_async_scheduler_t<stm>::type& scheduler,stm& sm,Args&& ...args)
{
	return details::async_print_coroutine<stm,false>(scheduler,-1,sm,std::forward<Args>(args)...);
}
template<async_output_stream stm,typename... Args>
constexpr inline details::async_print_coroutine<stm,true> async_println(typename io_async_scheduler_t<stm>::type& scheduler,stm& sm,Args&& ...args)
{
	return details::async_print_coroutine<stm,true>(scheduler,-1,sm,std::forward<Args>(args)...);
}

template<async_output_stream stm,typename... Args>
constexpr inline details::async_print_coroutine<stm,false> async_print(typename io_async_scheduler_t<stm>::type& scheduler,std::ptrdiff_t offset,stm& sm,Args&& ...args)
{
	return details::async_print_coroutine<stm,false>(scheduler,offset,sm,std::forward<Args>(args)...);
}
template<async_output_stream stm,typename... Args>
constexpr inline details::async_print_coroutine<stm,true> async_println(typename io_async_scheduler_t<stm>::type& scheduler,std::ptrdiff_t offset,stm& sm,Args&& ...args)
{
	return details::async_print_coroutine<stm,true>(scheduler,offset,sm,std::forward<Args>(args)...);
}



}
