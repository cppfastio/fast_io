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
	std::priority_queue<std::pair<std::size_t,std::size_t>,std::vector<std::pair<std::size_t,std::size_t>>,std::greater<std::pair<std::size_t,std::size_t>>> queue;
	std::vector<std::size_t> relax(n,SIZE_MAX);
	for(queue.push({relax.front()=0,0});!queue.empty();)
	{
		auto [minimum_weight,minimum_node]{queue.top()};
		queue.pop();
		if(relax[minimum_node]<minimum_weight)
			continue;
		for(auto e: graph[minimum_node])
			if(minimum_weight+e.weight<relax[e.to])
				queue.push({relax[e.to]=minimum_weight+e.weight,e.to});
	}
	fast_io::obuf_file obf("dijkstra.txt");
	if(relax.back()==SIZE_MAX)
		print(obf,"no answer\n");
	else
		println(obf,relax.back());
}