/*
Referenced from:
https://blog.csdn.net/pi9nc/article/details/11978989
*/
#include<fast_io.h>
#include<fast_io_device.h>
#include<vector>
#include<stack>

/*
Recursive one will overflow the stack. We need to write non_recursive version.
*/

struct gabow_ctx
{

struct gabow_dfs_stack_content
{
	std::size_t from{};
	std::vector<std::size_t>::const_iterator iter{};
};
std::vector<std::vector<std::size_t>> graph;
std::vector<std::size_t> order,part;
std::stack<std::size_t,std::vector<std::size_t>> path,root;
std::size_t order_num{};
std::stack<gabow_dfs_stack_content,std::vector<gabow_dfs_stack_content>> dfs_stack;
std::vector<std::vector<std::size_t>> component_graph;
void dfs(std::size_t source)
{
	order[source]=++order_num;
	path.push(source);
	root.push(source);
	for(dfs_stack.push({source,graph[source].cbegin()});!dfs_stack.empty();)
	{
		auto& dfs_stack_top{dfs_stack.top()};
		auto from{dfs_stack_top.from};
		auto topit{dfs_stack_top.iter};
		auto const& g{graph[from]};
		for(auto gcend{g.cend()};topit!=gcend;++topit)
		{
			auto to{*topit};
			if(order[to]==SIZE_MAX)//not visited
			{
				order[to]=++order_num;
				path.push(to);
				root.push(to);
				dfs_stack_top.iter=++topit;
				dfs_stack.push({to,graph[to].cbegin()});//push dfs tatck
				goto next_round;
			}
			else if(part[to]==SIZE_MAX)//visited but label not set
			{
				for(;order[root.top()]>order[to];root.pop());
			}
		}
		if(from==root.top())[[unlikely]]
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
			}while(top!=from);
		}
		dfs_stack.pop();
		next_round:;
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
	auto& component_graph{ctx.component_graph};
	fast_io::u8obuf_file obf(u8"out.txt");
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
