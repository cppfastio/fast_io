#include <fast_io.h>
#include <fast_io_dsal/sized_vector.h>

namespace
{

struct nontrivial
{
	int x;
	nontrivial(int v)
		: x(v)
	{
	}
	nontrivial(nontrivial const &o)
		: x(o.x)
	{
	}
	nontrivial(nontrivial &&o) noexcept
		: x(o.x)
	{
	}
	~nontrivial()
	{
	}
	nontrivial &operator=(nontrivial const &) = default;
	nontrivial &operator=(nontrivial &&) = default;
};

} // namespace

int main()
{
	using V = ::fast_io::sized_vector_zu32<int>;
	// 1 pointer + 2 32-bit sizes: 16 bytes instead of 24
	static_assert(sizeof(V) == 16);
	static_assert(::std::same_as<V::size_type, ::fast_io::size32_t>);
	static_assert(::std::same_as<V::iterator, int *>);
	static_assert(::std::same_as<V::const_iterator, int const *>);
	static_assert(noexcept(::std::declval<V &>().push_back(1)));
	static_assert(noexcept(::std::declval<V &>().emplace_back(1)));
	static_assert(noexcept(::std::declval<V &>().operator[](0u)));
	static_assert(noexcept(::std::declval<V &>().reserve(1u)));

	V v;
	for (int i = 0; i != 100; ++i)
	{
		v.push_back(i);
	}
	if (v.size() != 100 || v[50] != 50)
	{
		::fast_io::fast_terminate();
	}
	v.insert(v.begin() + 2, 5, 7);
	if (v.size() != 105 || v[2] != 7 || v[7] != 2)
	{
		::fast_io::fast_terminate();
	}
	v.erase(v.begin() + 2, v.begin() + 7);
	if (v.size() != 100 || v[2] != 2)
	{
		::fast_io::fast_terminate();
	}
	v.resize(200, 9);
	if (v.size() != 200 || v[199] != 9)
	{
		::fast_io::fast_terminate();
	}
	v.resize(10);
	if (v.size() != 10 || v.front() != 0 || v.back() != 9)
	{
		::fast_io::fast_terminate();
	}
	V w{v};
	if (w != v)
	{
		::fast_io::fast_terminate();
	}
	V u{::fast_io::freestanding::from_range_t{}, v};
	if (u != v)
	{
		::fast_io::fast_terminate();
	}
	int arr[3]{1, 2, 3};
	u.assign_range(arr);
	if (u.size() != 3 || u[2] != 3)
	{
		::fast_io::fast_terminate();
	}
	v.shrink_to_fit();
	v.clear();
	if (!v.is_empty())
	{
		::fast_io::fast_terminate();
	}

	::fast_io::sized_vector_zu32<nontrivial> nt;
	nt.emplace_back(1);
	nt.emplace_back(2);
	nt.insert(nt.begin(), nontrivial{0});
	if (nt.size() != 3 || nt[1].x != 1)
	{
		::fast_io::fast_terminate();
	}
	nt.erase(nt.begin());
	if (nt[0].x != 1)
	{
		::fast_io::fast_terminate();
	}

	::fast_io::zu32_sized_vector_zu32 counts{1u, 2u, 3u};
	static_assert(::std::same_as<decltype(counts)::value_type, ::fast_io::size32_t>);
	if (counts.size() != 3 || counts[2] != 3)
	{
		::fast_io::fast_terminate();
	}

	::fast_io::sized_vector_zu32<double> dv{1.5, 2.5};
	if (dv.size() != 2)
	{
		::fast_io::fast_terminate();
	}
}
