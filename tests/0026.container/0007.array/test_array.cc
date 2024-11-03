#include <fast_io_dsal/array.h>
#include <fast_io.h>

int main()
{
	constexpr ::fast_io::array<::std::size_t, 4> arr1{1, 4, 6, 8};
	constexpr ::fast_io::array<::std::size_t, 7> arr2{1, 4, 6, 7, 10};
	constexpr ::fast_io::array arr3{static_cast<::std::uint_least32_t>(3), 5, 6, 7, 8, 10};
	for (auto const &e : arr1)
	{
		::fast_io::io::println(e);
	}
	for (auto const &e : arr2)
	{
		::fast_io::io::println(e);
	}
	::fast_io::io::println(arr1 <=> arr2);

	for (::std::size_t i{}; i != arr1.size(); ++i)
	{
		::fast_io::io::println(arr1[i]);
	}
	for (::std::size_t i{}; i != arr2.size(); ++i)
	{
		::fast_io::io::println(arr2[i]);
	}
	::fast_io::io::println(arr1 <=> arr2);

	for (auto const &e : arr3)
	{
		::fast_io::io::println(e);
	}
}
