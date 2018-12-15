#ifndef PARALLELGRAPH_GRAPH_HPP
#define PARALLELGRAPH_GRAPH_HPP
#include<iostream>
#include <fstream>
#include <map>
#include <mutex>
#include <queue>
#include <graphpara/util/csr_storage.hpp>
#include <graphpara/graph/graph_basic_types.hpp>
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
#include <graphpara/util/counting_sort.hpp>


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
	typedef VertexData vertex_data_type;
	typedef EdgeData edge_data_type;
	typedef std::vector<std::pair<lvid_type, edge_id_type>>  edge_list_type;
	graph(){};
	~graph(){};
	void show_graph();
	void show_vertex();
	void show_edge();
	void save();
	class vertex_type;
	class edge_type;
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


	class vertex_type
	{
	 private:
	  lvid_type vid;
	  graph& graph_ref;
	 public:
	  vertex_type(graph& graph_ref, lvid_type vid):graph_ref(graph_ref), vid(vid){}
	  const vertex_data_type& data() const
	  {
		return graph_ref.vertex_data(vid);
	  };
	  vertex_data_type& data()
	  {
		return graph_ref.vertex_data(vid);
	  };
	  vertext_id_type global_id()
	  {
		  return graph_ref.get_global_id(vid);
	  }
	  size_t num_in_edges() const 
	  {
	  	return graph_ref.num_in_edges(vid);
	  }
	  
	  size_t num_out_edges() const 
	  {
	  	return graph_ref.num_out_edges(vid);
	  }
	
	  lvid_type id() const 
	  {
	  	return vid;
	  }	
	  edge_list_type in_edges() 
	  {
	  	return graph_ref.in_edges(vid);
	  }
	  edge_list_type out_edges() 
	  {
	  	return graph_ref.out_edges(vid);
	  }		 

	};
	class edge_type
	{
	 private:
	  graph& graph_ref;
	  lvid_type _source;
	  lvid_type _target;
	  edge_id_type _eid;
	 public:
	  edge_type(graph& graph_ref, lvid_type _source, lvid_type _target, edge_id_type _eid) :
	   graph_ref(graph_ref), _source(_source), _target(_target), _eid(_eid) { }
	  const edge_data_type& data() const {
        return graph_ref.edge_data(_eid);
      }
      /// \brief Returns a reference to the data on the edge.
      edge_data_type& data() {
        return graph_ref.edge_data(_eid);
      }
      /// \brief Returns the source vertex of the edge.
      vertex_type source() const {
        return vertex_type(graph_ref, _source);
      }
      /// \brief Returns the target vertex of the edge.
      vertex_type target() const {
        return vertex_type(graph_ref, _target);
      }
      /// \brief Returns the internal ID of this edge
      edge_id_type id() const { return _eid; }

	};
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

	VertexData& vertex_data(lvid_type v) 
	{
      return vertices[v];
    } // end of data(v)
	EdgeData& edge_data(edge_id_type eid) {      
      return edges[eid]; 
    }
    const EdgeData& edge_data(edge_id_type eid) const {
      return edges[eid]; 
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
		for(int i  =0; i <= max_val;i++)
			this->vertices.push_back(1);
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
		_csc_storage.values.begin()+end);
		// if(end >= begin)
		// return std::vector<std::pair<lvid_type, edge_id_type>> (
		// 	_csc_storage.values.begin()+begin,
		// 	_csc_storage.values.begin()+end
		// 	);
		// else
		// return std::vector<std::pair<lvid_type, edge_id_type>>();
		
	}
	size_t num_in_edges(const lvid_type v) const 
	{      
      return _csc_storage.value_ptrs[v+1]-_csc_storage.value_ptrs[v];
    }    
    size_t num_out_edges(const lvid_type v) const 
	{

      return _csr_storage.value_ptrs[v+1]-_csr_storage.value_ptrs[v];
    }
	size_t num_vertices()
	{
		return vertices.size();
	}
	size_t num_edges()
	{
		return edges.size();
	}
	vertex_type vertex(vid_type vid)
	{
		return vertex_type(*this, vid);
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
	
	show_vertex();
	show_edge();
	
}
template <typename VertexData, typename EdgeData>
void graph<VertexData, EdgeData>::show_vertex()
{
	for(auto x: this->global_id2local_id)
	{
		size_t global_id = x.first;
		size_t local_id = x.second;
		std::cout<<x.first<<"###"<<x.second<<std::endl;
		vertex_type vertex = this->vertex(local_id);
		std::cout<<vertex.data()<<std::endl;
	}
	
}
template <typename VertexData, typename EdgeData>
void graph<VertexData, EdgeData>::show_edge()
{
	for(auto x: this->global_id2local_id)
	{
		size_t global_id = x.first;
		size_t local_id = x.second;
		std::cout<<"out edges \n";
		for(auto e: this->out_edges(local_id))
		{
			size_t target_id = get_global_id(e.first);
			edge_id_type eid = e.second;
			edge_type edge(*this, global_id, target_id, eid);
			std::cout<<global_id<<"-->"<<target_id<<"##"<<edge.data()<<std::endl;
		}
		std::cout<<std::endl;
		std::cout<<"in edges \n";
		for(auto e: this->in_edges(local_id))
		{
			size_t source_id = get_global_id(e.first);
			edge_id_type eid = e.second;
			edge_type edge(*this, source_id, global_id, eid);
			std::cout<<source_id<<"-->"<<global_id<<"##"<<edge.data()<<std::endl;
		}
	}
	
}
template <typename VertexData, typename EdgeData>
void graph<VertexData, EdgeData>::save()
{
	std::string fname = "vertex.data";
	std::fstream s(fname, s.out);
	if(!s.is_open())
	std::cout<<"failed to open "<<fname<<std::endl;
	for(auto x: this->global_id2local_id)
	{
		size_t global_id = x.first;
		size_t local_id = x.second;
		vertex_type vertex = this->vertex(local_id);
		s<<x.first<<"###"<<x.second<<"###"<<vertex.data()<<std::endl;
	}
	std::cout<<"run to here\n";
}
}
#endif