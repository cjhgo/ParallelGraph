#include<iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
typedef uint32_t vertex_id_type;
// Jenkin's 32 bit integer mix from
// http://burtleburtle.net/bob/hash/integer.html
 uint32_t integer_mix(uint32_t a) {
  a -= (a<<6);
  a ^= (a>>17);
  a -= (a<<9);
  a ^= (a<<4);
  a -= (a<<3);
  a ^= (a<<10);
  a ^= (a>>15);
  return a;
}

size_t hash_edge (const std::pair<vertex_id_type, vertex_id_type>& e, const uint32_t seed = 5) {
      // a bunch of random numbers
#if (__SIZEOF_PTRDIFF_T__ == 8)
      static const size_t a[8] = {0x6306AA9DFC13C8E7,
        0xA8CD7FBCA2A9FFD4,
        0x40D341EB597ECDDC,
        0x99CFA1168AF8DA7E,
        0x7C55BCC3AF531D42,
        0x1BC49DB0842A21DD,
        0x2181F03B1DEE299F,
        0xD524D92CBFEC63E9};
#else
      static const size_t a[8] = {0xFC13C8E7,
        0xA2A9FFD4,
        0x597ECDDC,
        0x8AF8DA7E,
        0xAF531D42,
        0x842A21DD,
        0x1DEE299F,
        0xBFEC63E9};
#endif
      vertex_id_type src = e.first;
      vertex_id_type dst = e.second;
      return (integer_mix(src^a[seed%8]))^(integer_mix(dst^a[(seed+1)%8]));
    }
  
  int main(int argc, char const *argv[])
  {
  	/* code */
  	/* initialize random seed: */
  	srand (time(NULL));

  	 /* generate secret number between 1 and 10: */
  	for (int i = 0; i < 100; ++i)
  	{
  		int iSecret = rand()%100+1;
  		std::cout<<iSecret<<"####"<<integer_mix(iSecret)<<"####"<<integer_mix(iSecret)%100<<std::endl;
  	}
  	

  	vertex_id_type source = 0;
  	vertex_id_type target = 1;
  	typedef std::pair<vertex_id_type, vertex_id_type> edge_pair_type;
    const edge_pair_type edge_pair(std::min(source, target), 
            std::max(source, target));
  	std::cout<<hash_edge(edge_pair);
  	return 0;
  }