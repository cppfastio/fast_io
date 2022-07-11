#include<string>
#include<fast_io.h>
#include<random>
#include<array>
#include<string_view>

int main()
{
	constexpr std::array hiragana{
	u8"あ",u8"い",u8"う",u8"え",u8"お",
	u8"か",u8"き",u8"く",u8"け",u8"こ",
	u8"さ",u8"し",u8"す",u8"せ",u8"そ",
	u8"た",u8"ち",u8"つ",u8"て",u8"と",
	u8"な",u8"に",u8"ぬ",u8"ね",u8"の",
	u8"は",u8"ひ",u8"ふ",u8"へ",u8"ほ",
	u8"ま",u8"み",u8"む",u8"め",u8"も",
	u8"や",	u8"ゆ",	u8"よ",
	u8"ら",u8"り",u8"る",u8"れ",u8"ろ",
	u8"わ",			u8"を",
	u8"ん"
	};
	constexpr std::array katakana{
	u8"ア",u8"イ",u8"ウ",u8"エ",u8"オ",
	u8"カ",u8"キ",u8"ク",u8"ケ",u8"コ",
	u8"サ",u8"シ",u8"ス",u8"セ",u8"ソ",
	u8"タ",u8"チ",u8"ツ",u8"テ",u8"ト",
	u8"ナ",u8"ニ",u8"ヌ",u8"ネ",u8"ノ",
	u8"ハ",u8"ヒ",u8"フ",u8"ヘ",u8"ホ",
	u8"マ",u8"ミ",u8"ﾑ",u8"メ",u8"モ",
	u8"ヤ",	u8"ユ",	u8"ヨ",
	u8"ラ",u8"リ",u8"ル",u8"レ",u8"ロ",
	u8"ワ",			u8"ヲ",
	u8"ン"
	};
	constexpr std::array romaji{
	u8"a",u8"i",u8"u",u8"e",u8"o",
	u8"ka",u8"ki",u8"ku",u8"ke",u8"ko",
	u8"sa",u8"shi",u8"su",u8"se",u8"so",
	u8"ta",u8"chi",u8"tsu",u8"te",u8"to",
	u8"na",u8"ni",u8"nu",u8"ne",u8"no",
	u8"ha",u8"hi",u8"fu",u8"he",u8"ho",
	u8"ma",u8"mi",u8"mu",u8"me",u8"mo",
	u8"ya",u8"yu",u8"yo",
	u8"ra",u8"ri",u8"ru",u8"re",u8"ro",
	u8"wa",			u8"o",
	u8"n"
	};
	constexpr std::array ipa{
	u8"ä",u8"i",u8"ɯ",u8"e",u8"o̞",
	u8"kä",u8"ki",u8"kɯ",u8"ke",u8"ko̞",
	u8"sä",u8"ɕi",u8"sɯ",u8"se",u8"so̞",
	u8"tä",u8"tɕi",u8"tsɯ",u8"te",u8"to̞",
	u8"nä",u8"ɲi",u8"nɯ",u8"ne",u8"no̞",
	u8"hä",u8"hi",u8"ɸɯ",u8"he",u8"ho̞",
	u8"mä",u8"mi",u8"mɯ",u8"me",u8"mo̞",
	u8"jä",u8"jɯ",u8"jo̞",
	u8"ɾä",u8"ɾi",u8"ɾɯ",u8"ɾe",u8"ɾo̞",
	u8"wä",			u8"o̞",
	u8"n/m/ŋ/N/ũ͍/ĩ"
	};
	constexpr std::size_t hiragana_size{hiragana.size()};
	constexpr std::size_t total_kanas{hiragana.size()+katakana.size()};
	static_assert(total_kanas!=0);
	static_assert(hiragana_size==katakana.size());
	static_assert(hiragana_size==romaji.size());
	static_assert(hiragana_size==ipa.size());
	fast_io::ibuf_white_hole_engine engine;
	std::uniform_int_distribution<std::size_t> dis(0,total_kanas-1);
	::std::uint_least64_t rounds{};
	::std::uint_least64_t correct_rounds{};
	for(std::u8string buffer;;++rounds)
	{
		std::size_t v{dis(engine)};
		std::size_t correct_pos{v};
		char8_t const* s;
		if(v<hiragana_size)
		{
			s=hiragana[v];
		}
		else
		{
			correct_pos-=hiragana_size;
			s=katakana[correct_pos];
		}
		print(fast_io::u8c_stdout(),rounds,u8"/",correct_rounds,u8": Romaji of ",fast_io::mnp::os_c_str(s),u8" is??\n");
		if(!scan<true>(fast_io::u8c_stdin(),buffer))
		{
			break;
		}
		bool correct{};
		if(buffer==std::u8string_view(romaji[correct_pos]))
		{
			++correct_rounds;
			correct=true;
		}
		println(fast_io::u8c_stdout(),
			fast_io::mnp::cond(correct,u8"Right!",u8"\aWrong!"),
			u8"\tRomaji: ",fast_io::mnp::os_c_str(romaji[correct_pos]),
			u8"\tHiragana: ",fast_io::mnp::os_c_str(hiragana[correct_pos]),
			u8"\tKatakana: ",fast_io::mnp::os_c_str(katakana[correct_pos]),
			u8"\tIPA: ",fast_io::mnp::os_c_str(ipa[correct_pos]));

	}
	println(fast_io::u8c_stdout(),u8"Total rounds:", rounds,u8"\nCorrect:", correct_rounds);
}