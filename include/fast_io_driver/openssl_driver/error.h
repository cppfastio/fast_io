#pragma once

namespace fast_io
{

#if !defined(__HERBCEPTIONS__) && defined(FAST_IO_CPP_EXCEPTIONS)
class openssl_error
{
};
#endif

/* OpenSSL failures become std::tls_alert or std::errc through the
   herbceptions channel: fatal-alert reasons pack the wire description
   at SSL_AD_REASON_OFFSET + description, ERR_LIB_SYS carries a raw
   errno, and anything else degrades to alert internal_error. */
[[noreturn]] inline void throw_openssl_error() FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__HERBCEPTIONS__)
	unsigned long const code{ERR_get_error()};
	int const reason{ERR_GET_REASON(code)};
	if (reason > SSL_AD_REASON_OFFSET)
	{
		throw throws::std::tls_alert{
			::std::tls_alert::alert_level::fatal,
			static_cast<::std::tls_alert::alert_description>(reason - SSL_AD_REASON_OFFSET)};
	}
	switch (reason)
	{
	case SSL_R_CERTIFICATE_VERIFY_FAILED:
		throw throws::std::tls_alert{::std::tls_alert::alert_level::fatal,
									 ::std::tls_alert::alert_description::bad_certificate};
	case SSL_R_UNEXPECTED_EOF_WHILE_READING:
	case SSL_R_UNEXPECTED_MESSAGE:
		throw throws::std::tls_alert{::std::tls_alert::alert_level::fatal,
									 ::std::tls_alert::alert_description::unexpected_message};
	default:
		break;
	}
	if (ERR_GET_LIB(code) == ERR_LIB_SYS && reason > 0)
	{
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
		/* the queue carries GetLastError/WSA codes here, not errno */
		::fast_io::herbceptions::throws_win32_errc_with_value(
			static_cast<::std::uint_least32_t>(reason));
#else
		::fast_io::herbceptions::throws_errc_with_value(reason);
#endif
	}
	throw throws::std::tls_alert{::std::tls_alert::alert_level::fatal,
								 ::std::tls_alert::alert_description::internal_error};
#elif defined(FAST_IO_CPP_EXCEPTIONS)
	throw openssl_error{};
#else
	::fast_io::fast_terminate();
#endif
}

} // namespace fast_io
