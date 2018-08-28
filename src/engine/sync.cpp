#ifndef PARALLELGRAPH_SYNC_HPP
#define PARALLELGRAPH_SYNC_HPP
#include<iostream>
#include<util/dense_bitset.hpp>
#include<vertex_program/ivertex_program.hpp>
#include<vertex_program/context.hpp>
#include<graph/graph_basic_types.hpp>
#include<atomic>
#include<cstdlib>
#include<ctime>
#include<pthread.h>

#define NUM_THREADS 5
#define NUM_TASKS 64*1000
namespace parallelgraph
{


/**
 * the semantics of sync engine
 * activestep
 * shared_vid_counter
 * while( iter < MAX)
 * {
 *  G G G G G ... G#run_multi(G)
 *  A A A A A ... A#run_multi(A)
 *  S S S S S ... S#run_multi(S)
 *  iter++
 * }
 * 
 **/
typedef void * (*THREADFUNCPTR)(void *);
template<typename VertexProgram>
class Engine
{
public:
 typedef VertexProgram vertex_program_type;
 typedef typename VertexProgram::gather_type gather_type;
 typedef typename VertexProgram::vertex_data_type vertex_data_type;
 typedef typename VertexProgram::edge_data_type edge_data_type;
 typedef typename VertexProgram::graph_type  graph_type;
 typedef typename graph_type::vertex_type          vertex_type;
 typedef typename graph_type::edge_type            edge_type;

private:
    typedef context<Engine> context_type;
    friend class context<Engine>;
    graph_type& graph;
    size_t nthreads;
    size_t nverts;
    size_t iteration_counter;
    size_t max_iterations;
    float start_time;
    std::vector<gather_type>  gather_accum;
    std::vector<vertex_program_type> vertex_programs;    
    std::atomic<size_t> shared_vid_counter;
    dense_bitset current_active_superstep;
    dense_bitset next_active_superstep;
    pthread_t threads[NUM_THREADS];
public:
    Engine(graph_type& graph):
    graph(graph),iteration_counter(0),max_iterations(-1)
    {

      nverts = graph.num_vertices();
      vertex_programs.resize(nverts);
      current_active_superstep.resize(nverts);
      next_active_superstep.resize(nverts);
      gather_accum.resize(nverts, gather_type());
    };   
    template<typename MemberFun>
    void execute_fun(MemberFun fn)
    {
         
      shared_vid_counter = 0;
      int rc;   
      for(int i = 0; i < NUM_THREADS; i++ ) 
      {
          //std::cout << "creating thread, " << i << std::endl;
          rc = pthread_create(&threads[i], NULL, (THREADFUNCPTR)fn, (void *)this);
          if (rc) 
          {
            std::cout << "Error:unable to create thread," << rc << std::endl;
            exit(-1);
          }
      }
      for(int j = 0; j < NUM_THREADS; j++ )
      {
        pthread_join(threads[j], NULL);
      }
    };
    void signal(lvid_type lvid)
    {
      next_active_superstep.set_bit(lvid);
    }
    void signal_all()
    {
      next_active_superstep.fill();
    }

    void run()
    {
      while(iteration_counter < max_iterations)
      {
        std::cout<<"the iteration: "<<iteration_counter<<"begin\n";
        current_active_superstep = next_active_superstep;
        next_active_superstep.clear();
        size_t active_vertices = current_active_superstep.popcount();
        std::cout<<"active vertice : "<<active_vertices<<std::endl;
        if( active_vertices <= 0)break;

        execute_fun(&Engine::execute_gathers);  
        execute_fun(&Engine::execute_applys);  
        execute_fun(&Engine::execute_scatters);  
        std::cout<<"the iteration: "<<iteration_counter<<"end\n";
        iteration_counter++;
      }
      

      std::cout<<"in the end: "<<shared_vid_counter<<"..."<<shared_vid_counter/64<<std::endl;
    };  
    void execute_gathers()
    {
      context_type context(*this);
      fixed_dense_bitset<64> local_bitset;
      while(1)
      {
        size_t vid_block_start = shared_vid_counter.fetch_add(8 * sizeof(size_t));                  
        if (vid_block_start >= nverts) 
        break;
        size_t vid_block = current_active_superstep.containing_word(vid_block_start);
        if (vid_block == 0) continue;
        local_bitset.clear();        
        local_bitset.initialize_from_mem(&vid_block, 8);
        for(auto e: local_bitset)
        {
          vid_type vid = vid_block_start + e;
          vertex_program_type vprog = vertex_programs[vid];
          vertex_type vertex = graph.vertex(vid);
          gather_type accum = gather_type();
          edge_dir_type gather_dir =  vprog.gather_edges(context, vertex);
          if(gather_dir == IN_EDGES || gather_dir == ALL_EDGES)
          {
            for(auto e: vertex.in_edges())
            {
              vid_type source = e.first;
              vid_type target = vid;
              edge_id_type eid = e.second;
              edge_type edge(graph, source,target, eid);
              accum += vprog.gather(context, vertex, edge);
            }
          }
          if(gather_dir == OUT_EDGES || gather_dir == ALL_EDGES)
          {
            for(auto e: vertex.out_edges())
            {
              vid_type source = vid;
              vid_type target = e.first;
              edge_id_type eid = e.second;
              edge_type edge(graph, source,target, eid);
              accum += vprog.gather(context, vertex, edge);
            }
          }
          gather_accum[vid] = accum;
        }
        
      }      
    };
    void execute_applys()
    {
      fixed_dense_bitset<64> local_bitset;
      context_type context(*this);
      while(1)
      {
        size_t vid_block_start = shared_vid_counter.fetch_add(8 * sizeof(size_t));                  
        if (vid_block_start >= nverts) 
        break;
        size_t vid_block = current_active_superstep.containing_word(vid_block_start);
        if (vid_block == 0) continue;
        local_bitset.clear();
        local_bitset.initialize_from_mem(&vid_block, 8);
        for(auto e: local_bitset)
        {
          vid_type vid = vid_block_start + e;
          vertex_program_type& vprog = vertex_programs[vid];
          vertex_type vertex = graph.vertex(vid);
          gather_type accum = gather_accum[vid];
          vprog.apply(context, vertex, accum);
        }
        
      }      
    };
    void execute_scatters()
    {
      fixed_dense_bitset<64> local_bitset;
      context_type context(*this);
      while(1)
      {
        size_t vid_block_start = shared_vid_counter.fetch_add(8 * sizeof(size_t));                  
        if (vid_block_start >= nverts) 
        break;
        size_t vid_block = current_active_superstep.containing_word(vid_block_start);
        if (vid_block == 0) continue;
        local_bitset.clear();
        local_bitset.initialize_from_mem(&vid_block, 8);
        for(auto e: local_bitset)
        {
          vid_type vid = vid_block_start + e;
          vertex_program_type vprog = vertex_programs[vid];
          vertex_type vertex = graph.vertex(vid);
          edge_dir_type gather_dir =  vprog.scatter_edges(context, vertex);
          if(gather_dir == IN_EDGES || gather_dir == ALL_EDGES)
          {
            for(auto e: vertex.in_edges())
            {
              vid_type source = e.first;
              vid_type target = vid;
              edge_id_type eid = e.second;
              edge_type edge(graph, source,target, eid);
              vprog.scatter(context, vertex, edge);
            }
          }
          if(gather_dir == OUT_EDGES || gather_dir == ALL_EDGES)
          {
            for(auto e: vertex.out_edges())
            {
              vid_type source = vid;
              vid_type target = e.first;
              edge_id_type eid = e.second;
              edge_type edge(graph, source,target, eid);
              vprog.scatter(context, vertex, edge);
            }
          }
        }
        
      }      
    };             
};
}
#endif