#pragma once

namespace fast_io
{

template <::std::integral ch_type>
class basic_ssl_io_observer
{
public:
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using native_handle_type = SSL *;
	native_handle_type ssl{};
	constexpr native_handle_type const &native_handle() const noexcept
	{
		return ssl;
	}
	constexpr native_handle_type &native_handle() noexcept
	{
		return ssl;
	}
	explicit constexpr operator bool() noexcept
	{
		return ssl;
	}
	constexpr native_handle_type release() noexcept
	{
		auto temp{ssl};
		ssl = {};
		return temp;
	}
};

template <::std::integral ch_type, ::std::integral ch_type1, ::std::integral ch_type2>
inline void set_bio(basic_ssl_io_observer<ch_type> siob, basic_bio_file<ch_type1> &&rbio,
					basic_bio_file<ch_type2> &&wbio) noexcept
{
	::fast_io::noexcept_call(SSL_set_bio, siob.ssl, rbio.bio, wbio.bio);
	rbio.bio = nullptr;
	wbio.bio = nullptr;
}

template <::std::integral ch_type>
inline ::std::size_t use_count(basic_ssl_io_observer<ch_type> siob)
{
	return static_cast<::std::size_t>(SSL_up_ref(siob.ssl));
}

template <::std::integral ch_type>
inline void connect(basic_ssl_io_observer<ch_type> siob) FAST_IO_HERBCEPTIONS_THROWS
{
	if (::fast_io::noexcept_call(SSL_connect, siob.ssl) != 1)
	{
		throw_openssl_error();
	}
}

template <::std::integral ch_type, typename stm>
	requires ::fast_io::operations::decay::defines::has_input_transmit_handle_define<
		decltype(::fast_io::operations::input_stream_ref(::std::declval<stm &>()))>
inline void attach(basic_ssl_io_observer<ch_type> siob, stm &sm) FAST_IO_HERBCEPTIONS_THROWS
{
	if (!SSL_set_fd(siob.native_handle(),
					input_transmit_handle_define(::fast_io::operations::input_stream_ref(sm)).fd))
	{
		throw_openssl_error();
	}
}

template <::std::integral ch_type>
inline void attach(basic_ssl_io_observer<ch_type> siob, basic_bio_io_observer<ch_type> bio1,
				   basic_bio_io_observer<ch_type> bio2)
{
	SSL_set_bio(siob.native_handle(), bio1.native_handle(), bio2.native_handle());
}

template <::std::integral ch_type>
class basic_ssl_io_handle : public basic_ssl_io_observer<ch_type>
{
public:
	using char_type = ch_type;
	using native_handle_type = SSL *;
	constexpr basic_ssl_io_handle() = default;
	constexpr basic_ssl_io_handle(native_handle_type s)
		: basic_ssl_io_observer<ch_type>(s)
	{}
	basic_ssl_io_handle(basic_ssl_io_handle const &h) FAST_IO_HERBCEPTIONS_THROWS
		: basic_ssl_io_observer<ch_type>(SSL_dup(h.native_handle()))
	{
		if (this->native_handle() == nullptr) [[unlikely]]
		{
			throw_openssl_error();
		}
	}
	basic_ssl_io_handle &operator=(basic_ssl_io_handle const &h) FAST_IO_HERBCEPTIONS_THROWS
	{
		auto temp{SSL_dup(h.native_handle())};
		if (temp == nullptr) [[unlikely]]
		{
			throw_openssl_error();
		}
		if (this->native_handle()) [[likely]]
		{
			SSL_free(this->native_handle());
		}
		this->native_handle() = temp;
		return *this;
	}
	constexpr basic_ssl_io_handle(basic_ssl_io_handle &&__restrict h) noexcept
		: basic_ssl_io_observer<ch_type>(h.native_handle())
	{
		h.native_handle() = nullptr;
	}
	inline void reset(native_handle_type newhandle = nullptr) noexcept
	{
		if (this->native_handle()) [[likely]]
		{
			SSL_free(this->native_handle());
		}
		this->native_handle() = newhandle;
	}
	void close() noexcept
	{
		if (this->ssl) [[likely]]
		{
			SSL_free(this->ssl);
			this->ssl = nullptr;
		}
	}
	basic_ssl_io_handle &operator=(basic_ssl_io_handle &&__restrict h) noexcept
	{
		if (this->native_handle()) [[likely]]
		{
			SSL_free(this->native_handle());
		}
		this->native_handle() = h.native_handle();
		h.native_handle() = nullptr;
		return *this;
	}
};

template <::std::integral ch_type>
class basic_ssl_file : public basic_ssl_io_handle<ch_type>
{
public:
	using char_type = ch_type;
	using native_handle_type = SSL *;
	constexpr basic_ssl_file() = default;
	constexpr basic_ssl_file(native_handle_type s)
		: basic_ssl_io_handle<ch_type>(s)
	{}
	basic_ssl_file(io_cookie_t, ssl_context_observer ssl_ctx_ob) FAST_IO_HERBCEPTIONS_THROWS
		: basic_ssl_io_handle<ch_type>(SSL_new(ssl_ctx_ob.native_handle()))
	{
		if (this->native_handle() == nullptr)
		{
			throw_openssl_error();
		}
	}
	template <typename... Args>
	basic_ssl_file(ssl_context_observer ssl_ctx_ob, Args &&...args) FAST_IO_HERBCEPTIONS_THROWS
		: basic_ssl_io_handle<ch_type>(SSL_new(ssl_ctx_ob.native_handle()))
	{
		if (this->native_handle() == nullptr)
		{
			throw_openssl_error();
		}
		basic_ssl_file<ch_type> self(this->native_handle());
		attach(*this, ::std::forward<Args>(args)...);
		connect(*this);
		self.release();
	}
	constexpr basic_ssl_file(basic_ssl_file const &) = default;
	constexpr basic_ssl_file &operator=(basic_ssl_file const &) = default;
	constexpr basic_ssl_file(basic_ssl_file &&__restrict) noexcept = default;
	constexpr basic_ssl_file &operator=(basic_ssl_file &&__restrict) noexcept = default;
	~basic_ssl_file()
	{
		if (this->native_handle()) [[likely]]
		{
			SSL_free(this->native_handle());
		}
	}
};

template <::std::integral ch_type>
inline ::std::byte *read_some_bytes_underflow_define(basic_ssl_io_observer<ch_type> iob, ::std::byte *first,
													 ::std::size_t count) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::size_t read_bytes{};
	auto ret(SSL_read_ex(iob.native_handle(), first, count, __builtin_addressof(read_bytes)));
	if (ret <= 0)
	{
		int error{SSL_get_error(iob.native_handle(), ret)};
		if (error == SSL_ERROR_ZERO_RETURN)
		{
			return first; /* close_notify */
		}
		throw_openssl_error();
	}
	return first + read_bytes;
}

template <::std::integral ch_type>
inline ::std::byte const *write_some_bytes_overflow_define(basic_ssl_io_observer<ch_type> iob,
														   ::std::byte const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::std::size_t written_bytes{};
	auto ret(SSL_write_ex(iob.native_handle(), first, count,
						  __builtin_addressof(written_bytes)));
	if (ret <= 0)
	{
		int error{SSL_get_error(iob.native_handle(), ret)};
		if (error == SSL_ERROR_ZERO_RETURN)
		{
			return first; /* close_notify */
		}
		throw_openssl_error();
	}
	return first + written_bytes;
}

using ssl_io_observer = basic_ssl_io_observer<char>;
using ssl_file = basic_ssl_file<char>;

template <::std::integral ch_type>
using basic_ibuf_ssl_io_observer = basic_ibuf<basic_ssl_io_observer<ch_type>>;
template <::std::integral ch_type>
using basic_ibuf_ssl_file = basic_ibuf<basic_ssl_file<ch_type>>;

using ibuf_ssl_io_observer = basic_ibuf_ssl_io_observer<char>;
using ibuf_ssl_file = basic_ibuf_ssl_file<char>;

static_assert(input_stream<ssl_io_observer>);
} // namespace fast_io
