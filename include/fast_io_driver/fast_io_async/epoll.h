#pragma once

#include<sys/epoll.h>

namespace fast_io
{

namespace epoll
{

struct close_on_exec_function_invoked_t
{
explicit constexpr close_on_exec_function_invoked_t()=default;
};

inline constexpr close_on_exec_function_invoked_t close_on_exec_function_invoked{};


class handle_pool
{
	int fd;
	void close_impl() noexcept
	{
		if(fd!=-1)
			close(fd);
	}
public:
	using native_handle_type = int;
	explicit handle_pool(std::integral auto counts):fd(::epoll_create(static_cast<int>(counts)))
	{
		if(fd==-1)
			throw posix_error();
	}
	explicit handle_pool(std::integral auto counts,close_on_exec_function_invoked_t):
		fd(::epoll_create1(static_cast<int>(counts)|EPOLL_CLOEXEC))
	{
		if(fd==-1)
			throw posix_error();
	}
	handle_pool(handle_pool const& dp):fd(dup(dp.fd))
	{
		if(fd<0)
			throw posix_error();
	}
	handle_pool& operator=(handle_pool const& dp)
	{
		auto newfd(dup2(dp.fd,fd));
		if(newfd<0)
			throw posix_error();
		fd=newfd;
		return *this;
	}
	handle_pool(handle_pool&& b) noexcept : handle_pool(b.fd)
	{
		b.fd = -1;
	}
	handle_pool& operator=(handle_pool&& b) noexcept
	{
		if(__builtin_addressof(b)!=this)
		{
			close_impl();
			fd=b.fd;
			b.fd = -1;
		}
		return *this;
	}
	auto native_handle() const
	{
		return fd;
	}
	~handle_pool()
	{
		if(fd!=-1)
			close(fd);
	}
};

//TODO: give them more meaningful names
enum class event:std::uint32_t
{
none=0,in=1,pri=2,out=4,err=8,hup=0x10,nval=0x20,rdnorm=0x40,
rdband=0x80,wrnorm=0x100,wrband=0x200,message=0x400,
rdhup=0x2000,exclusive=1u<<28,oneshot=1u<<30,edge_trigger=1u<<31
};

constexpr event operator&(event x, event y) noexcept
{
using utype = typename std::underlying_type<event>::type;
return static_cast<event>(static_cast<utype>(x) & static_cast<utype>(y));
}

constexpr event operator|(event x, event y) noexcept
{
using utype = typename std::underlying_type<event>::type;
return static_cast<event>(static_cast<utype>(x) | static_cast<utype>(y));
}

constexpr event operator^(event x, event y) noexcept
{
using utype = typename std::underlying_type<event>::type;
return static_cast<event>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

constexpr event operator~(event x) noexcept
{
using utype = typename std::underlying_type<event>::type;
return static_cast<event>(~static_cast<utype>(x));
}

inline event& operator&=(event& x, event y) noexcept{return x=x&y;}

inline event& operator|=(event& x, event y) noexcept{return x=x|y;}

inline event& operator^=(event& x, event y) noexcept{return x=x^y;}

struct events:epoll_event{};

inline constexpr event get(events const& e)
{
	return static_cast<event>((static_cast<epoll_event const&>(e)).events);
}

inline constexpr auto fd(events const& e)
{
	return (static_cast<epoll_event const&>(e)).data.fd;
}

template<typename T>
concept epoll_stream = requires(T& out)
{
	{ultimate_native_handle(out)};
};

template<epoll_stream sm>
inline constexpr bool operator==(sm& s,events const& d)
{
	return ultimate_native_handle(s)==fd(d);
}

template<epoll_stream sm>
inline constexpr bool operator!=(sm& s,events const& d)
{
	return ultimate_native_handle(s)!=fd(d);
}

template<epoll_stream sm>
inline constexpr bool operator==(events const& d,sm& s)
{
	return ultimate_native_handle(s)==fd(d);
}

template<epoll_stream sm>
inline constexpr bool operator!=(events const& d,sm& s)
{
	return ultimate_native_handle(s)!=fd(d);
}

template<epoll_stream output>
inline output& add_control(handle_pool& pool,output& out,event e)
{
	auto const out_ultimate(ultimate_native_handle(out));
	epoll_event evt{static_cast<std::uint32_t>(e),{.fd=out_ultimate}};
	if(::epoll_ctl(pool.native_handle(),1,out_ultimate,__builtin_addressof(evt))==-1)
		throw posix_error();
	return out;
}

template<epoll_stream output>
inline output& delete_control(handle_pool& pool,output& out)
{
	auto const out_ultimate(ultimate_native_handle(out));
	epoll_event evt{1,{.fd=out_ultimate}};
	if(::epoll_ctl(pool.native_handle(),2,out_ultimate,__builtin_addressof(evt))==-1)
		throw posix_error();
	return out;
}
template<epoll_stream output>
inline output& modify_control(handle_pool& pool,output& out,event e)
{
	auto const out_ultimate(ultimate_native_handle(out));
	epoll_event evt{static_cast<std::uint32_t>(e),{.fd=out_ultimate}};
	if(::epoll_ctl(pool.native_handle(),3,out_ultimate,__builtin_addressof(evt))==-1)
		throw posix_error();
	return out;
}

template<typename Rep,typename Period>
inline std::span<events> wait(handle_pool& pool,std::span<events> evs,std::chrono::duration<Rep,Period> const& timeout)
{
	int ret(::epoll_wait(pool.native_handle(),evs.data(),static_cast<int>(evs.size()),
		static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count())==1));
	if(ret==-1)
		throw posix_error();
	return evs.first(static_cast<std::size_t>(ret));
}

inline std::span<events> wait(handle_pool& pool,std::span<events> evs)
{
	int ret(::epoll_wait(pool.native_handle(),evs.data(),static_cast<int>(evs.size()),-1));
	if(ret==-1)
		throw posix_error();
	return evs.first(static_cast<std::size_t>(ret));
}


/*
template<output_stream output>
inline void delete_control(handle_pool& pool,output& out,std::integral auto value)
{
	if(::epoll_ctl(pool.native_handle(),__builtin_addressof(func),2,nullptr,epoll_event{0,static_cast<std::uint64_t>(value)})==-1)
		throw posix_error();
}

template<output_stream output>
inline void modify_control(handle_pool& pool,output& out,event e,std::integral auto value)
{
	if(::epoll_ctl(pool.native_handle(),__builtin_addressof(func),3,epoll_event{e,static_cast<std::uint64_t>(value)})==-1)
		throw posix_error();
}
*/
}

}
