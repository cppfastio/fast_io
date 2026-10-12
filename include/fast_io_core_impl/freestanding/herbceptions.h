#pragma once

namespace fast_io::freestanding
{
#ifdef __HERBCEPTIONS__

#if defined(_WIN32) || defined(__CYGWIN__)
using ::std::win32_errc;
using ::std::nt_errc;
using ::std::wine_errc;
using ::std::com_errc;
#endif

using ::std::cmath_errc;
using ::std::parse_errc;
using ::std::http_errc;

using ::std::cxx_std_error;

#else
#if defined(_WIN32) || defined(__CYGWIN__)
// placeholders
enum class win32_errc : ::std::uint_least32_t
{
};
enum class nt_errc : ::std::uint_least32_t
{
};
enum class wine_errc : ::std::uint_least32_t
{
};
enum class com_errc : ::std::uint_least32_t
{
};
#endif

enum class cmath_errc : ::std::uint_least32_t
{
};
enum class parse_errc : ::std::uint_least32_t
{
	ok = 0,
	end_of_file = 1,
	partial = 2,
	invalid = 3,
	overflow = 4
};

enum class http_errc : ::std::uint_least32_t
{
};

struct cxx_std_error
{
	void const *domain{};
	::std::size_t code{};
};

#endif
#if defined(__HERBCEPTIONS__)
struct cxx_std_error_guard
{
public:
	::std::cxx_std_error err{};
	constexpr cxx_std_error_guard() noexcept = default;
	explicit constexpr cxx_std_error_guard(::std::cxx_std_error other) noexcept
		: err(other)
	{
	}
	constexpr cxx_std_error_guard(cxx_std_error_guard const &) = delete;
	constexpr cxx_std_error_guard &operator=(cxx_std_error_guard const &) = delete;
	constexpr cxx_std_error_guard(cxx_std_error_guard &&other) noexcept
		: err(other.err)
	{
		other.err = {};
	}
	constexpr cxx_std_error_guard &operator=(cxx_std_error_guard &&other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		this->destroy();
		this->err = other.err;
		other.err = {};
		return *this;
	}
	/* overwrite the held error, releasing the previous payload's
	 * resources through its domain's do_cleanup when it owns any */
	constexpr cxx_std_error_guard &operator=(::std::cxx_std_error other) noexcept
	{
		this->destroy();
		this->err = other;
		return *this;
	}
	constexpr void rethrow_if_cxx_std_error() FAST_IO_HERBCEPTIONS_THROWS
	{
		if (this->err.domain != nullptr)
		{
			throw throws this->release();
		}
	}
	constexpr ::std::cxx_std_error release() noexcept
	{
		auto tmp{this->err};
		this->err = {};
		return tmp;
	}
	constexpr ~cxx_std_error_guard() noexcept
	{
		this->destroy();
	}

private:
	/* domain == nullptr is not an error at all. Otherwise the payload may
	 * own resources — a C++ exception object handle is a typical one — and
	 * the domain's do_cleanup releases it */
	constexpr void destroy() noexcept
	{
		if (this->err.domain != nullptr)
		{
			auto domain{static_cast<::std::error_domain_singleton const *>(this->err.domain)};
			auto code{this->err.code};
			this->err = {};
			if (domain->do_cleanup != nullptr)
			{
				domain->do_cleanup(code);
			}
		}
	}
};

#endif /* defined(__HERBCEPTIONS__) */

} // namespace fast_io::freestanding
