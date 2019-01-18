#include <iostream>
#include <graphlab/rpc/dc.hpp>
#include <graphlab/rpc/dc_init_from_mpi.hpp>



void print(int val)
{
    std::cout<<"print the value "<<val<<std::endl;
}

int add_one(int val)
{
    return val+1;
}
int main(int argc, char ** argv) {
  graphlab::mpi_tools::init(argc, argv);

  graphlab::distributed_control dc;
  std::cout<<" i am the rank :"<<dc.procid()<<std::endl;
  dc.barrier();
  if(dc.procid() == 0)
  {
      dc.control_call(1, print , 22);
      std::cout<<"12 plus 1 is"<<dc.remote_request(1, add_one, 12)<<std::endl;
  }
  dc.barrier();
  graphlab::mpi_tools::finalize();
}