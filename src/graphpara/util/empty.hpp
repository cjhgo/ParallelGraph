#ifndef PARALLELGRAPH_EMPTY_HPP
#define PARALLELGRAPH_EMPTY_HPP
namespace parallelgraph 
{
 struct empty 
 {
    empty& operator+=(const empty&) { return *this; }
    empty& operator-=(const empty&) { return *this; }
 };
}
#endif