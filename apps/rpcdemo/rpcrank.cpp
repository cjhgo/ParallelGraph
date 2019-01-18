#include <iostream>
#include <graphlab/rpc/dc.hpp>
#include <graphlab/rpc/dc_init_from_mpi.hpp>

int main(int argc, char ** argv) {
  graphlab::mpi_tools::init(argc, argv);

  graphlab::distributed_control dc;
  std::cout<<" i am the rank :"<<dc.procid()<<std::endl;
  dc.barrier();
  std::cout<<" after barrier i am the rank :"<<dc.procid()<<std::endl;
  graphlab::mpi_tools::finalize();
}