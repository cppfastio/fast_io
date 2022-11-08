#pragma once

namespace fast_io
{

template<std::integral char_type,typename T>
struct io_strlike_type_t
{
	explicit constexpr io_strlike_type_t() noexcept = default;
};

template<std::integral char_type,typename T>
inline constexpr io_strlike_type_t<char_type,T> io_strlike_type{};

namespace details
{
template<typename char_type,typename T>
concept buffer_strlike_impl = requires(T& t)
{
	{strlike_begin(io_strlike_type<char_type,T>,t)};
	{strlike_curr(io_strlike_type<char_type,T>,t)};
	{strlike_end(io_strlike_type<char_type,T>,t)};
	requires requires(decltype(strlike_begin(io_strlike_type<char_type,T>,t)) ptr)
	{
		{strlike_set_curr(io_strlike_type<char_type,T>,t,ptr)};
	};
	requires requires(std::size_t n)
	{
		{strlike_reserve(io_strlike_type<char_type,T>,t,n)};
	};
};
}

template<typename char_type,typename T>
concept strlike = std::integral<char_type>&&std::is_default_constructible_v<T>&&(requires(char_type const* first)
{
	strlike_construct_define(io_strlike_type<char_type,T>,first,first);
}||::fast_io::details::buffer_strlike_impl<char_type,T>);

template<typename char_type,typename T>
concept single_character_constructible_strlike = strlike<char_type,T>&&requires(char_type ch)
{
	strlike_construct_single_character_define(io_strlike_type<char_type,T>,ch);
};

template<typename char_type,typename T>
concept alias_strlike = requires(T& t)
{
	strlike_alias_define(io_alias,t);
};

template<typename char_type,typename T>
concept buffer_strlike = strlike<char_type,T> && requires(T& t)
{
	{strlike_begin(io_strlike_type<char_type,T>,t)};
	{strlike_curr(io_strlike_type<char_type,T>,t)};
	{strlike_end(io_strlike_type<char_type,T>,t)};
	requires requires(decltype(strlike_begin(io_strlike_type<char_type,T>,t)) ptr)
	{
		{strlike_set_curr(io_strlike_type<char_type,T>,t,ptr)};
	};
	requires requires(std::size_t n)
	{
		{strlike_reserve(io_strlike_type<char_type,T>,t,n)};
	};
};

template<typename char_type,typename T>
concept auxiliary_strlike = strlike<char_type,T> && requires(T& t,char_type ch,char_type const* ptr)
{
	strlike_push_back(io_strlike_type<char_type,T>,t,ch);
	strlike_append(io_strlike_type<char_type,T>,t,ptr,ptr);
};

template<typename char_type,typename T>
concept sso_buffer_strlike = buffer_strlike<char_type,T> && requires()
{
	{strlike_sso_size(io_strlike_type<char_type,T>)}->std::same_as<std::size_t>;
};

}
