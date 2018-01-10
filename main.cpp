#include <string>

#include "graph.hpp"
#include <fstream>
#include <pthread.h>
#include <unistd.h>
#define NUM_THREADS 5


void *Printvertex(void *graphi) {
	graph<double> * somegraph =(graph<double> *)graphi;
	int x =somegraph->get_vertex_to_run();
	if(x != -1)
	{
		std::cout<<x<<"!!!!"<<std::endl;
		somegraph->resotore_vertex(x);
	}
	else
	{
		std::cout<<x<<"???"<<std::endl;
		sleep(2);
	}
	
	std::cout<<"the length of queue is "<<somegraph->activate_vertexes.size()<< std::endl;
   pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
	std::string file_path="dd";
	graph<double> graphi;
	graphi.directed=true;
	graphi.load(file_path, 1.1);
	graphi.show_graph();
	pthread_t threads[NUM_THREADS];
   	int rc;
   	int i;
   while(graphi.activate_vertexes.size() > 0)
   {
   	for( i = 0; i < NUM_THREADS; i++ ) {
   	   std::cout << "main() : creating thread, " << i << std::endl;
   	   rc = pthread_create(&threads[i], NULL, Printvertex, (void *)&graphi);
   	   
   	   if (rc) {
   	      std::cout << "wocao," << rc << std::endl;
   	      exit(-1);
   	   }
   	}
   	pthread_join( threads[0], NULL);
   	pthread_join( threads[1], NULL);
   	pthread_join( threads[2], NULL);
   	pthread_join( threads[3], NULL);
   	pthread_join( threads[4], NULL);
   }

   // pthread_exit(NULL);
	// graphi.crate_mutex();
	// for(int i = 0 ; i <= graphi.current_local_id;i++)
	// {
	// 	if(abs(graphi.matrix[0][i]-0) > 0.001)
	// 		graphi.matrix_mutex[0][i].lock();
	// }
	// for(int i = 0 ; i <= graphi.current_local_id;i++)
	// {
	// 	if(abs(graphi.matrix[i][0]-0) > 0.001)
	// 		graphi.matrix_mutex[i][0].lock();
	// }
	return 0;
}
