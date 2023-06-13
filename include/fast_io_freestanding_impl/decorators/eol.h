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
	eol_scheme toscheme>
struct basic_eol_converter
{
	bool last_unfinished{};
	template<::std::integral char_type>
	inline constexpr deco_result<char_type,char_type>
		process_chars(char_type const *fromfirst,char_type const *fromlast,
				char_type *tofirst,char_type *tolast) noexcept
	{
		if constexpr(fromscheme==eol_scheme::lf&&toscheme==eol_scheme::crlf)
		{
			if(last_unfinished)
			{
				if(tofirst==tolast)
				{
					return {fromfirst,tofirst};
				}
				*tofirst=char_literal_v<u8'\n',char_type>;
				++tofirst;
			}
			for(last_unfinished=false;fromfirst!=fromlast&&tofirst!=tolast;)
			{
				auto ch{*fromfirst};
				if(ch==char_literal_v<u8'\n',char_type>)
				{
					if(tofirst!=tolast)
					{
						*tofirst=char_literal_v<u8'\r',char_type>;
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
				if(ch==char_literal_v<u8'\n',char_type>)
				{
					*tofirst=char_literal_v<u8'\n',char_type>;
					++tofirst;
					++fromfirst;
				}
				else
				{
					*tofirst=char_literal_v<u8'\r',char_type>;
					++tofirst;
				}
			}
			for(last_unfinished=false;fromfirst!=fromlast&&tofirst!=tolast;)
			{
				auto ch{*fromfirst};
				if(ch==char_literal_v<u8'\r',char_type>)
				{
					if(fromfirst+1==fromlast)
					{
						last_unfinished=true;
						return {fromlast,tofirst};
					}
					auto tch{fromfirst[1]};
					if(tch==char_literal_v<u8'\n',char_type>)
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
		else /*For debugging purposes*/
		{
			for(;fromfirst!=fromlast&&tofirst!=tolast;)
			{
				*tofirst=*fromfirst;
				++fromfirst;
				++tofirst;
			}
			return {fromfirst,tofirst};
		}
	}
};	

template<eol_scheme internalscheme,eol_scheme externalscheme>
using basic_eol = basic_bidirectional_decorator_adaptor<basic_eol_converter<externalscheme,internalscheme>,
		basic_eol_converter<internalscheme,externalscheme>>;


using lf_crlf = basic_eol<eol_scheme::lf,eol_scheme::crlf>;
using crlf_lf = basic_eol<eol_scheme::crlf,eol_scheme::lf>;

}

}
