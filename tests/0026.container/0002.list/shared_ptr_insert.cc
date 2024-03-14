#include<fast_io_dsal/list.h>
#include<fast_io.h>
#include<memory>

namespace fast_io::freestanding
{

template<typename T>
struct is_trivially_relocatable<::std::shared_ptr<T>>
{
	static inline constexpr bool value = true;
};

}

int main()
{
	::fast_io::list<::std::shared_ptr<int>> ilist{ std::make_shared<int>(1), std::make_shared<int>(2), std::make_shared<int>(3) };
	ilist.insert(ilist.cbegin(), std::make_shared<int>(4));
}
