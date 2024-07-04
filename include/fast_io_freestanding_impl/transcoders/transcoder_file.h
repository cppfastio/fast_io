#pragma once

namespace fast_io
{

struct transcoder_file_base
{
	virtual constexpr bool transcode_always_none() const = 0;
	virtual constexpr ::std::size_t transcode_bytes_size(::std::byte const *, ::std::byte const *, ::std::size_t) const = 0;
	virtual constexpr transcode_bytes_result transcode_bytes(::std::byte const *, ::std::byte const *, ::std::byte *, ::std::byte *) = 0;
	virtual constexpr ::std::size_t transcode_bytes_eof_size(::std::byte const *, ::std::byte const *, ::std::size_t) const = 0;
	virtual constexpr transcode_bytes_result transcode_bytes_eof(::std::byte const *, ::std::byte const *, ::std::byte *, ::std::byte *) = 0;
	virtual void destroy() noexcept = 0;
	constexpr ~transcoder_file_base() = default;
};

template <typename allocator, typename T>
	requires(::fast_io::transcoder<T> || ::fast_io::byte_transcoder<T>)
struct transcoder_file_derived : transcoder_file_base
{
	T object;
	explicit constexpr basic_transcoder_file(::fast_io::io_cookie_type_t<T>, Args &&args...)
		: object(::std::forward<Args>(args)...)
	{
	}
	constexpr bool transcode_always_none() const override
	{
		if constexpr (requires() {
						  object.transcode_always_none();
					  })
		{
			return object.transcode_always_none();
		}
		else
		{
			return false;
		}
	}
	constexpr ::std::size_t transcode_bytes_size(::std::byte const *fromfirst, ::std::byte const *fromlast, ::std::size_t mxsz) const override
	{
		return object.transcode_bytes_size(fromfirst, fromlast, mxsz);
	}
	constexpr transcode_bytes_result transcode_bytes(::std::byte const *fromfirst, ::std::byte const *fromlast, ::std::byte *tofirst, ::std::byte *tolast) override
	{
		return object.transcode_bytes(fromfirst, fromlast, tofirst, tolast);
	}
	constexpr ::std::size_t transcode_bytes_eof_size(::std::byte const *fromfirst, ::std::byte const *fromlast, ::std::size_t mxsz) override
	{
		if constexpr (requires() {
						  { object.transcode_bytes_eof_size(fromfirst, fromlast, mxsz) } -> std::same_as<::std::size_t>;
					  })
		{
			return object.transcode_bytes_eof_size(fromfirst, fromlast, mxsz);
		}
		else
		{
			return object.transcode_bytes_size(fromfirst, fromlast, mxsz);
		}
	}
	constexpr transcode_bytes_result transcode_bytes_eof(::std::byte const *fromfirst, ::std::byte const *fromlast, ::std::byte *tofirst, ::std::byte *tolast) override
	{
		if constexpr (requires() {
						  { object.transcode_bytes_eof(fromfirst, fromlast, tofirst, tolast) } -> ::std::same_as<transcode_bytes_result>;
					  })
		{
			return object.transcode_bytes_eof(fromfirst, fromlast, tofirst, tolast);
		}
		else
		{
			return object.transcode_bytes(fromfirst, fromlast, tofirst, tolast);
		}
	}
	constexpr void destroy() noexcept override
	{
		::std::destroy(this);
		::fast_io::typed_allocator_adapter<allocator, ::fast_io::transcoder_file_derived<allocator, T>>::deallocate(this, 1);
	}
};

class transcoder_io_observer
{
public:
	using native_handle_type = transcoder_file_base *;
	native_handle_type transhandle{};
	constexpr native_handle_type release() noexcept
	{
		auto temptranshandle{transhandle};
		this->transhandle = nullptr;
		return temptranshandle;
	}
	constexpr native_handle_type native_handle() const noexcept
	{
		return this->transhandle;
	}
	constexpr bool transcode_always_none() const
	{
		return transhandle->transcode_always_none();
	}
	constexpr ::std::size_t transcode_bytes_size(::std::byte const *fromfirst, ::std::byte const *fromlast, ::std::size_t mxsz) const
	{
		return transhandle->transcode_bytes_size(fromfirst, fromlast, mxsz);
	}
	constexpr transcode_bytes_result transcode_bytes(::std::byte const *fromfirst, ::std::byte const *fromlast, ::std::byte *tofirst, ::std::byte *tolast) override
	{
		return transhandle->transcode_bytes(fromfirst, fromlast, tofirst, tolast);
	}
	constexpr ::std::size_t transcode_bytes_eof_size(::std::byte const *fromfirst, ::std::byte const *fromlast, ::std::size_t mxsz) const
	{
		return transhandle->transcode_bytes_eof_size(fromfirst, fromlast, mxsz);
	}
	constexpr transcode_bytes_result transcode_bytes_eof(::std::byte const *fromfirst, ::std::byte const *fromlast, ::std::byte *tofirst, ::std::byte *tolast) override
	{
		return transhandle->transcode_bytes_eof(fromfirst, fromlast, tofirst, tolast);
	}
};

template <typename allocator>
class basic_transcoder_file : public transcoder_io_observer
{
public:
	using allocator_type = allocator;
	using transcoder_io_observer::native_handle_type;
	using transcoder_io_observer::transhandle;
	template <typename T, typename... Args>
		requires ::std::constructible_from<T, Args...>
	explicit constexpr basic_transcoder_file(::fast_io::io_cookie_type_t<T>, Args &&args...)
		: transcoder_io_observer{::std::construct_at(typed_allocator_adapter<allocator, ::fast_io::transcoder_file_derived<allocator, T>>::allocate(1), ::fast_io::io_cookie_type<T>, ::std::forward<Args>(args)...)}
	{
	}
	constexpr basic_transcoder_file(basic_transcoder_file const &) = delete;
	constexpr basic_transcoder_file &operator=(basic_transcoder_file const &) = delete;

	constexpr basic_transcoder_file(basic_transcoder_file &&other) noexcept
		: transhandle{other.handle}
	{
		other.transhandle = nullptr;
	}
	constexpr basic_transcoder_file &operator=(basic_transcoder_file &&other) noexcept
	{
		if (__builtin_addressof(other) == this)
		{
			return *this;
		}
		this->transhandle->destroy();
		this->transhandle = other.transhandle;
		other.transhandle = nullptr;
		return *this;
	}
	constexpr void close() noexcept
	{
		this->transhandle->destroy();
		this->transhandle = nullptr;
	}
	constexpr ~basic_transcoder_file()
	{
		this->transhandle->destroy();
	}
};

} // namespace fast_io
