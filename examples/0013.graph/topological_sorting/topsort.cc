#include<fast_io.h>
#include<fast_io_device.h>
#include<queue>

using namespace fast_io::io;

int main()
{
	fast_io::u8ibuf_file ibf(u8"directed_graph.txt");
	std::size_t m,n;
	scan(ibf,m,n);
	std::vector<std::vector<std::size_t>> graph(n);
	std::vector<std::size_t> indegree(n);
	std::size_t const average{(m/n+1)*13/10};
	for(auto& v : graph)
		v.reserve(average);
	for(std::size_t i{};i!=m;++i)
	{
		std::size_t a,b;
		scan(ibf,a,b);
		graph[a].push_back(b);
		++indegree[b];
	}
	fast_io::u8obuf_file u8obf(u8"topsort.txt");
	std::queue<std::size_t> queue;
	for(std::size_t i{};i!=n;++i)
	{
		if(!indegree[i])
		{
			queue.push(i);
			println(u8obf,i);
		}
	}
	for(;!queue.empty();queue.pop())
	{
		for(auto const & e : graph[queue.front()])
		{
			if(!(--indegree[e]))
			{
				println(u8obf,e);
				queue.push(e);
			}
		}
	}

}
