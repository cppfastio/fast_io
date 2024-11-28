#pragma once

namespace fast_io
{

template <typename T>
struct transcode_reserve_t
{
	inline explicit constexpr transcode_reserve_t() noexcept = default;
};
template <typename T>
inline constexpr transcode_reserve_t<T> transcode_reserve{};

template <typename T>
struct transcoder_ref
{
	using value_type = T;
	using pointer = T *;
	pointer ptr{};
};

template <typename fromvtype, typename tovtype>
struct transcode_result
{
	using from_value_type = fromvtype;
	using to_value_type = tovtype;
	from_value_type const *fromit{};
	to_value_type *toit{};
};

using transcode_bytes_result = transcode_result<::std::byte, ::std::byte>;

namespace operations::defines
{
template <typename T>
concept has_transcode_ref_define = requires(T &ref) {
	transcode_ref_define(ref);
};
} // namespace operations::defines

template <typename T>
inline constexpr decltype(auto) transcode_ref(T &ref) noexcept
{
	if constexpr (::fast_io::operations::defines::has_transcode_ref_define<T>)
	{
		return transcode_ref_define(ref);
	}
	else
	{
		return transcoder_ref<::std::remove_reference_t<T>>{__builtin_addressof(ref)};
	}
}

namespace operations::decay::defines
{

template <typename T>
concept has_transcode_bytes_decay_define = requires(T t, ::std::byte const *ptr) {
	{ transcode_bytes_decay_define(t, ptr, ptr, ptr, ptr) } -> ::std::same_as<::fast_io::transcode_bytes_result>;
};

template <typename T>
concept has_transcode_bytes_tosize_decay_define = requires(T t, ::std::byte const *ptr) {
	{ transcode_bytes_tosize_decay_define(t, ptr, ptr) } -> ::std::same_as<::std::size_t>;
};

template <typename T>
concept has_transcode_bytes_min_tosize_decay_define = requires() {
	{ transcode_bytes_min_tosize_decay_define(::fast_io::transcode_reserve<T>) } -> ::std::same_as<::std::size_t>;
};

template <typename T>
concept has_transcode_bytes_imaginary_decay_define = requires(T t, ::std::byte const *ptr, ::std::size_t mxsz) {
	{ transcode_bytes_imaginary_decay_define(t, ptr, ptr, mxsz) } -> ::std::same_as<::std::byte const *>;
};

template <typename T>
concept has_transcode_decay_define = requires(T t, typename T::from_value_type const *fromit, typename T::to_value_type *toit) {
	{ transcode_decay_define(t, fromit, fromit, toit, toit) } -> ::std::same_as<::fast_io::transcode_result<typename T::from_value_type, typename T::to_value_type>>;
};

template <typename T>
concept has_transcode_tosize_decay_define = requires(T t, typename T::from_value_type const *fromit) {
	{ transcode_tosize_decay_define(t, fromit, fromit) } -> ::std::same_as<::std::size_t>;
};

template <typename T>
concept has_transcode_min_tosize_decay_define = requires() {
	{ transcode_min_tosize_decay_define(::fast_io::transcode_reserve<T>) } -> ::std::same_as<::std::size_t>;
};

template <typename T>
concept has_transcode_imaginary_decay_define = requires(T t, typename T::from_value_type const *fromit, ::std::size_t mxsz) {
	{ transcode_imaginary_decay_define(t, fromit, fromit, mxsz) } -> ::std::same_as<typename T::from_value_type const *>;
};

} // namespace operations::decay::defines

namespace operations::defines
{
template <typename T>
concept has_transcode_bytes_define = requires(T &t, ::std::byte const *ptr) {
	{ transcode_bytes_decay(::fast_io::transcode_ref(t), ptr, ptr, ptr, ptr) } -> ::std::same_as<::fast_io::transcode_bytes_result>;
};

template <typename T>
concept has_transcode_bytes_max_tosize_define = requires(T &t, ::std::byte const *ptr) {
	{ transcode_bytes_max_tosize_decay(::fast_io::transcode_ref(t), ptr, ptr) } -> ::std::same_as<::std::size_t>;
};

template <typename T>
concept has_transcode_bytes_min_tosize_define = requires(T &t) {
	{ transcode_bytes_min_tosize_decay(::fast_io::transcode_reserve<decltype(::fast_io::transcode_ref(t))>) } -> ::std::same_as<::std::size_t>;
};

template <typename T>
concept has_transcode_bytes_imaginary_define = requires(T &t, ::std::byte const *ptr, ::std::size_t mxsz) {
	{ transcode_bytes_imaginary_decay(::fast_io::transcode_ref(t), ptr, ptr, mxsz) } -> ::std::same_as<::std::byte const *>;
};
} // namespace operations::defines

} // namespace fast_io
