#include <iostream>
#include <graphlab/rpc/dc.hpp>
#include <graphlab/rpc/dc_init_from_mpi.hpp>
#include <graphlab/rpc/buffered_exchange.hpp>


typedef size_t vid_type;
typedef typename graphlab::buffered_exchange<vid_type> send_vid_type;
typedef typename graphlab::buffered_exchange<vid_type>::buffer_type send_vid_buffer_type;


int main(int argc, char ** argv) {
  graphlab::mpi_tools::init(argc, argv);

  graphlab::distributed_control dc;
  std::cout<<" i am the rank :"<<dc.procid()<<std::endl;
  dc.barrier();
  send_vid_type send_vid(dc);
  for(size_t i = dc.procid(); i < 100; i+=dc.numprocs())
  {
      
      graphlab::procid_t target_proc = dc.procid() % 2;
      std::cout<<"i am rank "<<dc.procid()<<",i send "<<i<<" to "<<target_proc<<std::endl;
      send_vid.send(target_proc, i);
  }
  send_vid.flush();
  dc.barrier();
  vid_type recv_vid;
  graphlab::procid_t proc;
  send_vid_buffer_type send_vid_buffer;
  std::cout<<"run to here\n";

  while(send_vid.recv(proc, send_vid_buffer,true))
  {
      std::cout<<"rank "<<dc.procid()<<"received ";
      for(size_t i = 0; i < send_vid_buffer.size(); i++)
      {
          std::cout<<send_vid_buffer[i]<<"\t";
      }
      std::cout<<"from " <<proc<<std::endl;
  }      
  send_vid.clear();  
  dc.barrier();
  graphlab::mpi_tools::finalize();
}