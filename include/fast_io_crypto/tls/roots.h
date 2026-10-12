#pragma once

/*
trust-root store: decodes every CERTIFICATE block from a PEM bundle
into DER and presents them in the flat array form tls_client_config
wants. Hosted only (reads files via native_file_loader).
*/


#include <cstdlib>

namespace fast_io::tls
{

template <typename allocator_type = ::fast_io::native_global_allocator>
struct basic_root_store
{
	using allocator_handle_type = typename allocator_type::handle_type;
	static inline constexpr bool alloc_with_status{
		allocator_type::has_status};

	::fast_io::vector<::std::byte, allocator_type> der_data;
	::fast_io::vector<::std::byte const *, allocator_type> ptrs;
	::fast_io::vector<::std::size_t, allocator_type> sizes;
	inline constexpr basic_root_store() noexcept
		requires(!alloc_with_status)
	= default;
	inline explicit constexpr basic_root_store(allocator_handle_type hdl) noexcept
		requires(alloc_with_status)
		: der_data{hdl}, ptrs{hdl}, sizes{hdl}
	{
	}
};

namespace details
{

/* pointer values depend on der_data's final allocation */
template <typename allocator_type>
inline void root_store_finalize(basic_root_store<allocator_type> *store) noexcept
{
	for (::std::size_t i{}, off{}; i != store->sizes.size(); ++i)
	{
		store->ptrs[i] = store->der_data.data() + off;
		off += store->sizes[i];
	}
}

/* append every CERTIFICATE pem block from pem */
template <typename allocator_type>
inline void root_store_load_pem_text(basic_root_store<allocator_type> *store,
									 ::fast_io::u8string_view pem) FAST_IO_HERBCEPTIONS_THROWS
{
	char8_t const *cur{pem.data()};
	char8_t const *const end{cur + pem.size()};
	char8_t const *label, *b64;
	::std::size_t label_size, b64_size;
	while (details::pem_next_block(cur, end, label, label_size, b64, b64_size))
	{
		if (label_size != 11 ||
			::fast_io::freestanding::my_memcmp(label, u8"CERTIFICATE", 11) != 0)
		{
			continue;
		}
		/* worst case decoded size ~ b64_size; grow once */
		::std::size_t const off{store->der_data.size()};
		store->der_data.resize(off + b64_size);
		::std::size_t const n{details::pem_decode_block(store->der_data.data() + off, b64_size,
														b64, b64_size)};
		if (n == 0)
		{
			store->der_data.resize(off);
			continue;
		}
		store->der_data.resize(off + n);
		store->sizes.push_back(n);
		store->ptrs.push_back(nullptr); /* patched by root_store_finalize(store) */
	}
	root_store_finalize(store);
}

/* decode a pem bundle file (e.g. /etc/ssl/certs/ca-certificates.crt) */
template <typename allocator_type, ::fast_io::constructible_to_os_c_str T>
inline void root_store_load_pem_file(basic_root_store<allocator_type> *store, T const &file) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::native_file_loader f{file};
	root_store_load_pem_text(store, ::fast_io::u8string_view{reinterpret_cast<char8_t const *>(f.data()), f.size()});
}

/*
the system trust bundle: $SSL_CERT_FILE first (openssl convention),
then the well-known per-distro bundle paths.
*/
template <typename allocator_type>
inline void root_store_load_system(basic_root_store<allocator_type> *store) FAST_IO_HERBCEPTIONS_THROWS
{
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
	constexpr ::std::size_t pathmax{32767};
	native_char_type envbuf[pathmax];
	::std::uint_least32_t n{};
	if constexpr (::fast_io::win32_family::native == ::fast_io::win32_family::ansi_9x)
	{
		n = ::fast_io::win32::GetEnvironmentVariableA(
			reinterpret_cast<char const *>(u8"SSL_CERT_FILE"), reinterpret_cast<char *>(envbuf), pathmax);
	}
	else
	{
		n = ::fast_io::win32::GetEnvironmentVariableW(
			u"SSL_CERT_FILE", reinterpret_cast<char16_t *>(envbuf), pathmax);
	}
	if (n != 0 && n < pathmax)
	{
		root_store_load_pem_file(store, ::fast_io::mnp::os_c_str_with_known_size(envbuf, n));
		return;
	}
#else
	if (char const *env{::std::getenv(reinterpret_cast<char const *>(u8"SSL_CERT_FILE"))}; env != nullptr && *env != 0)
	{
		root_store_load_pem_file(store, ::fast_io::mnp::os_c_str(env));
		return;
	}
#endif
	if constexpr (sizeof(native_char_type) == 2)
	{
		constexpr ::fast_io::basic_cstring_view<char16_t> paths[]{
			::fast_io::basic_cstring_view<char16_t>(u"/etc/ssl/certs/ca-certificates.crt"),                /* debian/ubuntu/arch */
			::fast_io::basic_cstring_view<char16_t>(u"/etc/pki/tls/certs/ca-bundle.crt"),                  /* fedora/rhel */
			::fast_io::basic_cstring_view<char16_t>(u"/etc/ssl/ca-bundle.pem"),                            /* opensuse */
			::fast_io::basic_cstring_view<char16_t>(u"/etc/pki/tls/cacert.pem"),                           /* openelec */
			::fast_io::basic_cstring_view<char16_t>(u"/etc/pki/ca-trust/extracted/pem/tls-ca-bundle.pem"), /* centos/rhel p11-kit */
			::fast_io::basic_cstring_view<char16_t>(u"/etc/ca-certificates/extracted/tls-ca-bundle.pem"),  /* fedora ca-certificates-ng */
			::fast_io::basic_cstring_view<char16_t>(u"/etc/ssl/cert.pem"),                                 /* alpine/macos-ish */
		};
		for (auto const p : paths)
		{
			FAST_IO_HERBCEPTIONS_TRY
			{
				root_store_load_pem_file(store, p);
				return;
			}
			FAST_IO_HERBCEPTIONS_CATCH_ALL
			{
			}
		}
	}
	else
	{
		constexpr ::fast_io::basic_cstring_view<char8_t> paths[]{
			::fast_io::basic_cstring_view<char8_t>(u8"/etc/ssl/certs/ca-certificates.crt"),                /* debian/ubuntu/arch */
			::fast_io::basic_cstring_view<char8_t>(u8"/etc/pki/tls/certs/ca-bundle.crt"),                  /* fedora/rhel */
			::fast_io::basic_cstring_view<char8_t>(u8"/etc/ssl/ca-bundle.pem"),                            /* opensuse */
			::fast_io::basic_cstring_view<char8_t>(u8"/etc/pki/tls/cacert.pem"),                           /* openelec */
			::fast_io::basic_cstring_view<char8_t>(u8"/etc/pki/ca-trust/extracted/pem/tls-ca-bundle.pem"), /* centos/rhel p11-kit */
			::fast_io::basic_cstring_view<char8_t>(u8"/etc/ca-certificates/extracted/tls-ca-bundle.pem"),  /* fedora ca-certificates-ng */
			::fast_io::basic_cstring_view<char8_t>(u8"/etc/ssl/cert.pem"),                                 /* alpine/macos-ish */
		};
		for (auto const p : paths)
		{
			FAST_IO_HERBCEPTIONS_TRY
			{
				root_store_load_pem_file(store, p);
				return;
			}
			FAST_IO_HERBCEPTIONS_CATCH_ALL
			{
			}
		}
	}
	::fast_io::throw_posix_error(ENOENT);
}

} // namespace details

using root_store = basic_root_store<>;

namespace details
{

/* hostname form: system trust bundle, chain + SAN checks on */
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_client_handshake(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
								 ::fast_io::u8cstring_view hostname) FAST_IO_HERBCEPTIONS_THROWS
{
	auto store{tls_alloc_construct<basic_root_store<allocator_type>, allocator_type>(client->allocator_handle)};
	root_store_load_system(__builtin_addressof(store));
	tls_client_config cfg{};
	cfg.hostname = hostname;
	cfg.roots = store.ptrs.data();
	cfg.root_sizes = store.sizes.data();
	cfg.root_count = store.sizes.size();
	tls_client_handshake(client, __builtin_addressof(cfg));
}

} // namespace details

template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto>
inline void handshake_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob,
							 ::fast_io::u8cstring_view hostname) FAST_IO_HERBCEPTIONS_THROWS
{
	details::tls_client_handshake(tob.handle, hostname);
}

} // namespace fast_io::tls
