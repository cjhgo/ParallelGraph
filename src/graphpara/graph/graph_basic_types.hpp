#ifndef PARALLELGRAPH_GRAPH_BASIC_TYPES
#define PARALLELGRAPH_GRAPH_BASIC_TYPES
#include<stdint.h>
namespace parallelgraph{

 typedef uint32_t vertext_id_type;
 typedef vertext_id_type vid_type;
 typedef vertext_id_type lvid_type;
 typedef lvid_type edge_id_type;

 enum edge_dir_type
 {
     NO_EDGES = 0,
     IN_EDGES = 1,
     OUT_EDGES = 2,
     ALL_EDGES = 3
 };
}
#endif