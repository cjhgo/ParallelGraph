#include<iostream>
#include<util/dense_bitset.hpp>
#include<atomic>
#include<cstdlib>
#include<ctime>
#include<pthread.h>
#include<boost/functional.hpp>
#define NUM_THREADS 5
#define NUM_TASKS 64*1000
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
class Engine
{

private:
    std::atomic<size_t> shared_vid_counter;
    parallelgraph::dense_bitset active;
    pthread_t threads[NUM_THREADS];
    void set_task()
    {
      std::srand(std::time(nullptr)); 
      for(int i = 0; i < NUM_TASKS;i++)
      {
        double r = ((double) rand() / (RAND_MAX));
        if(1 or r > 0.6)  
        {
          active.set_bit(i);
        }
      }
      
    };
public:
    Engine()
    {
      active.resize(NUM_TASKS);
      set_task();
      std::cout<<"the count of bit:"<<active.popcount()<<std::endl;
    };   
    void task()
    {
      while(1)
      {
        size_t vid_block_start = shared_vid_counter.fetch_add(8 * sizeof(size_t));                  
        if (vid_block_start >= NUM_TASKS) 
        break;
        size_t vid_block = active.containing_word(vid_block_start);
        parallelgraph::fixed_dense_bitset<64> local_bitset;
        local_bitset.initialize_from_mem(&vid_block, 8);
        for(auto e: local_bitset)
        std::cout<<"##:"<<vid_block_start+e<<std::endl;     
      }      
    };
    template<typename MemberFun>
    void execute_fun(MemberFun fn)
    {
         
      shared_vid_counter = 0;
      int rc;   
      for(int i = 0; i < NUM_THREADS; i++ ) 
      {
          std::cout << "creating thread, " << i << std::endl;
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
    void run()
    {
      execute_fun(&Engine::task);

      std::cout<<"in the end: "<<shared_vid_counter<<"..."<<shared_vid_counter/64<<std::endl;
    };   
};
int main(int argc, char const *argv[])
{
  Engine engine;
  engine.run();
  return 0;
}
