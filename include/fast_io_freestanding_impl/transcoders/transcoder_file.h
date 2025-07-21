#pragma once

namespace fast_io
{

struct transcoder_file_base
{
	inline virtual constexpr bool transcode_always_none() const = 0;
	inline virtual constexpr ::std::size_t transcode_bytes_min_tosize() const = 0;
	inline virtual constexpr ::std::size_t transcode_bytes_size(::std::byte const *, ::std::byte const *, ::std::size_t) const = 0;
	inline virtual constexpr transcode_bytes_result transcode_bytes(::std::byte const *, ::std::byte const *, ::std::byte *, ::std::byte *) = 0;
	inline virtual constexpr ::std::size_t transcode_bytes_do_final_tosize() const = 0;
	inline virtual constexpr ::std::byte *transcode_bytes_do_final(::std::byte *, ::std::byte *) = 0;
	inline virtual void destroy() noexcept = 0;
	inline constexpr ~transcoder_file_base() = default;
};

template <typename allocator, typename T>
	requires(::fast_io::transcoder<T> || ::fast_io::byte_transcoder<T>)
struct transcoder_file_derived final : transcoder_file_base
{
	T object;
	inline explicit constexpr basic_transcoder_file(::fast_io::io_cookie_type_t<T>, Args &&args...)
		: object(::std::forward<Args>(args)...)
	{
	}
	inline constexpr bool transcode_always_none() const override
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
	inline constexpr ::std::size_t transcode_bytes_min_tosize() const override
	{
		concept has_transcode_bytes_min_tosize = requires() {
			{ object.transcode_bytes_min_tosize() } -> ::std::same_as<::std::size_t>;
		};
		if constexpr (has_transcode_bytes_min_tosize)
		{
			return object.transcode_bytes_min_tosize();
		}
		else if (!has_transcode_bytes_min_tosize)
		{
			return object.transcode_min_tosize() * sizeof(typename T::to_value_type);
		}
	}
	inline constexpr ::std::size_t transcode_bytes_size(::std::byte const *fromfirst, ::std::byte const *fromlast, ::std::size_t mxsz) const override
	{
		concept has_transcode_bytes_size = requires() {
			{ object.transcode_bytes_size(fromfirst, fromlast, mxsz) } -> ::std::same_as<::std::size_t>;
		};
		if constexpr (has_transcode_bytes_size)
		{
			return object.transcode_bytes_size(fromfirst, fromlast, mxsz);
		}
		else if constexpr (!has_transcode_bytes_size)
		{
			using from_value_type = typename T::from_value_type;
			using to_value_type = typename T::to_value_type;
			using from_const_ptr_may_alias
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= from_value_type const *;
			return object.transcode_size(reinterpret_cast<from_const_ptr_may_alias>(fromfirst),
										 reinterpret_cast<from_const_ptr_may_alias>(fromfirst) + static_cast<::std::size_t>(fromlast - fromfirst) / sizeof(from_value_type),
										 mxsz / sizeof(to_value_type)) *
				   sizeof(from_value_type);
		}
	}
	inline constexpr transcode_bytes_result transcode_bytes(::std::byte const *fromfirst, ::std::byte const *fromlast, ::std::byte *tofirst, ::std::byte *tolast) override
	{
		concept has_transcode_bytes = requires() {
			{ object.transcode_bytes(fromfirst, fromlast, tofirst, tolast) } -> ::std::same_as<transcode_bytes_result>;
		};
		if constexpr (transcode_bytes_result)
		{
			return object.transcode_bytes(fromfirst, fromlast, tofirst, tolast);
		}
		else
		{
			using from_value_type = typename T::from_value_type;
			using to_value_type = typename T::to_value_type;
			using from_const_ptr_may_alias
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= from_value_type const *;
			using to_ptr_may_alias
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= to_value_type *;
			auto [fromit, toit] = object.transcode(reinterpret_cast<from_const_ptr_may_alias>(fromfirst),
												   reinterpret_cast<from_const_ptr_may_alias>(fromfirst) + static_cast<::std::size_t>(fromlast - fromfirst) / sizeof(from_value_type),
												   reinterpret_cast<to_ptr_may_alias>(tofirst),
												   reinterpret_cast<to_ptr_may_alias>(tofirst) + static_cast<::std::size_t>(tolast - tofirst) / sizeof(to_value_type));
			return {reinterpret_cast<::std::byte const *>(fromit), reinterpret_cast<::std::byte *>(toit)};
		}
	}
	inline constexpr ::std::size_t transcode_bytes_do_final_tosize() override
	{
		if constexpr (requires() {
						  { object.transcode_bytes_do_final_tosize() } -> ::std::same_as<::std::size_t>;
					  })
		{
			return object.transcode_bytes_do_final_tosize();
		}
		else if constexpr (requires() {
							   { object.transcode_do_final_tosize() } -> ::std::same_as<::std::size_t>;
						   })
		{
			return object.transcode_do_final_tosize() * sizeof(typename T::to_value_type);
		}
		else
		{
			return 0;
		}
	}
	inline constexpr ::std::byte *transcode_bytes_do_final(::std::byte *tofirst, ::std::byte *tolast) override
	{
		if constexpr (requires() {
						  { object.transcode_bytes_do_final(tofirst, tolast) } -> ::std::same_as<::std::byte *>;
					  })
		{
			return object.transcode_bytes_do_final(tofirst, tolast);
		}
		else if constexpr (requires() {
							   typename T::to_value_type;
							   requires(typename T::to_value_type * tovtpit) {
								   { object.transcode_do_final(tovtpit, tovtpit) } -> ::std::same_as<typename T::to_value_type *>;
							   };
						   })
		{
			using to_value_type = typename T::to_value_type;
			using to_ptr_may_alias
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= to_value_type *;
			return reinterpret_cast<::std::byte *>(object.transcode_do_final(reinterpret_cast<to_ptr_may_alias>(tofirst),
																			 reinterpret_cast<to_ptr_may_alias>(tofirst) + static_cast<::std::size_t>(tolast - tofirst) / sizeof(to_value_type)));
		}
		else
		{
			return fromfirst;
		}
	}
	inline constexpr void destroy() noexcept override
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
	inline constexpr native_handle_type release() noexcept
	{
		auto temptranshandle{transhandle};
		this->transhandle = nullptr;
		return temptranshandle;
	}
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return this->transhandle;
	}
	inline constexpr bool transcode_always_none() const
	{
		return transhandle->transcode_always_none();
	}
	inline constexpr ::std::size_t transcode_bytes_minsize() const
	{
		return transhandle->transcode_bytes_minsize();
	}
	inline constexpr ::std::size_t transcode_bytes_size(::std::byte const *fromfirst, ::std::byte const *fromlast, ::std::size_t mxsz) const
	{
		return transhandle->transcode_bytes_size(fromfirst, fromlast, mxsz);
	}
	inline constexpr transcode_bytes_result transcode_bytes(::std::byte const *fromfirst, ::std::byte const *fromlast, ::std::byte *tofirst, ::std::byte *tolast)
	{
		return transhandle->transcode_bytes(fromfirst, fromlast, tofirst, tolast);
	}
	inline constexpr ::std::size_t transcode_bytes_do_final_tosize() const
	{
		return transhandle->transcode_bytes_do_final_tosize();
	}

	inline constexpr ::std::byte *transcode_bytes_do_final(::std::byte *tofirst, ::std::byte *tolast)
	{
		return transhandle->transcode_bytes_do_final(tofirst, tolast);
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
	inline explicit constexpr basic_transcoder_file(::fast_io::io_cookie_type_t<T>, Args &&args...)
		: transcoder_io_observer{::std::construct_at(typed_allocator_adapter<allocator, ::fast_io::transcoder_file_derived<allocator, T>>::allocate(1), ::fast_io::io_cookie_type<T>, ::std::forward<Args>(args)...)}
	{
	}
	inline constexpr basic_transcoder_file(basic_transcoder_file const &) = delete;
	inline constexpr basic_transcoder_file &operator=(basic_transcoder_file const &) = delete;

	inline constexpr basic_transcoder_file(basic_transcoder_file &&other) noexcept
		: transhandle{other.handle}
	{
		other.transhandle = nullptr;
	}
	inline constexpr basic_transcoder_file &operator=(basic_transcoder_file &&other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		this->transhandle->destroy();
		this->transhandle = other.transhandle;
		other.transhandle = nullptr;
		return *this;
	}
	inline constexpr void close()
	{
		this->transhandle->destroy();
		this->transhandle = nullptr;
	}
	inline constexpr ~basic_transcoder_file()
	{
		this->transhandle->destroy();
	}
};

} // namespace fast_io
