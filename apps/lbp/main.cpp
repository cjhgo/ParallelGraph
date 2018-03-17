#include<iostream>
#include<parallel_graph.hpp>
#include <Eigen/Dense>

// #include "eigen_serialization.hpp"
using namespace parallelgraph;	

typedef Eigen::VectorXd factor_type;
struct vertex
{
	factor_type belief;
	factor_type potential;
};
struct edge
{
	factor_type messages[4];
};

void load_vdata(graph<vertex, edge> & g, std::string file_path)
{
	std::vector<double> values(5);
	int vid=0;
	
	std::ifstream infile(file_path);
	

	while(infile >>vid>>values[0]>>values[1]>>values[2]>>values[3]>>values[4])
	{		
		vertex vdata;
		vdata.potential.resize(5);
		for(size_t i = 0; i < 5; i++)
		{
			vdata.potential(i) = values[i];
		}
		
		// vdata.belief.resize(5);
		g.add_vertex(vid, vdata);

	}
}

int main(int argc, char const *argv[])
{
    // factor_type a;
    // factor_type b;
    // a.resize(3);
    // b.resize(3);
    // a<<2,-1,2;
    // b<<-1,3,-2;
    // factor_type c = a -b;
    // std::cout<<a<<std::endl;
    // std::cout<<b<<std::endl;
    // std::cout<<c<<std::endl;
    // std::cout<<c.cwiseAbs();
    // std::cout<<c.cwiseAbs().sum();

    std::string file_path="smallsynth_edata.tsv";
    std::string vfile_path = "/home/chen/ict/ParallelGraph/debug/apps/lbp/smallsynth_vdata.tsv";
    graph<vertex, edge> graphi;
    graphi.directed=true;
    graphi.load(file_path);
    graphi.load_vertex(vfile_path, load_vdata);
    graphi.finalize();
    std::cout<<"#########\n";
    for(auto x: graphi.out_edges(1))
    {
    	std::cout<<x.first<<std::endl<<x.second<<std::endl;
    	std::cout<<"\n\n";
    }
    return 0;
}