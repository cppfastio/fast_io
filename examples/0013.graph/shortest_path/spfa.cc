#include<fast_io.h>
#include<fast_io_device.h>
#include<queue>
#include<vector>

struct node
{
	std::size_t to,weight;
};

int main()
{
	fast_io::ibuf_file ibf("graph.txt");
	std::size_t m,n;
	scan(ibf,m,n);
	std::vector<std::vector<node>> graph(n);
	for(std::size_t i{};i!=m;++i)
	{
		std::size_t a,b,w;
		scan(ibf,a,b,w);
		graph[a].push_back({b,w});
	}
	std::vector<std::size_t> relax(n,SIZE_MAX);
	std::vector<char8_t> occupied(n);
	std::queue<std::size_t> queue;
	occupied.front()=true;
	for(queue.push(relax.front()=0);!queue.empty();queue.pop())
	{
		auto minimum_weight{relax[queue.front()]};
		for(auto e: graph[queue.front()])
			if(minimum_weight+e.weight<relax[e.to])
			{
				relax[e.to]=minimum_weight+e.weight;
				if(!occupied[e.to])
				{
					occupied[e.to]=true;
					queue.push(e.to);
				}
			}
		occupied[queue.front()]=false;
	}
	fast_io::obuf_file obf("spfa.txt");
	if(relax.back()==SIZE_MAX)
		print(obf,"no answer\n");
	else
		println(obf,relax.back());
}