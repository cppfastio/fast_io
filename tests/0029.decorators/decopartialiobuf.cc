#include<fast_io.h>

struct decofiltest
{
using input_char_type = char;
using output_char_type = char;
static inline constexpr ::std::size_t remained_max = 5u;
static inline constexpr ::std::size_t output_final_max = 5u;
static inline constexpr ::fast_io::deco_result<char,char> process_chars(input_char_type const *fromfirst,input_char_type const *fromlast,
				output_char_type *tofirst,output_char_type *tolast) noexcept
{
	::std::size_t diff{static_cast<::std::size_t>(fromlast-fromfirst)};
	::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
	if(todiff<diff)
	{
		diff=todiff;
	}
	::fast_io::details::non_overlapped_copy_n(fromfirst,diff,tofirst);
	return {fromfirst+diff,tofirst+diff};
}
};
using decofilttest_decorator = ::fast_io::deco_partial_adapter<decofiltest>;
using bidecorator = ::fast_io::basic_bidirectional_decorator_adaptor<decofilttest_decorator,decofilttest_decorator>;

int main()
{
	constexpr std::size_t N(10000000);
	::fast_io::iobuf_io_file obf(::fast_io::io_cookie_type<::fast_io::native_file>,u8"io_file_deco.txt",::fast_io::open_mode::out);

	::fast_io::operations::add_io_decos(obf,bidecorator{});
	for(std::size_t i{};i!=N;++i)
		::fast_io::io::println(obf,i);
}
