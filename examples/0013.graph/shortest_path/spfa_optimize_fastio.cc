#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/queue.h>
#include <fast_io_dsal/vector.h>

struct node
{
	std::size_t to, weight;
};

using namespace fast_io::io;

int main()
{
	fast_io::ibuf_file ibf("graph.txt");
	std::size_t m, n;
	scan(ibf, m, n);
	::fast_io::vector<::fast_io::vector<node>> graph(n);
	std::size_t const average{(m / n + 1) * 13 / 10};
	for (auto &v : graph)
	{
		v.reserve(average);
	}
	for (std::size_t i{}; i != m; ++i)
	{
		std::size_t a, b, w;
		scan(ibf, a, b, w);
		graph[a].push_back({b, w});
	}
	::fast_io::vector<std::size_t> relax(n, SIZE_MAX);
	::fast_io::vector<char8_t> occupied(n);
	::fast_io::queue<std::size_t> queue;
	occupied.front() = true;
	for (queue.push(relax.front() = 0); !queue.is_empty(); queue.pop())
	{
		auto front{queue.front()};
		auto minimum_weight{relax[front]};
		for (auto e : graph[front])
		{
			if (minimum_weight + e.weight < relax[e.to])
			{
				relax[e.to] = minimum_weight + e.weight;
				if (!occupied[e.to])
				{
					occupied[e.to] = true;
					queue.push(e.to);
				}
			}
		}
		occupied[front] = false;
	}
	fast_io::obuf_file obf("spfa.txt");
	if (relax.back() == SIZE_MAX)
	{
		print(obf, "no answer\n");
	}
	else
	{
		println(obf, relax.back());
	}
}