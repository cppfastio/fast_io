#include<string>
#include<fast_io.h>
#include<random>
#include<array>
#include<string_view>

using namespace fast_io::io;

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
	u8"ん",


	u8"きゃ",u8"きゅ",u8"きょ",
	u8"しゃ",u8"しゅ",u8"しょ",
	u8"ちゃ",u8"ちゅ",u8"ちょ",
	u8"にゃ",u8"にゅ",u8"にょ",
	u8"ひゃ",u8"ひゅ",u8"ひょ",
	u8"みゃ",u8"みゅ",u8"みょ",

	u8"りゃ",u8"りゅ",u8"りょ",


	u8"が",u8"ぎ",u8"ぐ",u8"げ",u8"ご",
	u8"ざ",u8"じ",u8"ず",u8"ぜ",u8"ぞ",
	u8"だ",u8"ぢ",u8"づ",u8"で",u8"ど",
	u8"ば",u8"び",u8"ぶ",u8"べ",u8"ぼ",
	u8"ぱ",u8"ぴ",u8"ぷ",u8"ぺ",u8"ぽ",

	u8"ぎゃ",u8"ぎゅ",u8"ぎょ",
	u8"じゃ",u8"じゅ",u8"じょ",

	u8"びゃ",u8"びゅ",u8"びょ",
	u8"ぴゃ",u8"ぴゅ",u8"ぴょ"
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
	u8"ン",


	u8"キャ",u8"キュ",u8"キョ",
	u8"シャ",u8"シュ",u8"ショ",
	u8"チャ",u8"チュ",u8"チョ",
	u8"ニャ",u8"ニュ",u8"ニョ",
	u8"ヒャ",u8"ヒュ",u8"ヒョ",
	u8"ミャ",u8"ミュ",u8"ミョ",

	u8"リャ",u8"リュ",u8"リョ",


	u8"ガ",u8"ギ",u8"グ",u8"ゲ",u8"ゴ",
	u8"ザ",u8"ジ",u8"ズ",u8"ゼ",u8"ゾ",
	u8"ダ",u8"ヂ",u8"ヅ",u8"デ",u8"ド",
	u8"バ",u8"ビ",u8"ブ",u8"ベ",u8"ボ",
	u8"パ",u8"ピ",u8"プ",u8"ペ",u8"ポ",

	u8"ギャ",u8"ギュ",u8"ギョ",
	u8"ジャ",u8"ジュ",u8"ジョ",

	u8"ビャ",u8"ビュ",u8"ビョ",
	u8"ピャ",u8"ピュ",u8"ピョ"
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
	u8"n",


	u8"kya",u8"kyu",u8"kyo",
	u8"sha",u8"shu",u8"sho",
	u8"cha",u8"chu",u8"cho",
	u8"nya",u8"nyu",u8"nyo",
	u8"hya",u8"hyu",u8"hyo",
	u8"mya",u8"myu",u8"myo",

	u8"rya",u8"ryu",u8"ryo",


	u8"ga",u8"gi",u8"gu",u8"ge",u8"go",
	u8"za",u8"zi",u8"zu",u8"ze",u8"zo",
	u8"da",u8"di",u8"du",u8"de",u8"do",
	u8"ba",u8"bi",u8"bu",u8"be",u8"bo",
	u8"pa",u8"pi",u8"pu",u8"pe",u8"po",


	u8"gya",u8"gyu",u8"gyo",
	u8"ja",u8"ju",u8"jo",

	u8"bya",u8"byu",u8"byo",
	u8"pya",u8"pyu",u8"pyo"
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
	u8"n/m/ŋ/N/ũ͍/ĩ",


	u8"kʲä",u8"kʲɯ",u8"kʲo̞",
	u8"ɕʲä",u8"ɕʲɯ",u8"ɕʲo̞",
	u8"tɕʲä",u8"tɕʲɯ",u8"tɕʲo̞",
	u8"ɲʲä",u8"ɲʲɯ",u8"ɲʲo̞",
	u8"hʲä",u8"hʲɯ",u8"hʲo̞",
	u8"mʲä",u8"mʲɯ",u8"mʲo̞",

	u8"ɾʲä",u8"ɾʲɯ",u8"ɾʲo̞",


	u8"ɡä",u8"gi",u8"gɯ",u8"ge",u8"go̞",
	u8"zä",u8"ʑi",u8"ʑɯ",u8"ze",u8"zo̞",
	u8"dä",u8"ʑi",u8"ʑɯ",u8"de",u8"do̞",
	u8"bä",u8"bi",u8"bɯ",u8"be",u8"bo̞",
	u8"pä",u8"pi",u8"pɯ",u8"pe",u8"po̞",


	u8"gʲä",u8"gʲɯ",u8"gʲo̞",
	u8"dʑä",u8"dʑɯ",u8"dʑo̞",

	u8"bʲä",u8"bʲɯ",u8"bʲo̞",
	u8"pʲä",u8"pʲɯ",u8"pʲo̞"
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
