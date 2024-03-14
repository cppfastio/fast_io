#include<fast_io_dsal/vector.h>
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
	::fast_io::vector<::std::shared_ptr<int>> vec{ std::make_shared<int>(1), std::make_shared<int>(2), std::make_shared<int>(3) };
	vec.insert(vec.begin()+1, std::make_shared<int>(4));
}
