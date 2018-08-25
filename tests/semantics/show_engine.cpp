#include<iostream>
#include<util/dense_bitset.hpp>
#include<atomic>
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
 **/
class Engine
{

private:
    std::atomic<size_t> shared_vid_counter;
    parallelgraph::dense_bitset db;
};
int main(int argc, char const *argv[])
{
      //denbse_bitset 只输出有效位
  parallelgraph::fixed_dense_bitset<128> fdb;
  parallelgraph::dense_bitset db;
  
  fdb.set_bit(0);
  fdb.set_bit(32);
  fdb.set_bit(65);
  
  for(auto e: fdb)
    std::cout<<e<<std::endl;
  // 0 32 65

  db.resize(32);
  db.set_bit(1);
  db.set_bit(3);
  db.set_bit(7);
  for(auto e: db)
    std::cout<<e<<std::endl;
  // 1 3 7


  size_t vid = 70;
  size_t vid_block = fdb.containing_word(vid);
  parallelgraph::fixed_dense_bitset<64> local_bitset;
  local_bitset.initialize_from_mem(&vid_block, 8);
  for(auto e: local_bitset)
  std::cout<<64+e;

  std::cout<<std::endl;    
    return 0;
}
