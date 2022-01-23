#pragma once

namespace fast_io::cryptopp
{

template<typename T>
requires requires(T handle,
	char unsigned const* constptr,
	char unsigned* ptr,std::size_t n)
{
	T::DIGESTSIZE;
	handle.Update(constptr,n);
	handle.Restart();
	handle.Final(ptr);
}
class iterated_hash_context
{
public:
	using native_handle_type = T;
	native_handle_type handle;
	static inline constexpr std::size_t digest_size{static_cast<std::size_t>(native_handle_type::DIGESTSIZE)};
	std::byte digest_buffer[digest_size];
	void update(std::byte const* first,std::byte const* last)
	{
		handle.Update(reinterpret_cast<char unsigned const*>(first),static_cast<std::size_t>(last-first));
	}
	void reset()
	{
		handle.Restart();
	}
	void do_final()
	{
		handle.Final(reinterpret_cast<char unsigned*>(digest_buffer));
	}
	constexpr void digest_to_byte_ptr(std::byte* __restrict dest) const noexcept
	{
		::fast_io::freestanding::nonoverlapped_bytes_copy_n(digest_buffer,digest_size,dest);
	}
};

}
