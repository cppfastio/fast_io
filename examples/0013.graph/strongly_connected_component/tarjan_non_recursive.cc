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
struct tarjan_dfs_stack_content
{
	std::size_t from{};
	std::size_t i{};
};
std::vector<std::vector<std::size_t>> graph;
std::vector<std::size_t> dfn,low,part;
std::vector<bool> ins;
std::stack<std::size_t,std::vector<std::size_t>> visited;
std::size_t vid{};
std::stack<tarjan_dfs_stack_content,std::vector<tarjan_dfs_stack_content>> dfs_stack;
std::vector<std::vector<std::size_t>> component_graph;
void dfs(std::size_t source)
{
	for(dfs_stack.push({source,0});!dfs_stack.empty();)
	{
		auto& dfs_stack_top{dfs_stack.top()};
		auto from{dfs_stack_top.from};
		std::size_t topi{dfs_stack_top.i};
		auto const& g{graph[from]};
		if(topi==0)
		{
			dfn[from]=low[from]=++vid;
			visited.push(from);
			ins[from]=1;			
		}
		else
		{
			std::size_t to{g[topi-1]};
			if(low[to]<low[from])
				low[from]=low[to];
		}
		std::size_t const gsize{g.size()};
		for(;topi!=gsize;++topi)
		{
			std::size_t to{g[topi]};
			if(!dfn[to])//not visited
			{
				++topi;
				dfs_stack_top.i=topi;
				dfs_stack.push({to,0});//push dfs stack
				goto next_round;
			}
			else if(ins[to])
			{
				if(low[to]<low[from])
					low[from]=low[to];
			}
		}
		if(dfn[from]==low[from])
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
			}while(top!=from);
		}
		dfs_stack.pop();
		next_round:;
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
	fast_io::u8obuf_file obf(u8"tarjan_non_recursive.txt");
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
