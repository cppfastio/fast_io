#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/priority_queue.h>
#include <fast_io.h>
#include <fast_io_device.h>

struct node
{
	std::size_t to, weight;
};

inline constexpr bool operator==(node const &a, node const &b) noexcept
{
	return a.to == b.to;
}

inline constexpr auto operator<=>(node const &a, node const &b) noexcept
{
	return a.to <=> b.to;
}

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
		graph.index_unchecked(a).push_back({b, w});
	}
	::fast_io::priority_queue<node, ::std::ranges::greater> queue;
	::fast_io::vector<std::size_t> relax(n, SIZE_MAX);
	for (queue.push({relax.front() = 0, 0}); !queue.is_empty();)
	{
		auto [minimum_weight, minimum_node]{queue.pop_element_unchecked()};
		if (relax.index_unchecked(minimum_node) < minimum_weight)
		{
			continue;
		}
		for (auto e : graph.index_unchecked(minimum_node))
		{
			if (minimum_weight + e.weight < relax.index_unchecked(e.to))
			{
				queue.push({relax.index_unchecked(e.to) = minimum_weight + e.weight, e.to});
			}
		}
	}
	fast_io::obuf_file obf("dijkstra.txt");
	if (relax.back_unchecked() == SIZE_MAX)
	{
		print(obf, "no answer\n");
	}
	else
	{
		println(obf, relax.back_unchecked());
	}
}
