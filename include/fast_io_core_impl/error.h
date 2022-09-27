#pragma once

namespace fast_io
{

struct error
{
	using value_type = std::uintptr_t;
	value_type domain{};
	value_type code{};
};

inline constexpr bool operator==(error e1,error e2) noexcept
{
	return e1.domain==e2.domain&&e1.code==e2.code;
}

inline constexpr bool operator!=(error e1,error e2) noexcept
{
	return !(e1==e2);
}

template<typename T>
struct error_type_t
{
explicit inline constexpr error_type_t() noexcept = default;
};

template<typename T>
inline constexpr error_type_t<T> error_type{};

template<typename T>
concept error_domain = std::is_trivially_copyable_v<T> && sizeof(T)<=sizeof(std::uintptr_t) &&
	requires(error e,T t)
{
	{domain_define(error_type<T>)}->std::same_as<std::uintptr_t>;
	{equivalent_define(error_type<T>,e,t)}->std::same_as<bool>;
#if 0
	{to_code_define(error_type<T>,e)}->std::same_as<T>;
#endif
};

template<error_domain D>
inline constexpr bool is_domain(error e) noexcept
{
	constexpr std::uintptr_t v{error_domain_define(error_type<D>)};
	return v==e.domain;
}

template<error_domain D>
inline constexpr D to_code(error e) noexcept
{
	return to_code_define(error_type<D>,e);
}

template<error_domain D>
inline constexpr bool operator==(error e,D t) noexcept
{
	return equivalent_define(error_type<D>,e,t);
}

template<error_domain D>
inline constexpr bool operator==(D t,error e) noexcept
{
	return equivalent_define(error_type<D>,e,t);
}

template<error_domain D>
inline constexpr bool operator!=(error e,D t) noexcept
{
	return !equivalent_define(error_type<D>,e,t);
}

template<error_domain D>
inline constexpr bool operator!=(D t,error e) noexcept
{
	return !equivalent_define(error_type<D>,e,t);
}

}
