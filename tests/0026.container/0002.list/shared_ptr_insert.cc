#include <fast_io_dsal/list.h>
#include <fast_io.h>
#include <memory>

namespace fast_io::freestanding
{

template <typename T>
struct is_trivially_relocatable<::std::shared_ptr<T>>
{
	static inline constexpr bool value = true;
};

} // namespace fast_io::freestanding

int main()
{
	::fast_io::list<::std::shared_ptr<int>> lst{std::make_shared<int>(1), std::make_shared<int>(2), std::make_shared<int>(3)};
	lst.insert(lst.cbegin(), std::make_shared<int>(4));
	for (auto const &e : lst)
	{
		println(::fast_io::mnp::pointervw(e.get()), " use_count: ", e.use_count());
	}
}
