#pragma once

/*
Schannel (SSPI) TLS client -- the OS TLS stack on Windows. Compiled on
_WIN32 only. SSPI decls are mirrored here the same way apis.h mirrors
win32: the security package lives in secur32.dll.
*/

#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)

namespace fast_io::tls::details
{

/* ---- sspi surface (secur32.dll), ABI-stable mirrors ---- */

struct schannel_handle /* SecHandle */
{
	::std::uintptr_t lower;
	::std::uintptr_t upper;
};

using schannel_timestamp = ::std::uint_least64_t; /* SECURITY_INTEGER/LARGE_INTEGER */

struct schannel_buffer /* SecBuffer */
{
	::std::uint_least32_t size;
	::std::uint_least32_t type;
	void *data;
};

struct schannel_buffer_desc /* SecBufferDesc */
{
	::std::uint_least32_t version;
	::std::uint_least32_t count;
	schannel_buffer *buffers;
};

struct schannel_cred /* SCHANNEL_CRED v5 */
{
	::std::uint_least32_t version;
	::std::uint_least32_t creds_count;
	void const **creds; /* PCCERT_CONTEXT const* */
	void *root_store;   /* HCERTSTORE */
	::std::uint_least32_t mappers_count;
	void *mappers;
	::std::uint_least32_t supported_algs_count;
	::std::uint_least32_t *supported_algs;
	::std::uint_least32_t enabled_protocols;
	::std::uint_least32_t minimum_cipher_strength;
	::std::uint_least32_t maximum_cipher_strength;
	::std::uint_least32_t session_lifespan;
	::std::uint_least32_t flags;
	::std::uint_least32_t cred_format;
};

struct schannel_stream_sizes /* SecPkgContext_StreamSizes */
{
	::std::uint_least32_t header;
	::std::uint_least32_t trailer;
	::std::uint_least32_t maximum_message;
	::std::uint_least32_t buffers_count;
	::std::uint_least32_t block_size;
};

inline constexpr ::std::uint_least32_t schannel_cred_version{4};             /* SCHANNEL_CRED_VERSION */
inline constexpr ::std::uint_least32_t sch_cred_auto_cred_validation{0x20u}; /* system trust store check */
inline constexpr ::std::uint_least32_t sch_cred_manual_cred_validation{0x08u};
inline constexpr ::std::uint_least32_t sch_cred_use_default_creds{0x40u};
inline constexpr ::std::uint_least32_t sch_send_root_cert{0x40000u};

inline constexpr ::std::uint_least32_t secpkg_cred_outbound{2};
inline constexpr ::std::uint_least32_t security_native_drep{0x10u};

inline constexpr ::std::uint_least32_t isc_req_delegate{0x1u};
inline constexpr ::std::uint_least32_t isc_req_mutual_auth{0x2u};
inline constexpr ::std::uint_least32_t isc_req_replay_detect{0x4u};
inline constexpr ::std::uint_least32_t isc_req_sequence_detect{0x8u};
inline constexpr ::std::uint_least32_t isc_req_confidentiality{0x10u};
inline constexpr ::std::uint_least32_t isc_req_allocate_memory{0x100u};
inline constexpr ::std::uint_least32_t isc_req_connection{0x800u};
inline constexpr ::std::uint_least32_t isc_req_stream{0x8000u};
inline constexpr ::std::uint_least32_t isc_req_extended_error{0x4000u};
inline constexpr ::std::uint_least32_t isc_req_use_supplied_creds{0x80u};
inline constexpr ::std::uint_least32_t isc_req_manual_cred_validation{0x80000u};

inline constexpr ::std::int_least32_t sec_e_ok{0};
inline constexpr ::std::int_least32_t sec_i_continue_needed{0x00090312};
inline constexpr ::std::int_least32_t sec_i_complete_needed{0x00090313};
inline constexpr ::std::int_least32_t sec_i_complete_and_continue{0x00090314};
inline constexpr ::std::int_least32_t sec_i_context_expired{0x00090317};
inline constexpr ::std::int_least32_t sec_e_incomplete_message{static_cast<::std::int_least32_t>(0x80090318u)};
inline constexpr ::std::int_least32_t sec_i_renegotiate{0x00090321};
inline constexpr ::std::int_least32_t sec_i_message_fragment{0x00090364};

inline constexpr ::std::uint_least32_t secbuffer_empty{0};
inline constexpr ::std::uint_least32_t secbuffer_data{1};
inline constexpr ::std::uint_least32_t secbuffer_token{2};
inline constexpr ::std::uint_least32_t secbuffer_missing{4};
inline constexpr ::std::uint_least32_t secbuffer_extra{5};
inline constexpr ::std::uint_least32_t secbuffer_stream_trailer{6};
inline constexpr ::std::uint_least32_t secbuffer_stream_header{7};
inline constexpr ::std::uint_least32_t secbuffer_padding{9};
inline constexpr ::std::uint_least32_t secbuffer_stream{10};
inline constexpr ::std::uint_least32_t secbuffer_alert{17};

inline constexpr ::std::uint_least32_t secpkg_attr_stream_sizes{4};
inline constexpr ::std::uint_least32_t secpkg_attr_remote_cert_context{0x53u};

inline constexpr ::std::uint_least32_t sch_protocols_tls13{0x2000u}; /* SP_PROT_TLS1_3 */

inline constexpr char16_t unisp_name_w[]{u"Microsoft Unified Security Protocol Provider"};
inline constexpr char16_t schannel_name_w[]{u"Schannel"};

FAST_IO_DLLIMPORT ::std::int_least32_t FAST_IO_WINSTDCALL AcquireCredentialsHandleW(
	char16_t const *, char16_t const *, ::std::uint_least32_t, void *,
	void *, void *, void *, schannel_handle *,
	schannel_timestamp *) noexcept FAST_IO_WINSTDCALL_RENAME(AcquireCredentialsHandleW, 36);
FAST_IO_DLLIMPORT ::std::int_least32_t FAST_IO_WINSTDCALL FreeCredentialsHandle(
	schannel_handle *) noexcept FAST_IO_WINSTDCALL_RENAME(FreeCredentialsHandle, 4);
FAST_IO_DLLIMPORT ::std::int_least32_t FAST_IO_WINSTDCALL InitializeSecurityContextW(
	schannel_handle *, schannel_handle *, char16_t *, ::std::uint_least32_t,
	::std::uint_least32_t, ::std::uint_least32_t, schannel_buffer_desc *,
	::std::uint_least32_t, schannel_handle *, schannel_buffer_desc *,
	::std::uint_least32_t *,
	schannel_timestamp *) noexcept FAST_IO_WINSTDCALL_RENAME(InitializeSecurityContextW, 48);
FAST_IO_DLLIMPORT ::std::int_least32_t FAST_IO_WINSTDCALL CompleteAuthToken(
	schannel_handle *,
	schannel_buffer_desc *) noexcept FAST_IO_WINSTDCALL_RENAME(CompleteAuthToken, 8);
FAST_IO_DLLIMPORT ::std::int_least32_t FAST_IO_WINSTDCALL DeleteSecurityContext(
	schannel_handle *) noexcept FAST_IO_WINSTDCALL_RENAME(DeleteSecurityContext, 4);
FAST_IO_DLLIMPORT ::std::int_least32_t FAST_IO_WINSTDCALL FreeContextBuffer(
	void *) noexcept FAST_IO_WINSTDCALL_RENAME(FreeContextBuffer, 4);
FAST_IO_DLLIMPORT ::std::int_least32_t FAST_IO_WINSTDCALL QueryContextAttributesW(
	schannel_handle *, ::std::uint_least32_t,
	void *) noexcept FAST_IO_WINSTDCALL_RENAME(QueryContextAttributesW, 12);
FAST_IO_DLLIMPORT ::std::int_least32_t FAST_IO_WINSTDCALL EncryptMessage(
	schannel_handle *, ::std::uint_least32_t, schannel_buffer_desc *,
	::std::uint_least32_t) noexcept FAST_IO_WINSTDCALL_RENAME(EncryptMessage, 16);
FAST_IO_DLLIMPORT ::std::int_least32_t FAST_IO_WINSTDCALL DecryptMessage(
	schannel_handle *, schannel_buffer_desc *, ::std::uint_least32_t,
	::std::uint_least32_t *) noexcept FAST_IO_WINSTDCALL_RENAME(DecryptMessage, 16);
FAST_IO_DLLIMPORT ::std::int_least32_t FAST_IO_WINSTDCALL ApplyControlToken(
	schannel_handle *,
	schannel_buffer_desc *) noexcept FAST_IO_WINSTDCALL_RENAME(ApplyControlToken, 8);

/* RAII for a SecBuffer token allocated by the SSPI package
   (ISC_REQ_ALLOCATE_MEMORY) -- FreeContextBuffer on scope exit */
struct schannel_out_token
{
	void *data{};
	::std::uint_least32_t size{};

	inline constexpr schannel_out_token() noexcept = default;
	schannel_out_token(schannel_out_token const &) = delete;
	schannel_out_token &operator=(schannel_out_token const &) = delete;
	schannel_out_token(schannel_out_token &&) = delete;
	schannel_out_token &operator=(schannel_out_token &&) = delete;
	inline ~schannel_out_token()
	{
		if (data != nullptr)
		{
			FreeContextBuffer(data);
		}
	}
	/* harvest an ISC output buffer */
	inline void take(schannel_buffer &b) noexcept
	{
		data = b.data;
		size = b.size;
		b.data = nullptr;
	}
};

[[noreturn]] inline void schannel_throw(::std::int_least32_t err) FAST_IO_HERBCEPTIONS_THROWS
{
	/* SECURITY_STATUS is not errno; fold into win32 error domain via
	   the low bits -- sspi errors are HRESULT-facility 9 */
	::fast_io::throw_win32_error(static_cast<::std::uint_least32_t>(err));
}

inline void schannel_check(::std::int_least32_t status) FAST_IO_HERBCEPTIONS_THROWS
{
	if (status < 0) /* FAILED() */
	{
		schannel_throw(status);
	}
}

} // namespace fast_io::tls::details

namespace fast_io::tls
{

template <typename socket_observer_type = ::fast_io::native_socket_io_observer>
struct basic_schannel_tls_client;

namespace details
{
template <typename socket_observer_type>
inline void schannel_tls_free(basic_schannel_tls_client<socket_observer_type> *client) noexcept;
}

/*
the schannel client aggregate: transport observer + SSPI handles +
the leftover ciphertext that completed a record boundary mid-buffer.
*/
template <typename socket_observer_type>
struct basic_schannel_tls_client
{
	using socket_observer = socket_observer_type;

	socket_observer_type sock_{};
	details::schannel_handle cred_{};
	details::schannel_handle ctx_{};
	/* pt_pending: decrypted plaintext the caller's buffer could not
	   take. ct_pending: undecrypted ciphertext DecryptMessage returned
	   as EXTRA -- fed back in as the next decrypt input. */
	::std::byte pt_pending_[details::tls_max_record]{};
	::std::size_t pt_pending_size_{};
	::std::byte ct_pending_[details::tls_max_record]{};
	::std::size_t ct_pending_size_{};
	details::schannel_stream_sizes stream_sizes_{};
	bool have_ctx_{};
	bool established_{};


	inline constexpr basic_schannel_tls_client() noexcept = default;
	inline explicit constexpr basic_schannel_tls_client(socket_observer_type sock) noexcept
		: sock_{sock}
	{
	}
	basic_schannel_tls_client(basic_schannel_tls_client const &) = delete;
	basic_schannel_tls_client &operator=(basic_schannel_tls_client const &) = delete;
	inline constexpr basic_schannel_tls_client(basic_schannel_tls_client &&other) noexcept
		: sock_{other.sock_}, cred_{other.cred_}, ctx_{other.ctx_},
		  pt_pending_size_{other.pt_pending_size_}, ct_pending_size_{other.ct_pending_size_},
		  stream_sizes_{other.stream_sizes_},
		  have_ctx_{other.have_ctx_}, established_{other.established_}
	{
		::fast_io::freestanding::non_overlapped_copy_n(other.pt_pending_, other.pt_pending_size_, pt_pending_);
		::fast_io::freestanding::non_overlapped_copy_n(other.ct_pending_, other.ct_pending_size_, ct_pending_);
		other.have_ctx_ = false;
		other.established_ = false;
		other.pt_pending_size_ = 0;
		other.ct_pending_size_ = 0;
	}
	inline basic_schannel_tls_client &operator=(basic_schannel_tls_client &&other) noexcept
	{
		if (__builtin_addressof(other) == this)
		{
			return *this;
		}
		details::schannel_tls_free(this);
		sock_ = other.sock_;
		cred_ = other.cred_;
		ctx_ = other.ctx_;
		pt_pending_size_ = other.pt_pending_size_;
		ct_pending_size_ = other.ct_pending_size_;
		::fast_io::freestanding::non_overlapped_copy_n(other.pt_pending_, other.pt_pending_size_, pt_pending_);
		::fast_io::freestanding::non_overlapped_copy_n(other.ct_pending_, other.ct_pending_size_, ct_pending_);
		stream_sizes_ = other.stream_sizes_;
		have_ctx_ = other.have_ctx_;
		established_ = other.established_;
		other.have_ctx_ = false;
		other.established_ = false;
		other.pt_pending_size_ = 0;
		other.ct_pending_size_ = 0;
		return *this;
	}
	inline ~basic_schannel_tls_client()
	{
		details::schannel_tls_free(this);
	}
};

namespace details
{

template <typename socket_observer_type>
inline void schannel_tls_free(basic_schannel_tls_client<socket_observer_type> *client) noexcept
{
	if (client->have_ctx_)
	{
		DeleteSecurityContext(__builtin_addressof(client->ctx_));
		client->have_ctx_ = false;
	}
	if (client->cred_.lower != 0 || client->cred_.upper != 0)
	{
		FreeCredentialsHandle(__builtin_addressof(client->cred_));
		client->cred_ = {};
	}
}

/* isc flags we always request */
inline constexpr ::std::uint_least32_t schannel_isc_reqs{
	isc_req_sequence_detect | isc_req_replay_detect | isc_req_confidentiality |
	isc_req_allocate_memory | isc_req_extended_error | isc_req_stream};

template <typename socket_observer_type>
inline void tls_client_handshake(basic_schannel_tls_client<socket_observer_type> *client,
								 tls_client_config const *cfg) FAST_IO_HERBCEPTIONS_THROWS
{
	if (client->established_)
	{
		return;
	}

	schannel_cred cred_params{};
	cred_params.version = schannel_cred_version;
	cred_params.flags = sch_send_root_cert;
	if (cfg->check_chain)
	{
		cred_params.flags |= sch_cred_auto_cred_validation;
	}
	else
	{
		cred_params.flags |= sch_cred_manual_cred_validation |
							 sch_cred_use_default_creds;
	}
	/* TLS 1.3 only, matching the native client's no-downgrade stance */
	cred_params.enabled_protocols = 0; /* OS picks: schannel on win11 does tls1.3 by default */

	schannel_check(AcquireCredentialsHandleW(nullptr, const_cast<char16_t *>(unisp_name_w),
											 secpkg_cred_outbound, nullptr,
											 __builtin_addressof(cred_params), nullptr, nullptr,
											 __builtin_addressof(client->cred_), nullptr));

	/* schannel wants a utf-16 target name */
	char16_t target[256];
	::std::size_t tn{0};
	for (::std::size_t i{}; i != cfg->hostname.size() && tn != 255; ++i)
	{
		char8_t const c{cfg->hostname[i]};
		if ((static_cast<::std::uint_least8_t>(c) & 0x80u) != 0)
		{
			break; /* non-ascii: skip target name entirely */
		}
		target[tn++] = static_cast<char16_t>(c);
	}
	target[tn] = 0;

	char16_t *const target_name{cfg->check_hostname ? target : nullptr};

	::std::byte inbuf[details::tls_max_record];
	::std::size_t in_size{};

	schannel_timestamp expiry{};
	for (;;)
	{
		schannel_buffer outb{0, secbuffer_token, nullptr};
		schannel_buffer_desc outd{0, 1, __builtin_addressof(outb)};
		schannel_buffer inb[2]{{static_cast<::std::uint_least32_t>(in_size), secbuffer_token, inbuf},
							   {0, secbuffer_empty, nullptr}};
		schannel_buffer_desc ind{0, 2, inb};
		::std::uint_least32_t attrs{};
		auto const status{InitializeSecurityContextW(
			__builtin_addressof(client->cred_),
			client->have_ctx_ ? __builtin_addressof(client->ctx_) : nullptr,
			target_name, schannel_isc_reqs, 0, security_native_drep,
			client->have_ctx_ ? __builtin_addressof(ind) : nullptr, 0,
			__builtin_addressof(client->ctx_), __builtin_addressof(outd),
			__builtin_addressof(attrs), __builtin_addressof(expiry))};
		client->have_ctx_ = true;

		/* any EXTRA input already belongs to the next record batch */
		::std::size_t leftover{};
		for (::std::uint_least32_t i{}; i != ind.count; ++i)
		{
			if (ind.buffers[i].type == secbuffer_extra)
			{
				leftover = ind.buffers[i].size;
				__builtin_memmove(inbuf,
								  static_cast<::std::byte *>(inbuf) + (in_size - leftover),
								  leftover);
			}
		}
		in_size = leftover;

		if (status == sec_i_continue_needed || status == sec_i_complete_and_continue ||
			status == sec_e_ok)
		{
			schannel_out_token out_token{};
			out_token.take(outb);
			if (out_token.size != 0)
			{
				tls_write_full(client->sock_,
							   reinterpret_cast<::std::byte const *>(out_token.data), out_token.size);
			}
			if (status == sec_i_complete_and_continue)
			{
				schannel_check(CompleteAuthToken(__builtin_addressof(client->ctx_),
												 __builtin_addressof(outd)));
			}
			if (status == sec_e_ok)
			{
				break;
			}
		}
		else if (status == sec_e_incomplete_message)
		{
			/* need more wire bytes -- fall through to read */
		}
		else
		{
			schannel_throw(status);
		}

		auto const e{details::tls_read_some(
			client->sock_, inbuf + in_size, sizeof(inbuf) - in_size)};
		if (e == inbuf + in_size)
		{
			::fast_io::throw_posix_error(EPIPE);
		}
		in_size += static_cast<::std::size_t>(e - inbuf);
	}

	/* ciphertext that arrived with the final handshake flight belongs
	   to the record layer -- stash it for the first decrypt */
	__builtin_memcpy(client->ct_pending_, inbuf, in_size);
	client->ct_pending_size_ = in_size;

	schannel_check(QueryContextAttributesW(__builtin_addressof(client->ctx_),
										   secpkg_attr_stream_sizes,
										   __builtin_addressof(client->stream_sizes_)));
	client->established_ = true;
}

template <typename socket_observer_type>
inline ::std::size_t tls_client_read_some(basic_schannel_tls_client<socket_observer_type> *client,
										  ::std::byte *buf, ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS
{
	if (!client->established_)
	{
		return static_cast<::std::size_t>(
			details::tls_read_some(client->sock_, buf, buf_size) - buf);
	}
	for (;;)
	{
		if (client->pt_pending_size_ != 0)
		{
			::std::size_t const n{client->pt_pending_size_ < buf_size
									  ? client->pt_pending_size_
									  : buf_size};
			__builtin_memcpy(buf, client->pt_pending_, n);
			__builtin_memmove(client->pt_pending_, client->pt_pending_ + n,
							  client->pt_pending_size_ - n);
			client->pt_pending_size_ -= n;
			return n;
		}
		schannel_buffer ib[4]{
			{static_cast<::std::uint_least32_t>(client->ct_pending_size_), secbuffer_data,
			 client->ct_pending_},
			{0, secbuffer_empty, nullptr},
			{0, secbuffer_empty, nullptr},
			{0, secbuffer_empty, nullptr}};
		schannel_buffer_desc id{0, 4, ib};
		auto const status{DecryptMessage(__builtin_addressof(client->ctx_),
										 __builtin_addressof(id), 0, nullptr)};


		/* DATA and EXTRA both alias ct_pending_ (decrypt is in-place):
		   copy DATA out first, then slide EXTRA ciphertext to the front
		   for the next DecryptMessage call */
		::std::byte const *data{};
		::std::size_t data_size{};
		::std::byte const *extra{};
		::std::size_t extra_size{};
		for (::std::uint_least32_t i{}; i != id.count; ++i)
		{
			if (id.buffers[i].type == secbuffer_data)
			{
				data = static_cast<::std::byte const *>(id.buffers[i].data);
				data_size = id.buffers[i].size;
			}
			else if (id.buffers[i].type == secbuffer_extra)
			{
				extra = static_cast<::std::byte const *>(id.buffers[i].data);
				extra_size = id.buffers[i].size;
			}
		}
		if (status == sec_e_ok || status == sec_i_renegotiate || status == sec_i_context_expired)
		{
			::std::size_t delivered{};
			if (data != nullptr)
			{
				delivered = data_size < buf_size ? data_size : buf_size;
				__builtin_memcpy(buf, data, delivered);
				if (delivered < data_size)
				{
					if (data_size - delivered > sizeof(client->pt_pending_))
					{
						::fast_io::throw_posix_error(EMSGSIZE);
					}
					__builtin_memcpy(client->pt_pending_, data + delivered, data_size - delivered);
					client->pt_pending_size_ = data_size - delivered;
				}
			}
			client->ct_pending_size_ = 0;
			if (extra != nullptr)
			{
				__builtin_memmove(client->ct_pending_, extra, extra_size);
				client->ct_pending_size_ = extra_size;
			}
			if (data != nullptr)
			{
				return delivered;
			}
			if (status == sec_i_context_expired)
			{
				return 0; /* close_notify */
			}
			/* no DATA buffer -- alert/padding only; loop again */
		}
		else if (status != sec_e_incomplete_message)
		{
			schannel_throw(status);
		}
		if (client->ct_pending_size_ == sizeof(client->ct_pending_))
		{
			::fast_io::throw_posix_error(EPROTO); /* record never completes */
		}
		auto const e{details::tls_read_some(
			client->sock_, client->ct_pending_ + client->ct_pending_size_,
			sizeof(client->ct_pending_) - client->ct_pending_size_)};
		if (e == client->ct_pending_ + client->ct_pending_size_)
		{
			return 0;
		}
		client->ct_pending_size_ = static_cast<::std::size_t>(e - client->ct_pending_);
	}
}

template <typename socket_observer_type>
inline ::std::size_t tls_client_write_some(basic_schannel_tls_client<socket_observer_type> *client,
										   ::std::byte const *buf, ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS
{
	if (!client->established_)
	{
		return static_cast<::std::size_t>(
			details::tls_write_some(client->sock_, buf, buf_size) - buf);
	}
	::std::size_t const limit{client->stream_sizes_.maximum_message};
	::std::size_t const n{buf_size < limit ? buf_size : limit};
	::std::size_t const total{client->stream_sizes_.header + n + client->stream_sizes_.trailer};
	::std::byte wire[16384 + 256];
	if (total > sizeof(wire))
	{
		::fast_io::throw_posix_error(EMSGSIZE);
	}
	::std::byte *const data{wire + client->stream_sizes_.header};
	__builtin_memcpy(data, buf, n);
	schannel_buffer ob[4]{
		{client->stream_sizes_.header, secbuffer_stream_header, wire},
		{static_cast<::std::uint_least32_t>(n), secbuffer_data, data},
		{client->stream_sizes_.trailer, secbuffer_stream_trailer,
		 data + n},
		{0, secbuffer_empty, nullptr}};
	schannel_buffer_desc od{0, 4, ob};
	schannel_check(EncryptMessage(__builtin_addressof(client->ctx_), 0,
								  __builtin_addressof(od), 0));
	::std::size_t const wire_size{ob[0].size + ob[1].size + ob[2].size};
	/* the pieces may not be contiguous after encrypt -- send them in
	   order */
	tls_write_full(client->sock_, static_cast<::std::byte const *>(ob[0].data), ob[0].size);
	tls_write_full(client->sock_, static_cast<::std::byte const *>(ob[1].data), ob[1].size);
	tls_write_full(client->sock_, static_cast<::std::byte const *>(ob[2].data), ob[2].size);
	(void)wire_size;
	return n;
}

template <typename socket_observer_type>
inline void tls_client_send_close_notify(basic_schannel_tls_client<socket_observer_type> *client) noexcept
{
	if (!client->have_ctx_)
	{
		return;
	}
	FAST_IO_HERBCEPTIONS_TRY
	{
		::std::uint_least32_t const shutdown_notify{0x1u}; /* SCHANNEL_SHUTDOWN */
		schannel_buffer sb{sizeof(shutdown_notify), secbuffer_token,
						   const_cast<::std::uint_least32_t *>(__builtin_addressof(shutdown_notify))};
		schannel_buffer_desc sd{0, 1, __builtin_addressof(sb)};
		ApplyControlToken(__builtin_addressof(client->ctx_), __builtin_addressof(sd));
		schannel_buffer ob{0, secbuffer_token, nullptr};
		schannel_buffer_desc od{0, 1, __builtin_addressof(ob)};
		schannel_timestamp expiry{};
		::std::uint_least32_t attrs{};
		auto const status{InitializeSecurityContextW(
			__builtin_addressof(client->cred_), __builtin_addressof(client->ctx_),
			nullptr, schannel_isc_reqs, 0, security_native_drep, nullptr, 0,
			__builtin_addressof(client->ctx_), __builtin_addressof(od),
			__builtin_addressof(attrs), __builtin_addressof(expiry))};
		schannel_out_token out_token{};
		if (status >= 0)
		{
			out_token.take(ob);
		}
		if (out_token.size != 0)
		{
			tls_write_full(client->sock_,
						   reinterpret_cast<::std::byte const *>(out_token.data), out_token.size);
		}
	}
	FAST_IO_HERBCEPTIONS_CATCH_ALL
	{
	}
}

} // namespace details

template <::std::integral ch_type, typename socket_observer_type = ::fast_io::native_socket_io_observer>
struct basic_schannel_tls_io_observer
{
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using native_handle_type = basic_schannel_tls_client<socket_observer_type> *;
	native_handle_type handle{};

	inline constexpr native_handle_type native_handle() const noexcept
	{
		return handle;
	}
	inline explicit constexpr operator bool() const noexcept
	{
		return handle != nullptr;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{handle};
		handle = nullptr;
		return temp;
	}
};

template <::std::integral ch_type, typename socket_observer_type>
inline constexpr basic_schannel_tls_io_observer<ch_type, socket_observer_type>
io_stream_ref_define(basic_schannel_tls_io_observer<ch_type, socket_observer_type> other) noexcept
{
	return other;
}

template <::std::integral ch_type, typename socket_observer_type>
inline constexpr basic_schannel_tls_io_observer<char, socket_observer_type>
io_bytes_stream_ref_define(basic_schannel_tls_io_observer<ch_type, socket_observer_type> other) noexcept
{
	return {other.handle};
}

template <::std::integral ch_type, typename socket_observer_type>
inline ::std::byte *read_some_bytes_underflow_define(basic_schannel_tls_io_observer<ch_type, socket_observer_type> tob,
													 ::std::byte *first, ::std::size_t count) FAST_IO_HERBCEPTIONS_THROWS
{
	return first + details::tls_client_read_some(tob.handle, first, count);
}

template <::std::integral ch_type, typename socket_observer_type>
inline ::std::byte const *write_some_bytes_overflow_define(basic_schannel_tls_io_observer<ch_type, socket_observer_type> tob,
														   ::std::byte const *first, ::std::size_t count) FAST_IO_HERBCEPTIONS_THROWS
{
	return first + details::tls_client_write_some(tob.handle, first, count);
}

template <::std::integral ch_type, typename socket_observer_type>
inline void handshake_define(basic_schannel_tls_io_observer<ch_type, socket_observer_type> tob,
							 tls_client_config cfg) FAST_IO_HERBCEPTIONS_THROWS
{
	details::tls_client_handshake(tob.handle, __builtin_addressof(cfg));
}

template <::std::integral ch_type, typename socket_observer_type>
inline void handshake_define(basic_schannel_tls_io_observer<ch_type, socket_observer_type> tob,
							 ::fast_io::u8cstring_view hostname) FAST_IO_HERBCEPTIONS_THROWS
{
	tls_client_config cfg{};
	cfg.hostname = hostname;
	details::tls_client_handshake(tob.handle, __builtin_addressof(cfg));
}

template <::std::integral ch_type, typename socket_observer_type>
inline void tls_close_notify(basic_schannel_tls_io_observer<ch_type, socket_observer_type> tob) noexcept
{
	details::tls_client_send_close_notify(tob.handle);
}

/* owning bundle, same shape as basic_tls / basic_ossl_tls */
template <typename socket_type, typename allocator_type = ::fast_io::native_global_allocator>
struct basic_schannel_tls
{
	using char_type = typename socket_type::char_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using socket_observer_type =
		decltype(::fast_io::io_stream_ref_define(::std::declval<socket_type &>()));
	using client_type = basic_schannel_tls_client<socket_observer_type>;

	socket_type socket;
	client_type client;

	inline constexpr basic_schannel_tls() noexcept
		requires(::std::is_default_constructible_v<socket_type>)
	= default;

	template <typename... Args>
		requires(::std::constructible_from<socket_type, Args...>)
	inline explicit constexpr basic_schannel_tls(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(socket_type(::std::forward<Args>(args)...))
		: socket(::std::forward<Args>(args)...), client{::fast_io::io_stream_ref_define(socket)}
	{
	}

	basic_schannel_tls(basic_schannel_tls const &) = delete;
	basic_schannel_tls &operator=(basic_schannel_tls const &) = delete;
	inline constexpr basic_schannel_tls(basic_schannel_tls &&other) noexcept
		: socket(::std::move(other.socket)), client(::std::move(other.client))
	{
	}
	inline basic_schannel_tls &operator=(basic_schannel_tls &&other) noexcept
	{
		if (__builtin_addressof(other) == this)
		{
			return *this;
		}
		socket = ::std::move(other.socket);
		client = ::std::move(other.client);
		return *this;
	}
};

template <typename socket_type, typename allocator_type>
inline constexpr basic_schannel_tls_io_observer<typename socket_type::char_type,
												typename basic_schannel_tls<socket_type, allocator_type>::socket_observer_type>
io_stream_ref_define(basic_schannel_tls<socket_type, allocator_type> &t) noexcept
{
	return {__builtin_addressof(t.client)};
}

template <typename socket_type, typename allocator_type>
inline constexpr basic_schannel_tls_io_observer<char,
												typename basic_schannel_tls<socket_type, allocator_type>::socket_observer_type>
io_bytes_stream_ref_define(basic_schannel_tls<socket_type, allocator_type> &t) noexcept
{
	return {__builtin_addressof(t.client)};
}

template <::std::integral ch_type, typename allocator_type = ::fast_io::native_global_allocator>
using basic_schannel_tls_socket_file = basic_schannel_tls<basic_native_socket_file<ch_type>, allocator_type>;

using schannel_tls_socket_file = basic_schannel_tls_socket_file<char>;
using u8schannel_tls_socket_file = basic_schannel_tls_socket_file<char8_t>;

template <::std::integral ch_type, typename allocator_type = ::fast_io::native_global_allocator>
using basic_iobuf_schannel_tls_socket_file =
	basic_iobuf<basic_schannel_tls_socket_file<ch_type, allocator_type>, allocator_type>;

using iobuf_schannel_tls_socket_file = basic_iobuf_schannel_tls_socket_file<char>;
using u8iobuf_schannel_tls_socket_file = basic_iobuf_schannel_tls_socket_file<char8_t>;

} // namespace fast_io::tls

#if defined(_MSC_VER) && !defined(_KERNEL_MODE)
#pragma comment(lib, "secur32.lib")
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#include "schannel_linker.h"
#endif

#endif
