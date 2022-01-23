	if constexpr(std::same_as<char_type,char>)
	{
		return tsc("Unknown");
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		return tsc(L"Unknown");
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		return tsc(u"Unknown");
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		return tsc(U"Unknown");
	}
	else
	{
		return tsc(u8"Unknown");
	}