#pragma once

namespace fast_io
{

enum class file_lock_mode:char unsigned
{
shared_lock,
exclusive_lock,
unlock,
};

template<std::signed_integral T>
struct basic_flock_request
{
	using int_type = T;
	file_lock_mode type=file_lock_mode::exclusive_lock;
	seekdir whence=seekdir::cur;
	int_type start=0;
	int_type len=0;
};

using flock_request_l64 = basic_flock_request<::std::int_least64_t>;

struct nop_file_lock
{
	template<typename RequestType>
	constexpr void lock(RequestType const&) noexcept{}
	template<typename RequestType>
	constexpr void unlock(RequestType const&) noexcept{}
	template<typename RequestType>
	constexpr bool try_lock(RequestType const&) noexcept
	{
		return true;
	}
};

template<typename FilelockT,typename RequestT>
requires requires(FilelockT t,RequestT& lk)
{
	t.lock(lk);
	t.unlock(lk);
}
struct file_lock_guard
{
	using file_lock_type = FilelockT;
	using request_type = RequestT;
	file_lock_type lock;
	request_type request;
	explicit constexpr file_lock_guard(file_lock_type const& lk,request_type const& flk):lock(lk),request(flk)
	{
		lock.lock(request);
	}
	file_lock_guard(file_lock_guard const&)=delete;
	file_lock_guard& operator=(file_lock_guard const&)=delete;
	constexpr ~file_lock_guard()
	{
		lock.unlock(request);
	}
};

}
