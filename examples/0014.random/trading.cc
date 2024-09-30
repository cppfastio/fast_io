#include <fast_io.h>
#include <random>

int main()
{
	::fast_io::ibuf_white_hole_engine eng;
	constexpr double startprice{67.22};
	constexpr double startpe{28.79};
	constexpr double fairpe{22};
	double earnings{startprice / startpe};

	::std::bernoulli_distribution dised(0.3);
	::std::normal_distribution<double> disbull(0.5, 0.5);
	::std::normal_distribution<double> disbear(-1, 1.5);
	::std::bernoulli_distribution startbullmarket(0.95);
	constexpr double startmoney{1000000};
	double cash{startmoney};
	double shares{};
	::std::size_t n{0};
	double currentprice{startprice};
	double lastbuyprice{};
	double lastdaychange{};
	double pe{startpe};
	for (bool bull{true};;)
	{
		double totalmoney{cash + shares * currentprice};
		int invest{};
		::fast_io::io::print(::fast_io::out(), "round n:", n, "\n0:hold\t1:buy\t2:sell\n",
							 "current price:", ::fast_io::mnp::fixed(currentprice), "\tgrowed:", ::fast_io::mnp::fixed((currentprice - startprice) / startprice * 100), "%\n",
							 "\nTotal Growth:", ::fast_io::mnp::fixed((totalmoney - startmoney) / startmoney * 100), "%"
																													 "\nTotal money:",
							 ::fast_io::mnp::fixed(cash + shares * currentprice),
							 "\nYou have cash:", ::fast_io::mnp::fixed(cash),
							 "\nCurrent PE:", pe, "\n",
							 "Price change:", ::fast_io::mnp::fixed(lastdaychange * 100), "%\n",
							 "\nshares:", shares,
							 "\tstocks money:", ::fast_io::mnp::fixed(shares * currentprice),
							 "\nYour input:");
		::fast_io::io::scan(invest);
		if (invest != 0)
		{
			if (invest == 1)
			{
				shares = ::std::floor(cash / currentprice);
				if (cash < shares * currentprice)
				{
					::fast_io::io::print(::fast_io::out(), "buy failed\n");
					continue;
				}
				lastbuyprice = currentprice;
				cash -= shares * currentprice;
			}
			else if (invest == 2)
			{
				if (shares == 0)
				{
					::fast_io::io::print(::fast_io::out(), "sell failed\n");
					continue;
				}
				double cashincrease{};
				if (lastbuyprice < currentprice)
				{
					cashincrease = shares * ((currentprice - lastbuyprice) * 0.9 + lastbuyprice); // sell tax
				}
				else
				{
					cashincrease = shares * currentprice;
				}
				cash += cashincrease;
				shares = 0;
			}
		}
		if (bull)
		{
			lastdaychange = disbull(eng) / 100;
			currentprice *= (1 + lastdaychange);
			pe *= (1 + lastdaychange);
			::std::normal_distribution<double> disenterbear(32, 0.5);
			if (pe > disenterbear(eng))
			{
				bull = false;
			}
		}
		else
		{
			lastdaychange = disbear(eng) / 100;
			currentprice *= (1 + lastdaychange);
			pe *= (1 + lastdaychange);
			::std::normal_distribution<double> disexitbear(fairpe, 3);
			if (pe < disexitbear(eng))
			{
				bull = true;
			}
		}
		pe /= (1 + 0.12 / 365.2429);
		++n;
	}
}