#include<string>
#include<vector>
#include<map>
#include <iostream>
#include <fstream>
#include<mutex>
#include <queue>
#include <string>

#include <graph/graph.hpp>

#include <fstream>
#include <pthread.h>
#include <unistd.h>

using namespace parallelgraph;	



struct vertex
{
	
};
struct edge
{
	
};
int main(int argc, char const *argv[])
{
	std::string file_path="dd";
	graph<vertex, edge> graphi;
	graphi.directed=true;
	graphi.load(file_path);
	graphi.finalize();
	std::cout<<"#########\n";
	for(auto x: graphi.out_edges(1))
	{
		std::cout<<x.first<<std::endl<<x.second<<std::endl;
	}
}
