#pragma once

#if defined(__wasi__)

#include <chrono>
#include <ranges>
#include <cstdint>
#include <utility>
#include <functional>
#include <type_traits>
#include <atomic>

#include <wasi/api.h>

namespace fast_io
{

namespace wasi
{

namespace details
{

using wasi_thread_id = ::std::uint_least64_t;

inline ::std::atomic<wasi_thread_id> next_thread_id{1u};

inline thread_local wasi_thread_id current_thread_id{};

inline
#if __cpp_constexpr >= 202207L
	constexpr
#endif
wasi_thread_id get_or_assign_thread_id() noexcept
{
	auto id{current_thread_id};
	if (id == 0)
	{
		id = next_thread_id.fetch_add(1u, ::std::memory_order_relaxed);
		current_thread_id = id;
	}
	return id;
}

inline void assign_thread_id_for_current(wasi_thread_id id) noexcept
{
	current_thread_id = id;
}

inline void sleep_for_ns(__wasi_timestamp_t ns) noexcept
{
	if (ns == 0)
	{
		return;
	}

	__wasi_timestamp_t start{};
	if (__wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 0, __builtin_addressof(start)) != __WASI_ERRNO_SUCCESS)
	{
		return;
	}

	for (;;)
	{
		__wasi_timestamp_t now{};
		if (__wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 0, __builtin_addressof(now)) != __WASI_ERRNO_SUCCESS)
		{
			return;
		}
		if (now - start >= ns)
		{
			break;
		}
		(void)__wasi_sched_yield();
	}
}

#if defined(__wasi_thread__)

struct wasi_thread_control_block
{
	wasi_thread_id id{};
	void *data{};
	void (*run)(void *) noexcept{};
	void (*destroy)(void *) noexcept{};
	::std::atomic<::std::uint_least32_t> refcount{2u};
	::std::atomic<bool> done{false};
};

inline void release_ref(wasi_thread_control_block *cb) noexcept
{
	if (cb == nullptr)
	{
		return;
	}
	if (cb->refcount.fetch_sub(1u, ::std::memory_order_acq_rel) == 1u)
	{
		cb->destroy(cb->data);
		using alloc_cb = ::fast_io::native_typed_global_allocator<wasi_thread_control_block>;
		alloc_cb::deallocate_n(cb, 1u);
	}
}

template <typename Tuple>
inline void run_impl(void *p) noexcept
{
	auto *tup{reinterpret_cast<Tuple *>(p)};
#ifdef FAST_IO_CPP_EXCEPTIONS
	try
#endif
	{
		::std::apply([](auto &func, auto &...args) { ::std::invoke(func, args...); }, *tup);
	}
#ifdef FAST_IO_CPP_EXCEPTIONS
	catch (...)
	{
		::fast_io::fast_terminate();
	}
#endif
}

template <typename Tuple>
inline void destroy_impl(void *p) noexcept
{
	using alloc = ::fast_io::native_typed_global_allocator<Tuple>;
	auto *tup{reinterpret_cast<Tuple *>(p)};
	::std::destroy_at(tup);
	alloc::deallocate_n(tup, 1u);
}

template <typename Func, typename... Args>
inline wasi_thread_control_block *make_control_block(Func &&func, Args &&...args)
{
	using tuple_type = ::fast_io::containers::tuple<::std::decay_t<Func>, ::std::decay_t<Args>...>;
	using alloc_tuple = ::fast_io::native_typed_global_allocator<tuple_type>;
	using alloc_cb = ::fast_io::native_typed_global_allocator<wasi_thread_control_block>;

	auto tup{alloc_tuple::allocate(1u)};
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif
	::new (tup) tuple_type{::std::forward<Func>(func), ::std::forward<Args>(args)...};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

	auto cb{alloc_cb::allocate(1u)};
	::new (cb) wasi_thread_control_block{next_thread_id.fetch_add(1u, ::std::memory_order_relaxed), tup,
											 &run_impl<tuple_type>, &destroy_impl<tuple_type>, 2u, false};
	return cb;
}

inline void wasi_thread_start_impl(void *start_arg) noexcept
{
	auto *cb{reinterpret_cast<wasi_thread_control_block *>(start_arg)};
	assign_thread_id_for_current(cb->id);
	cb->run(cb->data);
	cb->done.store(true, ::std::memory_order_release);
	release_ref(cb);
}

extern "C" inline void wasi_thread_start(void *start_arg) noexcept
	__attribute__((export_name("wasi_thread_start")));

extern "C" inline void wasi_thread_start(void *start_arg) noexcept
{
	::fast_io::wasi::details::wasi_thread_start_impl(start_arg);
}

#endif

} // namespace details

namespace this_thread
{

using id = ::fast_io::wasi::details::wasi_thread_id;

inline
#if __cpp_constexpr >= 202207L
	constexpr
#endif
id get_id() noexcept
{
	return ::fast_io::wasi::details::get_or_assign_thread_id();
}

template <typename Rep, typename Period>
inline
#if __cpp_constexpr >= 202207L
	constexpr
#endif
void sleep_for(::std::chrono::duration<Rep, Period> const &sleep_duration) noexcept
{
	auto const ns64{::std::chrono::duration_cast<::std::chrono::nanoseconds>(sleep_duration).count()};
	if (ns64 <= 0)
	{
		return;
	}
	::fast_io::wasi::details::sleep_for_ns(static_cast<__wasi_timestamp_t>(ns64));
}

template <typename Clock, typename Duration>
inline
#if __cpp_constexpr >= 202207L
	constexpr
#endif
void sleep_until(::std::chrono::time_point<Clock, Duration> const &expect_time) noexcept
{
	auto const now{Clock::now()};
	if (now < expect_time)
	{
		::fast_io::wasi::this_thread::sleep_for(expect_time - now);
	}
}

template <::std::int_least64_t off_to_epoch>
inline
#if __cpp_constexpr >= 202207L
	constexpr
#endif
void sleep_for(::fast_io::basic_timestamp<off_to_epoch> const &sleep_duration) noexcept
{
	if (sleep_duration.seconds < 0)
	{
		return;
	}

	constexpr ::std::uint_least64_t mul_factor{::fast_io::uint_least64_subseconds_per_second / 1000000000u};

	auto const ns64{
		static_cast<::std::uint_least64_t>(static_cast<::std::uint_least64_t>(sleep_duration.seconds) * 1000000000u +
										   sleep_duration.subseconds / mul_factor)};

	if (ns64 == 0)
	{
		return;
	}

	::fast_io::wasi::details::sleep_for_ns(static_cast<__wasi_timestamp_t>(ns64));
}

template <::std::int_least64_t off_to_epoch>
inline
#if __cpp_constexpr >= 202207L
	constexpr
#endif
void sleep_until(::fast_io::basic_timestamp<off_to_epoch> const &expect_time) noexcept
{
	if (expect_time.seconds < 0)
	{
		return;
	}

	auto const unix_expect{static_cast<::fast_io::unix_timestamp>(expect_time)};
	auto const now{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	if (!(now < unix_expect))
	{
		return;
	}

	auto const delta{unix_expect - now};
	::fast_io::wasi::this_thread::sleep_for(delta);
}

} // namespace this_thread

#if defined(__wasi_thread__)

class wasi_thread
{
public:
	using id = ::fast_io::wasi::details::wasi_thread_id;
	using native_handle_type = ::fast_io::wasi::details::wasi_thread_control_block *;

private:
	native_handle_type cb_{};
	id id_{};
	bool joinable_{false};

public:
	inline constexpr wasi_thread() noexcept = default;

	template <typename Func, typename... Args>
		requires(::std::invocable<Func, Args...>)
	inline wasi_thread(Func &&func, Args &&...args)
	{
		auto *cb{::fast_io::wasi::details::make_control_block(::std::forward<Func>(func),
											 ::std::forward<Args>(args)...)};
		auto ec{__wasi_thread_spawn(cb)};
		if (ec != 0)
		{
			::fast_io::wasi::details::release_ref(cb);
			::fast_io::wasi::details::release_ref(cb);
			::fast_io::throw_posix_error(ec);
		}
		cb_ = cb;
		id_ = cb->id;
		joinable_ = true;
	}

	inline constexpr wasi_thread(wasi_thread const &) noexcept = delete;

	inline constexpr wasi_thread(wasi_thread &&other) noexcept : cb_{other.cb_}, id_{other.id_}, joinable_{other.joinable_}
	{
		other.cb_ = nullptr;
		other.id_ = {};
		other.joinable_ = false;
	}

	inline ~wasi_thread() noexcept
	{
		if (this->joinable()) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
	}

	inline constexpr wasi_thread &operator=(wasi_thread const &) noexcept = delete;

	inline constexpr wasi_thread &operator=(wasi_thread &&other) noexcept
	{
		if (this == __builtin_addressof(other)) [[unlikely]]
		{
			return *this;
		}
		this->swap(other);
		return *this;
	}

	inline constexpr bool joinable() const noexcept
	{
		return this->joinable_;
	}

	inline void join()
	{
		if (!this->joinable()) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		for (;;)
		{
			if (cb_->done.load(::std::memory_order_acquire))
			{
				break;
			}
			(void)__wasi_sched_yield();
		}
		this->joinable_ = false;
		::fast_io::wasi::details::release_ref(cb_);
		cb_ = nullptr;
	}

	inline void detach()
	{
		if (!this->joinable()) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		this->joinable_ = false;
		::fast_io::wasi::details::release_ref(cb_);
		cb_ = nullptr;
	}

	inline constexpr void swap(wasi_thread &other) noexcept
	{
		::std::ranges::swap(cb_, other.cb_);
		::std::ranges::swap(id_, other.id_);
		::std::ranges::swap(joinable_, other.joinable_);
	}

	[[nodiscard]]
	inline constexpr auto get_id() const noexcept
	{
		return this->id_;
	}

	[[nodiscard]]
	inline constexpr auto native_handle() const noexcept
	{
		return this->cb_;
	}

	[[nodiscard]]
	inline static constexpr ::std::uint_least32_t hardware_concurrency() noexcept
	{
		return 0u;
	}
};

#endif

} // namespace wasi

#if defined(__wasi_thread__)
using wasi_thread = wasi::wasi_thread;
using native_thread = wasi_thread;
#endif

namespace this_thread
{
using ::fast_io::wasi::this_thread::get_id;
using ::fast_io::wasi::this_thread::sleep_for;
using ::fast_io::wasi::this_thread::sleep_until;
} // namespace this_thread

} // namespace fast_io

#endif
