/*
Referenced from:
https://blog.csdn.net/pi9nc/article/details/11978989
*/
#include<fast_io.h>
#include<fast_io_device.h>
#include<vector>
#include<stack>

struct gabow_ctx
{
std::vector<std::vector<std::size_t>> graph;
std::vector<std::size_t> order,part;
std::stack<std::size_t
#if defined(_MSVC_STL_UPDATE)
,std::vector<std::size_t> //msvc STL has performance issues with deque. use vector instead
#endif
> path,root;
std::size_t order_num{};
std::vector<std::vector<std::size_t>> component_graph;
void dfs(std::size_t v)
{
	order[v]=++order_num;
	path.push(v);
	root.push(v);
	for(std::size_t to : graph[v])
	{
		if(order[to]==SIZE_MAX)//not visited
		{
			dfs(to);
		}
		else if(part[to]==SIZE_MAX)//visited but label not set
		{
			for(;order[root.top()]>order[to];root.pop());
		}
	}
	if(v==root.top())[[unlikely]]
	{
		root.pop();
		std::size_t const part_num{component_graph.size()};
		auto &componentg{component_graph.emplace_back()};
		std::size_t top{};
		do
		{
			top=path.top();
			part[top]=part_num;
			componentg.emplace_back(top);
			path.pop();
		}while(top!=v);
	}
}
void run()
{
	std::size_t const n{graph.size()};
	order.assign(n,SIZE_MAX);
	part.assign(n,SIZE_MAX);
	for(std::size_t i{};i!=n;++i)
		if(order[i]==SIZE_MAX)
			dfs(i);
}

};

using namespace fast_io::io;

int main()
{
	gabow_ctx ctx;
	std::size_t n;
	{
		fast_io::u8ibuf_file ibf(u8"directed_graph.txt");
		std::size_t m;
		scan(ibf,m,n);
		auto &graph{ctx.graph};
		graph.assign(n,{});
		std::size_t const average{(m/n+1)*13/10};
		for(auto& v : graph)
			v.reserve(average);
		for(std::size_t i{};i!=m;++i)
		{
			std::size_t a,b;
			scan(ibf,a,b);
			graph[a].push_back(b);
		}
	}
	ctx.run();
	fast_io::u8obuf_file obf(u8"gabow.txt");
	auto& component_graph{ctx.component_graph};
	for(std::size_t i{},sz{component_graph.size()};i!=sz;++i)
	{
		print(obf,i,u8":");
		for(auto const& e : component_graph[i])
		{
			print(obf,u8"\t",e);
		}
		println(obf);
	}
}
