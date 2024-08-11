#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	fast_io::vector<int> numbers{2, 4, 6, 8};

	print("Second element: ", numbers[1], "\n");

	numbers[0] = 5;

	println("All numbers: ", rgvw(numbers, " "));
}

// Gets the sum of all primes in [0, N) using sieve of Eratosthenes
consteval auto sum_of_all_primes_up_to(unsigned N)
{
	if (N < 2)
	{
		return 0ULL;
	}

	fast_io::vector<bool> is_prime(N, true);
	is_prime[0] = is_prime[1] = false;

	auto propagate_non_primality = [&](decltype(N) n) {
		for (decltype(N) m = n + n; m < is_prime.size(); m += n)
		{
			is_prime[m] = false;
		}
	};

	auto sum{0ULL};
	for (decltype(N) n{2}; n != N; ++n)
	{
		if (is_prime[n])
		{
			sum += n;
			propagate_non_primality(n);
		}
	}

	return sum;
} //< vector's memory is released here

static_assert(sum_of_all_primes_up_to(42) == 0xEE);
static_assert(sum_of_all_primes_up_to(100) == 0x424);
static_assert(sum_of_all_primes_up_to(1001) == 76127);
