#include<string>
#include<string_view>
#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	std::u16string u16str=fast_io::u16concat(u"The fart is ",18);
	char16_t const* cstylestr{u" years old hot girl😍.\n"};
	constexpr std::u8string_view simp{u8"Don't be a simp! Be a chad dude.\n"};
	using namespace fast_io::mnp;
	std::u32string u32str=fast_io::u32concat(
		code_cvt(u16str),code_cvt_os_c_str(cstylestr),U"Leroy!!!");
	print(code_cvt(u32str)," Jenkins!!!\n",fast_io::mnp::code_cvt(simp));
}

