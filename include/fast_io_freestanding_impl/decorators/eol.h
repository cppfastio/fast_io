#pragma once

namespace fast_io
{

namespace decorators
{

enum class eol_scheme
{
lf,
crlf,
cr,
lfcr,
nl,	/*EBCDIC*/
newline
};

template<eol_scheme fromscheme,
	eol_scheme toscheme,
	::std::integral chartype>
struct basic_eol_converter
{
	using input_char_type = chartype;
	using output_char_type = chartype;
	bool last_unfinished{};
	inline constexpr deco_result<output_char_type,input_char_type>
		process_chars(input_char_type const *fromfirst,input_char_type const *fromlast,
				output_char_type *tofirst,output_char_type *tolast) noexcept
	{
		if constexpr(fromscheme==eol_scheme::lf&&toscheme==eol_scheme::crlf)
		{
			if(last_unfinished)
			{
				if(tofirst==tolast)
				{
					return {fromfirst,tofirst};
				}
				*tofirst=char_literal_v<u8'\n',output_char_type>;
				++tofirst;
			}
			for(last_unfinished=false;fromfirst!=fromlast&&tofirst!=tolast;)
			{
				auto ch{*fromfirst};
				if(ch==char_literal_v<u8'\n',output_char_type>)
				{
					if(tofirst!=tolast)
					{
						*tofirst=char_literal_v<u8'\r',output_char_type>;
						++tofirst;
						if(tofirst==tolast)
						{
							last_unfinished=true;
							break;
						}
					}
				}
				*tofirst=ch;
				++fromfirst;
				++tofirst;
			}
			return {fromfirst,tofirst};
		}
		else if constexpr(fromscheme==eol_scheme::crlf&&toscheme==eol_scheme::lf)
		{
			if(last_unfinished)
			{
				if(tofirst==tolast||fromfirst==fromlast)
				{
					return {fromfirst,tofirst};
				}
				auto ch{*fromfirst};
				if(ch==char_literal_v<u8'\n',output_char_type>)
				{
					*tofirst=char_literal_v<u8'\n',output_char_type>;
					++tofirst;
					++fromfirst;
				}
				else
				{
					*tofirst=char_literal_v<u8'\r',output_char_type>;
					++tofirst;
				}
			}
			for(last_unfinished=false;fromfirst!=fromlast&&tofirst!=tolast;)
			{
				auto ch{*fromfirst};
				if(ch==char_literal_v<u8'\r',output_char_type>)
				{
					if(fromfirst+1==fromlast)
					{
						last_unfinished=true;
						return {fromlast,tofirst};
					}
					auto tch{fromfirst[1]};
					if(tch==char_literal_v<u8'\n',output_char_type>)
					{
						ch=tch;
						++fromfirst;
					}
				}
				*tofirst=ch;
				++fromfirst;
				++tofirst;
			}
			return {fromfirst,tofirst};
		}
	}
};	

template<eol_scheme internalscheme,eol_scheme externalscheme,::std::integral char_type>
using basic_eol = basic_bidirectional_decorator_adaptor<basic_eol_converter<externalscheme,internalscheme,char_type>,
		basic_eol_converter<internalscheme,externalscheme,char_type>>;

template<::std::integral char_type>
using basic_lf_crlf = basic_eol<eol_scheme::lf,eol_scheme::crlf,char_type>;
using lf_crlf = basic_lf_crlf<char>;
using wlf_crlf = basic_lf_crlf<wchar_t>;
using u8lf_crlf = basic_lf_crlf<char8_t>;
using u16lf_crlf = basic_lf_crlf<char16_t>;
using u32lf_crlf = basic_lf_crlf<char32_t>;

}

}
