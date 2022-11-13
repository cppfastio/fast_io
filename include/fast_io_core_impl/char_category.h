#pragma once

namespace fast_io::char_category
{

template<std::integral char_type>
inline constexpr bool is_c_alnum(char_type ch) noexcept
{
	if constexpr(std::same_as<char,char_type>)
switch(ch){case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':case 'G':case 'H':case 'I':case 'J':case 'K':case 'L':case 'M':case 'N':case 'O':case 'P':case 'Q':case 'R':case 'S':case 'T':case 'U':case 'V':case 'W':case 'X':case 'Y':case 'Z':case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':case 'g':case 'h':case 'i':case 'j':case 'k':case 'l':case 'm':case 'n':case 'o':case 'p':case 'q':case 'r':case 's':case 't':case 'u':case 'v':case 'w':case 'x':case 'y':case 'z':return true;default:return false;}
	else if constexpr(std::same_as<wchar_t,char_type>)
switch(ch){case L'0':case L'1':case L'2':case L'3':case L'4':case L'5':case L'6':case L'7':case L'8':case L'9':case L'A':case L'B':case L'C':case L'D':case L'E':case L'F':case L'G':case L'H':case L'I':case L'J':case L'K':case L'L':case L'M':case L'N':case L'O':case L'P':case L'Q':case L'R':case L'S':case L'T':case L'U':case L'V':case L'W':case L'X':case L'Y':case L'Z':case L'a':case L'b':case L'c':case L'd':case L'e':case L'f':case L'g':case L'h':case L'i':case L'j':case L'k':case L'l':case L'm':case L'n':case L'o':case L'p':case L'q':case L'r':case L's':case L't':case L'u':case L'v':case L'w':case L'x':case L'y':case L'z':return true;default:return false;}
	else
switch(ch){case u8'0':case u8'1':case u8'2':case u8'3':case u8'4':case u8'5':case u8'6':case u8'7':case u8'8':case u8'9':case u8'A':case u8'B':case u8'C':case u8'D':case u8'E':case u8'F':case u8'G':case u8'H':case u8'I':case u8'J':case u8'K':case u8'L':case u8'M':case u8'N':case u8'O':case u8'P':case u8'Q':case u8'R':case u8'S':case u8'T':case u8'U':case u8'V':case u8'W':case u8'X':case u8'Y':case u8'Z':case u8'a':case u8'b':case u8'c':case u8'd':case u8'e':case u8'f':case u8'g':case u8'h':case u8'i':case u8'j':case u8'k':case u8'l':case u8'm':case u8'n':case u8'o':case u8'p':case u8'q':case u8'r':case u8's':case u8't':case u8'u':case u8'v':case u8'w':case u8'x':case u8'y':case u8'z':return true;default:return false;}
}

template<std::integral char_type>
inline constexpr bool is_c_alpha(char_type ch) noexcept
{
	if constexpr(std::same_as<char,char_type>)
switch(ch){case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':case 'G':case 'H':case 'I':case 'J':case 'K':case 'L':case 'M':case 'N':case 'O':case 'P':case 'Q':case 'R':case 'S':case 'T':case 'U':case 'V':case 'W':case 'X':case 'Y':case 'Z':case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':case 'g':case 'h':case 'i':case 'j':case 'k':case 'l':case 'm':case 'n':case 'o':case 'p':case 'q':case 'r':case 's':case 't':case 'u':case 'v':case 'w':case 'x':case 'y':case 'z':return true;default:return false;}
	else if constexpr(std::same_as<wchar_t,char_type>)
switch(ch){case L'A':case L'B':case L'C':case L'D':case L'E':case L'F':case L'G':case L'H':case L'I':case L'J':case L'K':case L'L':case L'M':case L'N':case L'O':case L'P':case L'Q':case L'R':case L'S':case L'T':case L'U':case L'V':case L'W':case L'X':case L'Y':case L'Z':case L'a':case L'b':case L'c':case L'd':case L'e':case L'f':case L'g':case L'h':case L'i':case L'j':case L'k':case L'l':case L'm':case L'n':case L'o':case L'p':case L'q':case L'r':case L's':case L't':case L'u':case L'v':case L'w':case L'x':case L'y':case L'z':return true;default:return false;}
	else
switch(ch){case u8'A':case u8'B':case u8'C':case u8'D':case u8'E':case u8'F':case u8'G':case u8'H':case u8'I':case u8'J':case u8'K':case u8'L':case u8'M':case u8'N':case u8'O':case u8'P':case u8'Q':case u8'R':case u8'S':case u8'T':case u8'U':case u8'V':case u8'W':case u8'X':case u8'Y':case u8'Z':case u8'a':case u8'b':case u8'c':case u8'd':case u8'e':case u8'f':case u8'g':case u8'h':case u8'i':case u8'j':case u8'k':case u8'l':case u8'm':case u8'n':case u8'o':case u8'p':case u8'q':case u8'r':case u8's':case u8't':case u8'u':case u8'v':case u8'w':case u8'x':case u8'y':case u8'z':return true;default:return false;}
}

template<std::integral char_type>
inline constexpr bool is_c_lower(char_type ch) noexcept
{
	if constexpr(std::same_as<char,char_type>)
switch(ch){case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':case 'g':case 'h':case 'i':case 'j':case 'k':case 'l':case 'm':case 'n':case 'o':case 'p':case 'q':case 'r':case 's':case 't':case 'u':case 'v':case 'w':case 'x':case 'y':case 'z':return true;default:return false;}
	else if constexpr(std::same_as<wchar_t,char_type>)
switch(ch){case L'a':case L'b':case L'c':case L'd':case L'e':case L'f':case L'g':case L'h':case L'i':case L'j':case L'k':case L'l':case L'm':case L'n':case L'o':case L'p':case L'q':case L'r':case L's':case L't':case L'u':case L'v':case L'w':case L'x':case L'y':case L'z':return true;default:return false;}
	else
switch(ch){case u8'a':case u8'b':case u8'c':case u8'd':case u8'e':case u8'f':case u8'g':case u8'h':case u8'i':case u8'j':case u8'k':case u8'l':case u8'm':case u8'n':case u8'o':case u8'p':case u8'q':case u8'r':case u8's':case u8't':case u8'u':case u8'v':case u8'w':case u8'x':case u8'y':case u8'z':return true;default:return false;}
}

template<std::integral char_type>
inline constexpr bool is_c_upper(char_type ch) noexcept
{
	if constexpr(std::same_as<char,char_type>)
switch(ch){case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':case 'G':case 'H':case 'I':case 'J':case 'K':case 'L':case 'M':case 'N':case 'O':case 'P':case 'Q':case 'R':case 'S':case 'T':case 'U':case 'V':case 'W':case 'X':case 'Y':case 'Z':return true;default:return false;}
	else if constexpr(std::same_as<wchar_t,char_type>)
switch(ch){case L'A':case L'B':case L'C':case L'D':case L'E':case L'F':case L'G':case L'H':case L'I':case L'J':case L'K':case L'L':case L'M':case L'N':case L'O':case L'P':case L'Q':case L'R':case L'S':case L'T':case L'U':case L'V':case L'W':case L'X':case L'Y':case L'Z':return true;default:return false;}
	else
switch(ch){case u8'A':case u8'B':case u8'C':case u8'D':case u8'E':case u8'F':case u8'G':case u8'H':case u8'I':case u8'J':case u8'K':case u8'L':case u8'M':case u8'N':case u8'O':case u8'P':case u8'Q':case u8'R':case u8'S':case u8'T':case u8'U':case u8'V':case u8'W':case u8'X':case u8'Y':case u8'Z':return true;default:return false;}
}

template<std::integral char_type>
inline constexpr bool is_c_digit(char_type ch) noexcept
{
	if constexpr(std::same_as<char,char_type>)
switch(ch){case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':return true;default:return false;}
	else if constexpr(std::same_as<wchar_t,char_type>)
switch(ch){case L'0':case L'1':case L'2':case L'3':case L'4':case L'5':case L'6':case L'7':case L'8':case L'9':return true;default:return false;}
	else
switch(ch){case u8'0':case u8'1':case u8'2':case u8'3':case u8'4':case u8'5':case u8'6':case u8'7':case u8'8':case u8'9':return true;default:return false;}
}

template<std::integral char_type>
inline constexpr bool is_c_xdigit(char_type ch) noexcept
{
	if constexpr(std::same_as<char,char_type>)
switch(ch){case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':return true;default:return false;}
	else if constexpr(std::same_as<wchar_t,char_type>)
switch(ch){case L'0':case L'1':case L'2':case L'3':case L'4':case L'5':case L'6':case L'7':case L'8':case L'9':case L'a':case L'b':case L'c':case L'd':case L'e':case L'f':case L'A':case L'B':case L'C':case L'D':case L'E':case L'F':return true;default:return false;}
	else
switch(ch){case u8'0':case u8'1':case u8'2':case u8'3':case u8'4':case u8'5':case u8'6':case u8'7':case u8'8':case u8'9':case u8'a':case u8'b':case u8'c':case u8'd':case u8'e':case u8'f':case u8'A':case u8'B':case u8'C':case u8'D':case u8'E':case u8'F':return true;default:return false;}
}

template<std::integral char_type>
inline constexpr bool is_c_punct(char_type ch) noexcept
{
	if constexpr(std::same_as<char,char_type>)
switch(ch){case '!':case '\"':case '#':case '$':case '%':case '&':case '\'':case '(':case ')':case '*':case '+':case ',':case '-':case '.':case '/':case ':':case ';':case '<':case '=':case '>':case '?':case '@':case '[':case '\\':case ']':case '^':case '_':case '`':case '{':case '|':case '}':case '~':return true;default:return false;}
	else if constexpr(std::same_as<wchar_t,char_type>)
switch(ch){case L'!':case L'\"':case L'#':case L'$':case L'%':case L'&':case L'\'':case L'(':case L')':case L'*':case L'+':case L',':case L'-':case L'.':case L'/':case L':':case L';':case L'<':case L'=':case L'>':case L'?':case L'@':case L'[':case L'\\':case L']':case L'^':case L'_':case L'`':case L'{':case L'|':case L'}':case L'~':return true;default:return false;}
	else
switch(ch){case u8'!':case u8'\"':case u8'#':case u8'$':case u8'%':case u8'&':case u8'\'':case u8'(':case u8')':case u8'*':case u8'+':case u8',':case u8'-':case u8'.':case u8'/':case u8':':case u8';':case u8'<':case u8'=':case u8'>':case u8'?':case u8'@':case u8'[':case u8'\\':case u8']':case u8'^':case u8'_':case u8'`':case u8'{':case u8'|':case u8'}':case u8'~':return true;default:return false;}
}

template<std::integral char_type>
inline constexpr bool is_c_graph(char_type ch) noexcept
{
	if constexpr(std::same_as<char,char_type>)
switch(ch){case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':case 'G':case 'H':case 'I':case 'J':case 'K':case 'L':case 'M':case 'N':case 'O':case 'P':case 'Q':case 'R':case 'S':case 'T':case 'U':case 'V':case 'W':case 'X':case 'Y':case 'Z':case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':case 'g':case 'h':case 'i':case 'j':case 'k':case 'l':case 'm':case 'n':case 'o':case 'p':case 'q':case 'r':case 's':case 't':case 'u':case 'v':case 'w':case 'x':case 'y':case 'z':case '!':case '\"':case '#':case '$':case '%':case '&':case '\'':case '(':case ')':case '*':case '+':case ',':case '-':case '.':case '/':case ':':case ';':case '<':case '=':case '>':case '?':case '@':case '[':case '\\':case ']':case '^':case '_':case '`':case '{':case '|':case '}':case '~':return true;default:return false;}
	else if constexpr(std::same_as<wchar_t,char_type>)
switch(ch){case L'0':case L'1':case L'2':case L'3':case L'4':case L'5':case L'6':case L'7':case L'8':case L'9':case L'A':case L'B':case L'C':case L'D':case L'E':case L'F':case L'G':case L'H':case L'I':case L'J':case L'K':case L'L':case L'M':case L'N':case L'O':case L'P':case L'Q':case L'R':case L'S':case L'T':case L'U':case L'V':case L'W':case L'X':case L'Y':case L'Z':case L'a':case L'b':case L'c':case L'd':case L'e':case L'f':case L'g':case L'h':case L'i':case L'j':case L'k':case L'l':case L'm':case L'n':case L'o':case L'p':case L'q':case L'r':case L's':case L't':case L'u':case L'v':case L'w':case L'x':case L'y':case L'z':case L'!':case L'\"':case L'#':case L'$':case L'%':case L'&':case L'\'':case L'(':case L')':case L'*':case L'+':case L',':case L'-':case L'.':case L'/':case L':':case L';':case L'<':case L'=':case L'>':case L'?':case L'@':case L'[':case L'\\':case L']':case L'^':case L'_':case L'`':case L'{':case L'|':case L'}':case L'~':return true;default:return false;}
	else
switch(ch){case u8'0':case u8'1':case u8'2':case u8'3':case u8'4':case u8'5':case u8'6':case u8'7':case u8'8':case u8'9':case u8'A':case u8'B':case u8'C':case u8'D':case u8'E':case u8'F':case u8'G':case u8'H':case u8'I':case u8'J':case u8'K':case u8'L':case u8'M':case u8'N':case u8'O':case u8'P':case u8'Q':case u8'R':case u8'S':case u8'T':case u8'U':case u8'V':case u8'W':case u8'X':case u8'Y':case u8'Z':case u8'a':case u8'b':case u8'c':case u8'd':case u8'e':case u8'f':case u8'g':case u8'h':case u8'i':case u8'j':case u8'k':case u8'l':case u8'm':case u8'n':case u8'o':case u8'p':case u8'q':case u8'r':case u8's':case u8't':case u8'u':case u8'v':case u8'w':case u8'x':case u8'y':case u8'z':case u8'!':case u8'\"':case u8'#':case u8'$':case u8'%':case u8'&':case u8'\'':case u8'(':case u8')':case u8'*':case u8'+':case u8',':case u8'-':case u8'.':case u8'/':case u8':':case u8';':case u8'<':case u8'=':case u8'>':case u8'?':case u8'@':case u8'[':case u8'\\':case u8']':case u8'^':case u8'_':case u8'`':case u8'{':case u8'|':case u8'}':case u8'~':return true;default:return false;}
}

template<std::integral char_type>
inline constexpr bool is_c_print(char_type ch) noexcept
{
	if constexpr(std::same_as<char,char_type>)
switch(ch){case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':case 'G':case 'H':case 'I':case 'J':case 'K':case 'L':case 'M':case 'N':case 'O':case 'P':case 'Q':case 'R':case 'S':case 'T':case 'U':case 'V':case 'W':case 'X':case 'Y':case 'Z':case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':case 'g':case 'h':case 'i':case 'j':case 'k':case 'l':case 'm':case 'n':case 'o':case 'p':case 'q':case 'r':case 's':case 't':case 'u':case 'v':case 'w':case 'x':case 'y':case 'z':case '!':case '\"':case '#':case '$':case '%':case '&':case '\'':case '(':case ')':case '*':case '+':case ',':case '-':case '.':case '/':case ':':case ';':case '<':case '=':case '>':case '?':case '@':case '[':case '\\':case ']':case '^':case '_':case '`':case '{':case '|':case '}':case '~':case ' ':return true;default:return false;}
	else if constexpr(std::same_as<wchar_t,char_type>)
switch(ch){case L'0':case L'1':case L'2':case L'3':case L'4':case L'5':case L'6':case L'7':case L'8':case L'9':case L'A':case L'B':case L'C':case L'D':case L'E':case L'F':case L'G':case L'H':case L'I':case L'J':case L'K':case L'L':case L'M':case L'N':case L'O':case L'P':case L'Q':case L'R':case L'S':case L'T':case L'U':case L'V':case L'W':case L'X':case L'Y':case L'Z':case L'a':case L'b':case L'c':case L'd':case L'e':case L'f':case L'g':case L'h':case L'i':case L'j':case L'k':case L'l':case L'm':case L'n':case L'o':case L'p':case L'q':case L'r':case L's':case L't':case L'u':case L'v':case L'w':case L'x':case L'y':case L'z':case L'!':case L'\"':case L'#':case L'$':case L'%':case L'&':case L'\'':case L'(':case L')':case L'*':case L'+':case L',':case L'-':case L'.':case L'/':case L':':case L';':case L'<':case L'=':case L'>':case L'?':case L'@':case L'[':case L'\\':case L']':case L'^':case L'_':case L'`':case L'{':case L'|':case L'}':case L'~':case L' ':return true;default:return false;}
	else
switch(ch){case u8'0':case u8'1':case u8'2':case u8'3':case u8'4':case u8'5':case u8'6':case u8'7':case u8'8':case u8'9':case u8'A':case u8'B':case u8'C':case u8'D':case u8'E':case u8'F':case u8'G':case u8'H':case u8'I':case u8'J':case u8'K':case u8'L':case u8'M':case u8'N':case u8'O':case u8'P':case u8'Q':case u8'R':case u8'S':case u8'T':case u8'U':case u8'V':case u8'W':case u8'X':case u8'Y':case u8'Z':case u8'a':case u8'b':case u8'c':case u8'd':case u8'e':case u8'f':case u8'g':case u8'h':case u8'i':case u8'j':case u8'k':case u8'l':case u8'm':case u8'n':case u8'o':case u8'p':case u8'q':case u8'r':case u8's':case u8't':case u8'u':case u8'v':case u8'w':case u8'x':case u8'y':case u8'z':case u8'!':case u8'\"':case u8'#':case u8'$':case u8'%':case u8'&':case u8'\'':case u8'(':case u8')':case u8'*':case u8'+':case u8',':case u8'-':case u8'.':case u8'/':case u8':':case u8';':case u8'<':case u8'=':case u8'>':case u8'?':case u8'@':case u8'[':case u8'\\':case u8']':case u8'^':case u8'_':case u8'`':case u8'{':case u8'|':case u8'}':case u8'~':case u8' ':return true;default:return false;}
}

namespace details
{
inline constexpr char32_t to_c_upper_ascii_impl(char32_t ch) noexcept
{
	constexpr char32_t alphanum{static_cast<char32_t>(26u)};
	char32_t res{ch-U'a'};
	if(res<alphanum)
		return res+U'A';
	return ch;
}

inline constexpr char32_t to_c_lower_ascii_impl(char32_t ch) noexcept
{
	constexpr char32_t alphanum{static_cast<char32_t>(26u)};
	char32_t res{ch-U'A'};
	if(res<alphanum)
		return res+U'a';
	return ch;
}

}

template<std::integral char_type>
inline constexpr char_type to_c_lower(char_type ch) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	if constexpr(!::fast_io::details::is_ebcdic<char_type>)
		return static_cast<char_type>(static_cast<unsigned_char_type>(details::to_c_lower_ascii_impl(static_cast<unsigned_char_type>(ch))));
	else if constexpr(std::same_as<char,char_type>)
switch(ch){case 'A':return 'a';case 'B':return 'b';case 'C':return 'c';case 'D':return 'd';case 'E':return 'e';case 'F':return 'f';case 'G':return 'g';case 'H':return 'h';case 'I':return 'i';case 'J':return 'j';case 'K':return 'k';case 'L':return 'l';case 'M':return 'm';case 'N':return 'n';case 'O':return 'o';case 'P':return 'p';case 'Q':return 'q';case 'R':return 'r';case 'S':return 's';case 'T':return 't';case 'U':return 'u';case 'V':return 'v';case 'W':return 'w';case 'X':return 'x';case 'Y':return 'y';case 'Z':return 'z';default:return ch;}
	else if constexpr(std::same_as<wchar_t,char_type>)
switch(ch){case L'A':return L'a';case L'B':return L'b';case L'C':return L'c';case L'D':return L'd';case L'E':return L'e';case L'F':return L'f';case L'G':return L'g';case L'H':return L'h';case L'I':return L'i';case L'J':return L'j';case L'K':return L'k';case L'L':return L'l';case L'M':return L'm';case L'N':return L'n';case L'O':return L'o';case L'P':return L'p';case L'Q':return L'q';case L'R':return L'r';case L'S':return L's';case L'T':return L't';case L'U':return L'u';case L'V':return L'v';case L'W':return L'w';case L'X':return L'x';case L'Y':return L'y';case L'Z':return L'z';default:return ch;}
}

template<std::integral char_type>
inline constexpr char_type to_c_upper(char_type ch) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	if constexpr(!::fast_io::details::is_ebcdic<char_type>)
		return static_cast<char_type>(static_cast<unsigned_char_type>(details::to_c_upper_ascii_impl(static_cast<unsigned_char_type>(ch))));
	else if constexpr(std::same_as<char,char_type>)
switch(ch){case 'a':return 'A';case 'b':return 'B';case 'c':return 'C';case 'd':return 'D';case 'e':return 'E';case 'f':return 'F';case 'g':return 'G';case 'h':return 'H';case 'i':return 'I';case 'j':return 'J';case 'k':return 'K';case 'l':return 'L';case 'm':return 'M';case 'n':return 'N';case 'o':return 'O';case 'p':return 'P';case 'q':return 'Q';case 'r':return 'R';case 's':return 'S';case 't':return 'T';case 'u':return 'U';case 'v':return 'V';case 'w':return 'W';case 'x':return 'X';case 'y':return 'Y';case 'z':return 'Z';default:return ch;}
	else if constexpr(std::same_as<wchar_t,char_type>)
switch(ch){case L'a':return L'A';case L'b':return L'B';case L'c':return L'C';case L'd':return L'D';case L'e':return L'E';case L'f':return L'F';case L'g':return L'G';case L'h':return L'H';case L'i':return L'I';case L'j':return L'J';case L'k':return L'K';case L'l':return L'L';case L'm':return L'M';case L'n':return L'N';case L'o':return L'O';case L'p':return L'P';case L'q':return L'Q';case L'r':return L'R';case L's':return L'S';case L't':return L'T';case L'u':return L'U';case L'v':return L'V';case L'w':return L'W';case L'x':return L'X';case L'y':return L'Y';case L'z':return L'Z';default:return ch;}
}

/*
All Ascii based charset, only 6 character is supported
space (0x20, ' ')
form feed (0x0c, '\f')
line feed (0x0a, '\n')
carriage return (0x0d, '\r')
horizontal tab (0x09, '\t')
vertical tab (0x0b, '\v')

Any other exec-charset, besides these 6 characters, they can optionally support others, but too many are not allowed.

For EBCDIC NL should also get supported.
ASCII: space (0x20, ' '), EBCDIC:64
ASCII: form feed (0x0c, '\f'), EBCDIC:12
ASCII: line feed (0x0a, '\n'), EBCDIC:37
ASCII: carriage return (0x0d, '\r'), EBCDIC:13
ASCII: horizontal tab (0x09, '\t'), EBCDIC:5
ASCII: vertical tab (0x0b, '\v'), EBCDIC:11
EBCDIC specific: NL:21
*/

namespace details
{
inline constexpr bool is_c_space_wide_impl(wchar_t ch) noexcept
{
	switch(ch)
	{
		case L'\f':case L'\n':case L'\r':
		case L'\t':case L'\v':case L' ':return true;
		default:
		return false;
	};
}

template<bool use_ebcdic,std::integral char_type>
inline constexpr bool is_c_space_impl(char_type ch) noexcept
{
	if constexpr(use_ebcdic)
	{
		switch(ch)
		{
			case 5:case 11:case 12:case 13:
			case 21:case 37:case 64:return true;
			default:
			return false;
		};
	}
	else
	{
		if constexpr(std::unsigned_integral<std::remove_cvref_t<char_type>>)
		{
			using unsigned_t = char_type;
			return (ch==0x20)|(static_cast<unsigned_t>(ch-0x9)<static_cast<unsigned_t>(0x5));
		}
		else
		{
			using unsigned_t = std::make_unsigned_t<char_type>;
			unsigned_t const e(ch);
			return (e==0x20)|(static_cast<unsigned_t>(e-0x9)<static_cast<unsigned_t>(0x5));
		}
	}
}

template<bool ebcdic,std::integral char_type>
inline constexpr auto is_c_space_tb_impl() noexcept
{
	::fast_io::freestanding::array<bool,256u> tb;
	for(std::size_t i{};i!=tb.size();++i)
		tb[i]=is_c_space_impl<ebcdic>(static_cast<char_type>(i));
	return tb;
}

template<bool ebcdic,std::integral char_type>
requires (sizeof(char_type)==1)
inline constexpr auto is_c_space_tb{is_c_space_tb_impl<ebcdic,char_type>()};
}

template<std::integral char_type>
inline constexpr bool is_c_space(char_type ch) noexcept
{
	constexpr bool optimize_with_spacetb{sizeof(char_type)==1&&std::numeric_limits<std::uint_least8_t>::digits==8};
	if constexpr(optimize_with_spacetb)
	{
		if constexpr(::fast_io::details::is_ebcdic<char_type>)
			return details::is_c_space_tb<true,char8_t>[static_cast<std::make_unsigned_t<char_type>>(ch)];
		else
			return details::is_c_space_tb<false,char8_t>[static_cast<std::make_unsigned_t<char_type>>(ch)];
	}
	else
	{
	if constexpr(::fast_io::details::is_ebcdic<char_type>)
		return details::is_c_space_impl<true>(ch);
	else if constexpr(::std::same_as<char_type,wchar_t>&&::fast_io::details::wide_is_none_utf_endian)
		return details::is_c_space_wide_impl(ch);
	else
		return details::is_c_space_impl<false>(static_cast<char32_t>(static_cast<std::make_unsigned_t<char_type>>(ch)));
	}
}

/*
https://www.gnu.org/software/gcc/gcc-11/changes.html


A series of conditional expressions that compare the same variable can be transformed into a switch statement if each of them contains a comparison expression. Example:
        int IsHTMLWhitespace(int aChar) {
          return aChar == 0x0009 || aChar == 0x000A ||
                 aChar == 0x000C || aChar == 0x000D ||
                 aChar == 0x0020;
        }
Let's just add this into this library

*/

namespace details
{
template<std::unsigned_integral char_type>
inline constexpr bool is_html_whitespace_ascii_impl(char_type ch) noexcept
{
	switch(ch)
	{
	case 0x0009:case 0x000A:case 0x000C:case 0x000D:case 0x0020:return true;
	default:return false;
	}
}

/*
For EBCDIC NL should also get supported.
ASCII: space (0x20, ' '), EBCDIC:64
ASCII: form feed (0x0c, '\f'), EBCDIC:12
ASCII: line feed (0x0a, '\n'), EBCDIC:37
ASCII: carriage return (0x0d, '\r'), EBCDIC:13
ASCII: horizontal tab (0x09, '\t'), EBCDIC:5
EBCDIC specific: NL:21
*/
inline constexpr bool is_html_whitespace_ebcdic_impl(char32_t ch) noexcept
{
	switch(ch)
	{
		case 5:case 12:case 13:
		case 21:case 37:case 64:return true;
		default:
		return false;
	};
}

inline constexpr bool is_html_whitespace_wide_impl(wchar_t ch) noexcept
{
	switch(ch)
	{
		case L'\f':case L'\n':case L'\r':
		case L'\t':case L' ':return true;
		default:
		return false;
	};
}

}

template<std::integral char_type>
inline constexpr bool is_html_whitespace(char_type ch) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	if constexpr(::fast_io::details::is_ebcdic<char_type>)
	{
		return details::is_html_whitespace_ebcdic_impl(static_cast<unsigned_char_type>(ch));
	}
	else
	{
		if constexpr(sizeof(char_type)<=sizeof(char32_t))
		{
			return details::is_html_whitespace_ascii_impl(static_cast<char32_t>(static_cast<unsigned_char_type>(ch)));
		}
		else
		{
			return details::is_html_whitespace_ascii_impl(static_cast<unsigned_char_type>(ch));
		}
	}
}

template<std::integral char_type>
inline constexpr bool is_c_halfwidth(char_type ch) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	if constexpr(sizeof(char_type)<sizeof(char32_t))
	{
		return ch;
	}
	else if constexpr(!std::same_as<char_type,char32_t>&&sizeof(char_type)==sizeof(char32_t))
	{
		return is_c_halfwidth(static_cast<char32_t>(ch));
	}
	else if constexpr(std::signed_integral<char_type>)
	{
		return is_c_halfwidth(static_cast<unsigned_char_type>(ch));
	}
	else if constexpr(::std::same_as<char_type,wchar_t>&&::fast_io::details::wide_is_none_utf_endian)
	{
		constexpr unsigned_char_type halfwidth_exclaimation_mark_val{u8'!'};
		constexpr unsigned_char_type num{94};
		unsigned_char_type cht{ch};
		cht=::fast_io::byte_swap(cht);
		return static_cast<unsigned_char_type>(cht-halfwidth_exclaimation_mark_val)<num;
	}
	else
	{
		constexpr unsigned_char_type halfwidth_exclaimation_mark_val{u8'!'};
		constexpr unsigned_char_type num{94};
		return static_cast<unsigned_char_type>(ch-halfwidth_exclaimation_mark_val)<num;
	}
}

template<std::integral char_type>
inline constexpr bool is_c_fullwidth(char_type ch) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	if constexpr(sizeof(char_type)<sizeof(char32_t))
	{
		return ch;
	}
	else if constexpr(!std::same_as<char_type,char32_t>&&sizeof(char_type)==sizeof(char32_t))
	{
		return is_c_fullwidth(static_cast<char32_t>(ch));
	}
	else if constexpr(std::signed_integral<char_type>)
	{
		return is_c_fullwidth(static_cast<unsigned_char_type>(ch));
	}
	else if constexpr(::std::same_as<char_type,wchar_t>&&::fast_io::details::wide_is_none_utf_endian)
	{
		constexpr unsigned_char_type halfwidth_exclaimation_mark_val{0xFF01};
		constexpr unsigned_char_type num{94};
		unsigned_char_type cht{ch};
		cht=::fast_io::byte_swap(cht);
		return static_cast<unsigned_char_type>(cht-halfwidth_exclaimation_mark_val)<num;
	}
	else
	{
		constexpr unsigned_char_type fullwidth_exclaimation_mark_val{0xFF01};
		constexpr unsigned_char_type num{94};
		return static_cast<unsigned_char_type>(ch-fullwidth_exclaimation_mark_val)<num;
	}
}


/*
To do: to_c_fullwidth
*/

template<std::integral char_type>
inline constexpr char_type to_c_halfwidth(char_type ch) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	if constexpr(sizeof(char_type)<sizeof(char32_t))
	{
		return ch;
	}
	else if constexpr(!std::same_as<char_type,char32_t>&&sizeof(char_type)==sizeof(char32_t))
	{
		return static_cast<char_type>(to_c_halfwidth(static_cast<char32_t>(ch)));
	}
	else if constexpr(std::signed_integral<char_type>)
	{
		return static_cast<char_type>(to_c_halfwidth(static_cast<unsigned_char_type>(ch)));
	}
	else if constexpr(::std::same_as<char_type,wchar_t>&&::fast_io::details::wide_is_none_utf_endian)
	{
		constexpr unsigned_char_type fullwidth_exclaimation_mark_val{0xFF01};
		constexpr unsigned_char_type num{94};
		constexpr unsigned_char_type halfwidth_exclaimation_mark_val{u8'!'};
		unsigned_char_type cht{ch};
		cht=::fast_io::byte_swap(cht);
		unsigned_char_type const umav{static_cast<unsigned_char_type>(cht-fullwidth_exclaimation_mark_val)};
		if(umav<num)
		{
			return static_cast<unsigned_char_type>(umav+halfwidth_exclaimation_mark_val);
		}
		return cht;
	}
	else
	{
		constexpr unsigned_char_type fullwidth_exclaimation_mark_val{0xFF01};
		constexpr unsigned_char_type num{94};
		constexpr unsigned_char_type halfwidth_exclaimation_mark_val{u8'!'};
		unsigned_char_type const umav{static_cast<unsigned_char_type>(ch-fullwidth_exclaimation_mark_val)};
		if(umav<num)
		{
			return static_cast<unsigned_char_type>(umav+halfwidth_exclaimation_mark_val);
		}
		return ch;
	}
}

}

namespace fast_io
{
namespace details
{
template<std::integral char_type>
inline constexpr char_type const* find_lf_simd_impl(char_type const*,char_type const*) noexcept;

template<bool ishtml,bool findnot,::std::forward_iterator Iter>
requires (::std::integral<::std::iter_value_t<Iter>>)
inline constexpr Iter find_space_impl(Iter,Iter);
}

template<::std::forward_iterator Iter>
requires (::std::integral<::std::iter_value_t<Iter>>)
inline constexpr Iter find_lf(Iter first, Iter last)
{
	using char_type = ::std::iter_value_t<Iter>;
	if constexpr(::std::contiguous_iterator<Iter>)
	{
		return ::fast_io::details::find_lf_simd_impl(::std::to_address(first),::std::to_address(last))-::std::to_address(first)+first;
	}
	else
	{
		return ::fast_io::freestanding::find(first,last,::fast_io::char_literal_v<u8'\n',char_type>);
	}
}

template<::std::forward_iterator Iter>
requires (::std::integral<::std::iter_value_t<Iter>>)
inline constexpr Iter find_none_c_space(Iter begin,Iter end)
{
	return ::fast_io::details::find_space_impl<false,true>(begin,end);
}

template<::std::forward_iterator Iter>
requires (::std::integral<::std::iter_value_t<Iter>>)
inline constexpr Iter find_c_space(Iter begin,Iter end)
{
	return ::fast_io::details::find_space_impl<false,false>(begin,end);
}

template<::std::forward_iterator Iter>
requires (::std::integral<::std::iter_value_t<Iter>>)
inline constexpr Iter find_none_html_whitespace(Iter begin,Iter end)
{
	return ::fast_io::details::find_space_impl<true,true>(begin,end);
}

template<::std::forward_iterator Iter>
requires (::std::integral<::std::iter_value_t<Iter>>)
inline constexpr Iter find_html_whitespace(Iter begin,Iter end)
{
	return ::fast_io::details::find_space_impl<true,false>(begin,end);
}

}
