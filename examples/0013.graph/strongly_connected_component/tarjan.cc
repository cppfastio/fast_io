/*
Referenced from:
https://www.cnblogs.com/Mr94Kevin/p/9651975.html
*/
#include<fast_io.h>
#include<fast_io_device.h>
#include<vector>
#include<stack>

struct tarjan_ctx
{
std::vector<std::vector<std::size_t>> graph;
std::vector<std::size_t> dfn,low,part;
std::vector<bool> ins;
std::stack<std::size_t
#if defined(_MSVC_STL_UPDATE)
,std::vector<std::size_t> //msvc STL has performance issues with deque. use vector instead
#endif
> visited;
std::size_t vid{};
std::vector<std::vector<std::size_t>> component_graph;
void dfs(std::size_t v)
{
	dfn[v]=low[v]=++vid;
	visited.push(v);
	ins[v]=1;
	for(std::size_t to : graph[v])
	{
		if(!dfn[to])//not visited
		{
			dfs(to);
			if(low[to]<low[v])
				low[v]=low[to];
		}
		else if(ins[to])
		{
			if(low[to]<low[v])
				low[v]=low[to];
		}
	}
	if(dfn[v]==low[v])
	{
		std::size_t const part_num{component_graph.size()};
		auto &componentg{component_graph.emplace_back()};
		std::size_t top{};
		do
		{
			top=visited.top();
			ins[top]=0;
			part[top]=part_num;
			componentg.emplace_back(top);
			visited.pop();
		}while(top!=v);
	}
}
void run()
{
	std::size_t const n{graph.size()};
	dfn.assign(n,0);
	low.assign(n,0);
	ins.assign(n,0);
	part.assign(n,SIZE_MAX);
	for(std::size_t i{};i!=n;++i)
		if(dfn[i]==0)
			dfs(i);
}

};

using namespace fast_io::io;

int main()
{
	tarjan_ctx ctx;
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
	fast_io::u8obuf_file obf(u8"tarjan.txt");
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
