#include<iostream>
#include <string>
#include <graph/graph.hpp>
#include <fstream>
using namespace parallelgraph;
int main(int argc, char const *argv[])
{
	std::string file_path="/home/chen/ict/ParallelGraph/tests/dd";
	graph<double, double> graphi;
	graphi.directed=true;
	graphi.load(file_path, 1.1);
	graphi.finalize();
	graphi.show_graph();
	return 0;
}
