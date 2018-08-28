#ifndef PARALLELGRAPH_IVERTEX_PROGRAM_HPP
#define PARALLELGRAPH_IVERTEX_PROGRAM_HPP
#include<graph/graph_basic_types.hpp>
#include<vertex_program/context.hpp>
namespace parallelgraph 
{
 template<typename Graph,
          typename GatherType>
 class ivertex_program 
 {
  public:
    typedef typename Graph::vertex_data_type vertex_data_type;
    typedef typename Graph::edge_data_type edge_data_type;
    typedef GatherType gather_type;
    typedef Graph graph_type;
    typedef typename graph_type::vertex_type vertex_type;
    typedef typename graph_type::edge_type edge_type;
    typedef parallelgraph::edge_dir_type edge_dir_type;
    typedef icontext icontext_type;
    virtual ~ivertex_program() { }
    virtual void init(const vertex_type& vertex) { /** NOP */ }  
    virtual edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const
    { 
        return IN_EDGES;
    } 
    virtual gather_type gather(icontext_type& context, const vertex_type& vertex,
                               edge_type& edge ) const
    {
        return gather_type();
    }
    virtual void apply(icontext_type& context, vertex_type& vertex, gather_type& total) {}//= 0;
    virtual edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const
    { 
        return OUT_EDGES;
    } 
    virtual void scatter(icontext_type& context, const vertex_type vertex,edge_type& edge) const {}    
 };
          
}
#endif