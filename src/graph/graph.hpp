#ifndef PARALLELGRAPH_GRAPH_HPP
#define PARALLELGRAPH_GRAPH_HPP
#include<iostream>
#include <fstream>
#include <map>
#include <mutex>
#include <queue>
#include <util/csr_storage.hpp>
#include <graph/graph_basic_types.hpp>
#include <cmath>
#include <string>
#include <list>
#include <vector>
#include <set>
#include <map>

#include <queue>
#include <algorithm>
#include <functional>
#include <fstream>

#include <boost/bind.hpp>
#include <boost/unordered_set.hpp>
#include <boost/type_traits.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/range/iterator_range.hpp>
#include <util/counting_sort.hpp>


namespace parallelgraph{
/**
 * the representation of graph 
 * std::vector<VertexData> vertices;
 * std::vector<EdgeData> edges;
 * csr_type _csr_storage;
 * csr_type _csc_storage;
 * std::map<int, int> global_id2local_id;
 * std::map<int, int> local_id2gloabl_id;
 * load 
 * load graph from file 
 * add_edge
 * add_vertex
 * out_edges
 * in_edges
 */


template <typename VertexData, typename EdgeData>
class edge_buffer
{
public:
	std::vector<EdgeData>  data;
	std::vector<lvid_type> source_arr;
	std::vector<lvid_type> target_arr;

	void add_edge(lvid_type source, lvid_type target, EdgeData _data)
	{
		source_arr.push_back(source);
		target_arr.push_back(target);
		data.push_back(_data);
	}

};


template <typename VertexData, typename EdgeData>
class graph
{
public:
	graph(){};
	~graph(){};
	void show_graph();
	void load_vertex(std::string file_path, void (*laod_vertex_parser)(graph &, std::string ))
	{
		laod_vertex_parser(*this, file_path);

	}
	void load(std::string file_path, EdgeData data=EdgeData())
	{
		std::ifstream infile(file_path);
		int source, target;
		while(infile >> source >> target)
		{

			if(this->directed)
			{
				this->add_edge(source,target,data);
			}
			else
			{
				this->add_edge(source, target,data);
				this->add_edge(target, source,data);
			}

		}
	}


	bool directed = true;
	int current_local_id = -1;
// private:
	
	std::map<int, int> global_id2local_id;
	std::map<int, int> local_id2gloabl_id;
	int get_local_id(int global_id);
	int get_global_id(int global_id);	
	
	void add_edge(lvid_type source, lvid_type target, EdgeData data=EdgeData())
	{
		lvid_type source_local_id = get_local_id(source);
		lvid_type target_local_id = get_local_id(target);	
		this->local_edge_buffer.add_edge(source_local_id, target_local_id, data);
	}

	void add_vertex(const vertext_id_type _vid, VertexData vdata)
	{
		
		lvid_type vid = get_local_id(_vid);
		if(vid >= vertices.size()) 
		{
		  // Enable capacity doubling if resizing beyond capacity
		  if(vid >= vertices.capacity()) 
		  {
		    const size_t new_size = std::max(2 * vertices.capacity(), 
		                                     size_t(vid));
		    vertices.reserve(new_size);
		  }
		  vertices.resize(vid+1);
		}
		
		this->vertices[vid]=vdata;
	}

	void finalize()
	{
		size_t max_val = 0;
		for (auto const& x : global_id2local_id)
		{
			size_t g_id = x.first;
			size_t l_id = x.second;
			if(l_id > max_val)
				max_val = l_id;
			this->local_id2gloabl_id[l_id]=g_id;
		}
		std::vector<edge_id_type> src_permute;
		std::vector<edge_id_type> dest_permute;
		std::vector<edge_id_type> src_counting_prefix_sum;
		std::vector<edge_id_type> dest_counting_prefix_sum;


		std::vector<std::pair<lvid_type, edge_id_type>> csr_values;
		std::vector<std::pair<lvid_type, edge_id_type>> csc_values;


		counting_sort(local_edge_buffer.source_arr, dest_permute, &src_counting_prefix_sum, max_val);

		counting_sort(local_edge_buffer.target_arr, src_permute, &dest_counting_prefix_sum, max_val);
		for(size_t i = 0; i < dest_permute.size(); i++)
		{
			auto pos = dest_permute[i];
			csr_values.push_back(std::pair<lvid_type, edge_id_type>(
								local_edge_buffer.target_arr[pos],
								pos));
		}
		for(size_t i = 0; i < src_permute.size(); i++)
		{
			auto pos = src_permute[i];
			csc_values.push_back(std::pair<lvid_type, edge_id_type>(
								local_edge_buffer.source_arr[pos],
								pos));
		}

		_csr_storage.swap(csr_values, src_counting_prefix_sum);
		_csc_storage.swap(csc_values, dest_counting_prefix_sum);


		edges.swap(local_edge_buffer.data);


	}

	std::vector<std::pair<lvid_type, edge_id_type>> 
	out_edges(lvid_type v)
	{
		size_t begin = _csr_storage.value_ptrs[v];
		size_t end = _csr_storage.value_ptrs[v+1];
		
		return std::vector<std::pair<lvid_type, edge_id_type>> (
			_csr_storage.values.begin()+begin,
			_csr_storage.values.begin()+end
			);
	}
	std::vector<std::pair<lvid_type, edge_id_type>> 
	in_edges(lvid_type v)
	{
		size_t begin = _csc_storage.value_ptrs[v];
		size_t end = _csc_storage.value_ptrs[v+1];
		return std::vector<std::pair<lvid_type, edge_id_type>> (
			_csc_storage.values.begin()+begin,
			_csc_storage.values.begin()+end
			);
	}
// private:
	std::vector<VertexData> vertices;
	std::vector<EdgeData> edges;
	edge_buffer<VertexData, EdgeData> local_edge_buffer;
	typedef csr_storage<std::pair<lvid_type, edge_id_type>, edge_id_type> csr_type;
	csr_type _csr_storage;
	csr_type _csc_storage;


};





template <typename VertexData, typename EdgeData>
int graph<VertexData, EdgeData>::get_local_id(int global_id)
{
	
	if(global_id2local_id.find(global_id) == global_id2local_id.end())
	{
		this->current_local_id++;
		global_id2local_id[global_id] = this->current_local_id;
		return this->current_local_id;
	}
	else
	{
		auto search = global_id2local_id.find(global_id);
		return search->second;
	}

}

template <typename VertexData, typename EdgeData>
int graph<VertexData, EdgeData>::get_global_id(int local_id)
{

	auto search = local_id2gloabl_id.find(local_id);
	return search->second;

}

template <typename VertexData, typename EdgeData>
void graph<VertexData, EdgeData>::show_graph()
{
	for(size_t i = 0; i < _csr_storage.values.size();i++)
	{
		size_t source_id = get_global_id(_csr_storage.values[i].first);
		size_t index = _csr_storage.values[i].second;
		std::cout<<source_id<<"\t"<<index<<std::endl;
	}
	std::cout<<std::endl;
	for(size_t i = 0; i < _csc_storage.values.size();i++)
	{
		size_t target_id = get_global_id(_csc_storage.values[i].first);
		size_t index = _csc_storage.values[i].second;
		std::cout<<target_id<<"\t"<<index<<std::endl;
	}
	for(auto x: this->global_id2local_id)
	{
		std::cout<<x.first<<"###"<<x.second<<std::endl;
		size_t global_id = x.first;
		size_t local_id = x.second;
		std::cout<<"out edges \n";
		for(auto e: this->out_edges(local_id))
		{
			size_t target_id = get_global_id(e.first);
			std::cout<<global_id<<"-->"<<target_id<<std::endl;
		}
		std::cout<<std::endl;
		std::cout<<"in edges \n";
		for(auto e: this->in_edges(local_id))
		{
			size_t source_id = get_global_id(e.first);
			std::cout<<source_id<<"-->"<<global_id<<std::endl;
		}
	}
}
}
#endif