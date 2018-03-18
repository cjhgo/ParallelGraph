#include<iostream>
#include<parallel_graph.hpp>
#include <Eigen/Dense>
#include<queue>
#include<cmath>
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
	
  /**
   * \brief We store old and new messages in both directions as an
   * array of messages.  The particular message index is then computed
   * using the \ref message_idx function.
   */
  factor_type messages_[4];

  /**
   * \brief The function used to compute the message index in the edge
   * message array.
   */
  size_t message_idx(size_t source_id, size_t target_id, bool is_new) {
    return size_t(source_id < target_id)  + 2 * size_t(is_new);
  }

public:


  /**
   * \brief Get the new message value from source_id to target_id
   */
  factor_type& message(size_t source_id, size_t target_id) { 
    return messages_[message_idx(source_id, target_id, true)];
  }
  /**
   * \brief Get the old message value from source_id to target_id
   */
  factor_type& old_message(size_t source_id, size_t target_id) { 
     return messages_[message_idx(source_id, target_id, false)];
  }

  /**
   * \brief Set the old message value equal to the new message value
   */
  void update_old(size_t source_id, size_t target_id) { 
    old_message(source_id, target_id) = message(source_id, target_id);
  }
  
  /**
   * \brief Initialize the edge data with source and target having the
   * appropriate number of states.
   *
   * \param source_id the vertex id of the source
   * \param nsource the number of states the source vertex takes
   * \param target_id the vertex id of the target
   * \param ntarget the number of states the target vertex takes
   */
  void initialize(size_t source_id, size_t nsource, size_t target_id, size_t ntarget) {
    // ASSERT_GT(nsource, 0); ASSERT_GT(ntarget, 0);
    message(source_id, target_id).setZero(ntarget);
    old_message(source_id, target_id).setZero(ntarget);
    message(target_id, source_id).setZero(nsource);
    old_message(target_id, source_id).setZero(nsource);
  }

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
			vdata.potential(i) = std::log(values[i]);
		}
		
		// vdata.belief.resize(5);
		g.add_vertex(vid, vdata);

	}
}

void save_pred(graph<vertex, edge> & g)
{
	std::ofstream outfile("pred_res.tsv");	
	for(int i = 0; i < g.vertices.size();i++)
	{
		vertex& vdata = g.vertices[i];
		size_t g_id = g.get_global_id(i);
		std::stringstream strm;
		strm << g_id<< '\t';
		factor_type pred = vdata.belief;
		double sum = 0;
		for(int i = 0; i < pred.size(); ++i) 
		  sum += (pred(i) = std::exp(pred(i)));
		pred.array() /= sum;
		for(int i = 0; i < pred.size(); ++i) 
		  strm << pred(i) << (i+1 < pred.size()? '\t' : '\n');
		outfile<<strm.str();
	}
}
void convolve(const factor_type& cavity, 
                     factor_type& message)  {
  for(int i = 0; i < message.size(); ++i) {
    double sum = 0;
    for(int j = 0; j < cavity.size(); ++j) {
      sum += std::exp( cavity(j)  + ( i == j? 0 : -2 ) ); 
    }
    // To try and ensure numerical stability we do not allow
    // messages to underflow in log-space
    message(i) = (sum > 0)? std::log(sum) : std::numeric_limits<double>::min();
  }
} 

int main(int argc, char const *argv[])
{
    std::string file_path="smallsynth_edata.tsv";
    std::string vfile_path = "smallsynth_vdata.tsv";
    graph<vertex, edge> graphi;
    graphi.load(file_path);
    graphi.load_vertex(vfile_path, load_vdata);
    graphi.finalize();
    std::queue<int> run;
    for(int i = 0; i < graphi.vertices.size();i++)
    {
    	for(auto x: graphi.out_edges(i))
    	{
    		size_t source = i;
    		size_t target = x.first;
    		size_t edata_pos = x.second;
    		edge& edata = graphi.edges[edata_pos];
    		edata.initialize(source, 5, target, 5);    		
    	}   
    	run.push(i);
    }
    int iter_count = 0;
    while(not run.empty())
    {
    	int v = run.front();
    	size_t g_id = graphi.get_global_id(v);
		std::cout<<v<<"#########"<<g_id<<"\n";
		vertex& vv = graphi.vertices[v];
		std::cout<<vv.potential<<std::endl<<std::endl;
		factor_type total;
		total.resize(5);
		//Gather
		for(auto x: graphi.out_edges(v))
		{
			size_t target = x.first;
			size_t e_pos = x.second;
			edge& edata = graphi.edges[e_pos];
			edata.update_old(target, v);
			total += edata.old_message(target, v);
			
		}   
		for(auto x: graphi.in_edges(v))
		{
			size_t source = x.first;
			size_t e_pos = x.second;
			edge& edata = graphi.edges[e_pos];
			edata.update_old(source, v);
			total += edata.old_message(source, v);
		}

		//Apply
		vv.belief = vv.potential+total;
		std::cout<<vv.belief<<"\n\n"; 	
		vv.belief.array() -= vv.belief.maxCoeff();
		std::cout<<vv.belief<<"\n\n"; 	

		//Scater
		for(auto x: graphi.out_edges(v))
		{
			size_t target = x.first;
			size_t e_pos = x.second;
			edge& edata = graphi.edges[e_pos];
			// Divide (subtract in log space) out of the belief the old in
			// message to construct the cavity
			const factor_type& in_message = 
			  edata.message(target, v); 
			const factor_type& old_in_message = 
			  edata.old_message(target, v);

			factor_type cavity = vv.belief - old_in_message;
			// compute the new message by convolving with the Ising-Potts Edge
			// factor.
			factor_type& new_out_message = 
			  edata.message(v, target);

			const factor_type& old_out_message = 
			  edata.old_message(v, target);

			convolve(cavity, new_out_message);

			new_out_message.array() -= new_out_message.maxCoeff();			

			// Apply damping to the message to stabilize convergence.
			new_out_message = 0.1 * old_out_message + 0.9 * new_out_message;
			
			// Compute message residual
			const double residual = 
			  (new_out_message - old_out_message).cwiseAbs().sum();
			// Schedule the adjacent vertex
			if(residual > 0.01) run.push(target);
		}   
		for(auto x: graphi.in_edges(v))
		{
			size_t source = x.first;
			size_t e_pos = x.second;
			edge& edata = graphi.edges[e_pos];
			// Divide (subtract in log space) out of the belief the old in
			// message to construct the cavity
			const factor_type& in_message = 
			  edata.message(source, v); 
			const factor_type& old_in_message = 
			  edata.old_message(source, v);

			factor_type cavity = vv.belief - old_in_message;
			// compute the new message by convolving with the Ising-Potts Edge
			// factor.
			factor_type& new_out_message = 
			  edata.message(v, source);

			const factor_type& old_out_message = 
			  edata.old_message(v, source);

			convolve(cavity, new_out_message);

			new_out_message.array() -= new_out_message.maxCoeff();
			

			// Apply damping to the message to stabilize convergence.
			new_out_message = 0.1 * old_out_message + 0.9 * new_out_message;

			// Compute message residual
			const double residual = 
			  (new_out_message - old_out_message).cwiseAbs().sum();
			// Schedule the adjacent vertex
			if(residual > 0.01) run.push(source);
		}
		run.pop();
		std::cout<<"iteration : "<<iter_count<<std::endl;
		iter_count++;
    }
    save_pred(graphi);
    return 0;
}