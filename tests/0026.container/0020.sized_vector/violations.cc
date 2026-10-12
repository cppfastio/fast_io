#include <fast_io.h>
#include <fast_io_dsal/sized_vector.h>
#include <fast_io_dsal/sized_string.h>
#include <fast_io_core_impl/allocation/adapters.h>
#include <fast_io_core_impl/allocation/c_malloc.h>
#include <system_error>

namespace
{

using throwing_alloc = ::fast_io::generic_allocator_adapter<::fast_io::c_malloc_allocator,
															::fast_io::allocator_adapter_flags::throws_on_allocation_failure |
																::fast_io::allocator_adapter_flags::throws_on_violations>;

} // namespace

int main()
{
	using V = ::fast_io::sized_vector_zu32<int>;
	using TV = ::fast_io::sized_vector<::fast_io::size32_t, int, throwing_alloc>;
	static_assert(!noexcept(::std::declval<TV &>().operator[](0u)));
	static_assert(!noexcept(::std::declval<TV &>().push_back(1)));
	static_assert(!noexcept(::std::declval<TV &>().reserve(1u)));
	{
		TV v{1, 2, 3};
#ifdef __HERBCEPTIONS__
		bool caught{};
		try
		{
			static_cast<void>(v[10]);
		}
		catch throws(::std::error)
		{
			caught = true;
		}
		if (!caught)
		{
			::fast_io::fast_terminate();
		}
#endif
	}
	// deduction guides pick size32_t as the size type
	::fast_io::containers::sized_vector dv{1, 2, 3};
	static_assert(::std::same_as<decltype(dv)::size_type, ::fast_io::size32_t>);

	using TS = ::fast_io::basic_sized_string<::fast_io::size32_t, char, throwing_alloc>;
	static_assert(!noexcept(::std::declval<TS &>().push_back('x')));
	static_assert(!noexcept(::std::declval<TS &>().operator[](0u)));
	{
		TS ts{"abc"};
#ifdef __HERBCEPTIONS__
		bool caught{};
		try
		{
			static_cast<void>(ts[10]);
		}
		catch throws(::std::error)
		{
			caught = true;
		}
		if (!caught)
		{
			::fast_io::fast_terminate();
		}
#endif
	}
	// mixed uinttype comparison is allowed like mixed allocators
	::fast_io::sized_vector<::std::size_t, int> w64{1, 2, 3};
	if (!(dv == w64))
	{
		::fast_io::fast_terminate();
	}
	static_cast<void>(sizeof(V));
}
