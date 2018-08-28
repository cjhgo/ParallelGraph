#include <vector>
#include <string>
#include <fstream>
#include <parallelgraph.hpp>
double RESET_PROB = 0.15;

double TOLERANCE = 1.0E-2;

size_t ITERATIONS = 0;


// The vertex data is just the pagerank value (a double)
typedef double vertex_data_type;

// There is no edge data in the pagerank application
//typedef parallelgraph::empty edge_data_type;
typedef double edge_data_type;

// The graph type is determined by the vertex and edge data types
typedef parallelgraph::graph<vertex_data_type, edge_data_type> graph_type;

/*
 * A simple function used by graph.transform_vertices(init_vertex);
 * to initialize the vertes data.
 */
void init_vertex(graph_type::vertex_type& vertex) { vertex.data() = 1; }

class pagerank :
  public parallelgraph::ivertex_program<graph_type, double> {

  double last_change;
public:

  edge_dir_type gather_edges(icontext_type& context,
                              const vertex_type& vertex) const {
    return parallelgraph::IN_EDGES;
  } // end of Gather edges


  /* Gather the weighted rank of the adjacent page   */
  double gather(icontext_type& context, const vertex_type& vertex,
               edge_type& edge) const 
  {
    vertex_data_type d = edge.source().data();
    size_t noe = edge.source().num_out_edges();
    double value = d /noe;
    return value;
    //return (edge.source().data() / edge.source().num_out_edges());
  }

  /* Use the total rank of adjacent pages to update this page */
  void apply(icontext_type& context, vertex_type& vertex,
             const gather_type& total) {

    const double newval = (1.0 - RESET_PROB) * total + RESET_PROB;
    last_change = (newval - vertex.data());
    vertex.data() = newval;
  }

  /* The scatter edges depend on whether the pagerank has converged */
  edge_dir_type scatter_edges(icontext_type& context,
                              const vertex_type& vertex) const {
    // In the dynamic case we run scatter on out edges if the we need
    // to maintain the delta cache or the tolerance is above bound.
    if( std::fabs(last_change) > TOLERANCE ) {
      return parallelgraph::OUT_EDGES;
    } else {
      return parallelgraph::NO_EDGES;
    }
  }

  /* The scatter function just signal adjacent pages */
  void scatter(icontext_type& context, const vertex_type& vertex,
               edge_type& edge) const 
  {
    if(last_change > TOLERANCE || last_change < -TOLERANCE) 
    {
        context.signal(edge.target().id());
        std::cout<<"signal happend\n";
    }
  }


}; // end of factorized_pagerank update functor



int main(int argc, char** argv) 
{
	//std::string file_path="/home/chen/ict/ParallelGraph/tests/dd";
  std::string file_path= argv[1];
	graph_type graph;
	graph.directed=true;
	graph.load(file_path,0.5);
	graph.finalize();
	
  std::cout << "#vertices: " << graph.num_vertices()
            << " #edges:" << graph.num_edges() << std::endl;

  parallelgraph::Engine<pagerank> engine(graph);
  engine.signal_all();
  engine.run();
  graph.show_vertex();
  return 0;
} // End of main


// We render this entire program in the documentation


