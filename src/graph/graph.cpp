#include <iostream>
#include <fstream>
#include "graph.hpp"

template <typename T>
void graph::load(std::string file_path)
{
	std::ifstream infile(file_path);
	int source, target;
	while(infile >> source >> target)
	{

		if(this->directed)
		{
			this->add_edge(source,target);
		}
		else
		{
			this->add_edge(source, target);
			this->add_edge(target, source);
		}

	}
}



template <typename T>
int graph::get_local_id(int global_id)
{
	if(global_id2local_id.find(global_id) == global_id2local_id.end())
	{
		this->current_local_id++;
		global_id2local_id[global_id] = this->current_local_id;
		this->matrix.resize(this->current_local_id+1);
		for(auto& row : this->matrix)
			row.resize(this->current_local_id+1);
		return this->current_local_id;
	}
	else
	{
		auto search = global_id2local_id.find(global_id);
		return search->second;
	}

}

template <typename T>
void graph::show_graph()
{
	for(auto x: this->global_id2local_id)
	{
		std::cout<<x.first<<"###"<<x.second<<std::endl;
	}
	std::cout<<this->matrix.size()<<std::endl;
	for(auto row : this->matrix)
	{
		for(auto ele : row)
		{
			std::cout<<ele<<"---";
		}
		std::cout<<std::endl;
	}
}

template <typename T>
void graph::add_edge(int source, int target, T data)
{

	int source_local_id = get_local_id(source);
	int target_local_id = get_local_id(target);	
	this->matrix[source_local_id][target_local_id]=data;
}