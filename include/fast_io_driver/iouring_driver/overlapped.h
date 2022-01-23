#pragma once

namespace fast_io
{

class io_uring_overlapped_base
{
public:
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	virtual void invoke(std::size_t,int errn) noexcept = 0;
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	virtual ~io_uring_overlapped_base()=default;
};

template<typename T>
class io_uring_overlapped_derived:public io_uring_overlapped_base
{
public:
	T callback;
	template<typename... Args>
	requires std::constructible_from<T,Args...>
	constexpr io_uring_overlapped_derived(std::in_place_t,Args&& ...args):callback(::fast_io::freestanding::forward<Args>(args)...){}
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	void invoke(std::size_t res,int errn) noexcept override
	{
		callback(res,errno);
	}
};

class io_uring_overlapped_observer
{
public:
	using native_handle_type = io_uring_overlapped_base*;
	native_handle_type handle{};
#if __cpp_constexpr >= 201907L
	constexpr
#endif
	void operator()(std::size_t res,int errn) noexcept
	{
		handle->invoke(res,errn);
	}
	constexpr native_handle_type const& native_handle() const noexcept
	{
		return handle;
	}
	constexpr native_handle_type& native_handle() noexcept
	{
		return handle;
	}
	constexpr native_handle_type release() noexcept
	{
		auto temp{handle};
		handle={};
		return temp;
	}
};

class io_uring_overlapped:public io_uring_overlapped_observer
{
public:
	using native_handle_type = io_uring_overlapped_base*;
	constexpr io_uring_overlapped()=default;
	constexpr io_uring_overlapped(native_handle_type hd):io_uring_overlapped_observer{hd}{}

	template<typename T,typename... Args>
	requires std::constructible_from<T,Args...>
#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	io_uring_overlapped(std::in_place_type_t<T>,Args&& ...args):
		io_uring_overlapped_observer{new io_uring_overlapped_derived<T>(std::in_place,::fast_io::freestanding::forward<Args>(args)...)}{}
	template<typename Func>
#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	io_uring_overlapped(std::in_place_t,Func&& func):io_uring_overlapped(std::in_place_type<std::remove_cvref_t<Func>>,::fast_io::freestanding::forward<Func>(func)){}

	io_uring_overlapped(io_uring_overlapped const&)=delete;
	io_uring_overlapped& operator=(io_uring_overlapped const&)=delete;
	constexpr io_uring_overlapped(io_uring_overlapped&& bmv) noexcept : io_uring_overlapped_observer{bmv.release()}{}

#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif	
	io_uring_overlapped& operator=(io_uring_overlapped&& bmv) noexcept
	{
		if(bmv.native_handle()==this->native_handle())
			return *this;
		delete this->native_handle();
		this->native_handle() = bmv.release();
		return *this;
	}
	inline
#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void reset(native_handle_type newhandle=nullptr) noexcept
	{
		delete this->native_handle();
		this->native_handle()=newhandle;
	}

#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	~io_uring_overlapped()
	{
		delete this->native_handle();
	}
};

}
